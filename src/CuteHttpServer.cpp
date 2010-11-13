/*******************************************************************************/
/**
@file       CuteHttpServer.cpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3  
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains implementation of the CuteHttpServer class
*
@history
* 2010/28/10  mrkkrj 
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

/* INCLUDE FILES **************************************************************/

#include "CuteHttpServer.hpp" 
#include "CuteHttpParser.hpp" 
#include "CuteTrace.hpp"

//#include "../Threads/ThreadPool.hpp" 
//#include "../Config/ConfigSettings.hpp"

#include <qtcpserver.h>
#include <qtcpsocket.h>
#include <qfileinfo.h>
#include <QString>
#include <qmetatype.h>

#include <algorithm>
#include <iostream>
#include <cassert>

using  namespace ibkrj::yawf4q;
using namespace std;


/* LOCAL FUNCTIONS ************************************************************/

/* CLASS DECLARATION **********************************************************/
/**
  Helper functor for searching the request list
*
*******************************************************************************/
struct EqualReqId 
{
    EqualReqId(int reqId) : m_reqId(reqId) {}

    bool operator() (std::pair<const std::string, RequestData> elem) 
    {
        // return elem.second.first.internalReqId == m_reqId;
        return elem.second.internalReqId == m_reqId;

    }
private:
    int m_reqId;
};


/* CLASS DECLARATION **********************************************************/
/**
  Helper functor for dealing with the request list
*
*******************************************************************************/
struct ResetConnIdIf 
{
    ResetConnIdIf(int connId) : m_connId(connId) {}
    
    void operator() (std::pair<const std::string, RequestData>& elem) 
    {
        //if(elem.second.first.clientConnId == m_connId)
        //    elem.second.first.clientConnId = -1; // matching: reset!

        if(elem.second.clientConnId == m_connId)
            elem.second.clientConnId = -1; // matching: reset!
    }
private:
    int m_connId;
};


/* STATIC INITIALIZATION ******************************************************/

namespace
{
    const int c_httpServerPort = 8080; // default value 
    const string c_httpLineEnd = "\r\n";
}

// ????????????????
// the one and only instance:
CuteHttpServer* CuteHttpServer::m_instance = 0;


/* PUBLIC *********************************************************************/

/* METHOD *********************************************************************/
/**
  Constructor of the CuteHttpServer class. 
*
@param: none
*******************************************************************************/
CuteHttpServer::CuteHttpServer(int serverPort)
    : m_tcpServer(new QTcpServer),
      m_connCount(0),
      m_maxNoOfClients((unsigned)-1),
      m_serverPort(serverPort)
{

    //// connect internal signals
    //connect(this, SIGNAL(notifyReqCompleted(ThreadGraph*)),  
    //        this, SLOT(requestCompleted(ThreadGraph*)));    


    // the server port number:
    if(serverPort == 0)    
    {
        m_serverPort = c_httpServerPort;
            //ConfigSettings::getConfigInt("web-interface-port", c_httpServerPort);
    }

#if 0
    // the server strategy: 
    //   -- thread_pool, thread_per_request, single_threaded
    m_serverServingMode = 
        ConfigSettings::getConfigString("web-server-req-handling", "thread_per_request");
#endif


    if(TR_WEBIF) 
    {
        TRACE_INFO2("http_srv: WebInterface will listen on port=", m_serverPort);
        TRACE_INFO2("http_srv: request serving mode will be:", m_serverServingMode);
    }
}


/* METHOD *********************************************************************/
/**
  Destructor of the CuteHttpServer class. 
*
*******************************************************************************/
CuteHttpServer::~CuteHttpServer()
{
    // empty
}


/* METHOD *********************************************************************/
/**
  This method opens a server socket and waits for the HTTP clients to connect. 
*
@param [out] errString: description of the error or empty
*
@return: success/failure indication
*******************************************************************************/
bool CuteHttpServer::start(string& errString)
{
    // start listening for new clients
    if(!m_tcpServer->listen(QHostAddress::Any, m_serverPort))
    {        
        TRACE_ERR2("http_srv: cannot open the webserver socket: ", 
                   m_tcpServer->errorString().toLatin1().constData());

        errString = "Cannot bind to the webserver socket, check if the port is free.";
        return false;
    }

    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(acceptNewConn()));

    // ok!
    return true;
}


#if 0
/* METHOD *********************************************************************/
/**
  This method will send a signal to the CuteHttpServer from the invoker's thread 
  context indicating that a thread graph finished its execution.
*
@param threadGraph: 
*
@return: none
*******************************************************************************/
void CuteHttpServer::setReqCompleted(ThreadGraph* threadGraph)
{
    emit notifyReqCompleted(threadGraph);
}

#endif


//-------------------------------
//
//  Client connection handling
//
//-------------------------------


/* PROTECTED ******************************************************************/

/* PRIVATE ********************************************************************/

/* METHOD *********************************************************************/
/**
  This method will be invoked by Qt framework for a new incoming client 
  connection attempt.
*
@param: none
*
@return: none
*******************************************************************************/
void CuteHttpServer::acceptNewConn()
{
    QTcpSocket *conn = m_tcpServer->nextPendingConnection();

    // max clients?
    if(m_maxNoOfClients != 0 && 
       m_connections.size() >= m_maxNoOfClients)
    {
        TRACE_ERR2("http_srv: max number of clients exceeded, rejecting connection from:", 
                    conn->peerAddress().toString());

        CuteSrvRequest dummy;
        string resp = m_parser.makeErrorResp(dummy, 500, "too many clients");

        conn->write(resp.c_str(), resp.size());
        conn->flush();

        // let Qt delete the connection object:
        connect(conn, SIGNAL(disconnected()), conn, SLOT(deleteLater()));
        conn->disconnectFromHost();
        return;
    }

    // accept!
    connect(conn, SIGNAL(disconnected()), this, SLOT(removeClosedConn()));
    connect(conn, SIGNAL(readyRead()), this, SLOT(readRequestString()));

    int connID = saveNewConn(conn);

    if(TR_WEBIF || INFO_LVL)
    {
        TRACE_INFO3("http_srv: client conn accepted, connection's IpAddr/clientId=", 
                    conn->peerAddress().toString(), connID);
        if(TR_WEBIF) TRACE2("http_srv: opened conns=", m_connCount);
    }
}


/* METHOD *********************************************************************/
/**
  This method will be invoked by Qt framework when a client connection gets 
  disconnected.
*
@param: none
*
@return: none
*******************************************************************************/
void CuteHttpServer::removeClosedConn()
{
    // who sent the Qt signal?
    QTcpSocket* caller = dynamic_cast<QTcpSocket*>(sender());
    
    vector<QTcpSocket*>::iterator iter;
    QTcpSocket* conn = 0;

    if(caller == 0)
    {
        // 1. invoked by Qt on shutdown!
        //  -- find first closing conn:
        for(iter = m_connections.begin(); iter != m_connections.end(); iter++)
        {
            // empty slot?
            if(*iter == 0) continue;

            if((*iter)->state() == QAbstractSocket::UnconnectedState)
            {
                conn = *iter;
                break;
            }
        }
    }
    else
    {
        // 2. normal case, client exited, TCP object called back
        iter = find(m_connections.begin(), m_connections.end(), caller);

        if(iter != m_connections.end())
        {
            conn = *iter;
            assert(conn == caller);
        }
    }       

    // found?    
    if(conn != 0)
    {
        assert(iter != m_connections.end());    
        int connID = delSavedConn(conn);

        //delete conn; --> NO-GO!, Qt will need it later in this event handler!
        conn->deleteLater();  // Qt will delte it!        

        if(TR_WEBIF || INFO_LVL)
        {
            TRACE_INFO3("http_srv: client conn closed, connection's IpAddr/clientId=", 
                        conn->peerAddress().toString(), connID);
            if(TR_WEBIF) TRACE2("http_srv: opened conns=", m_connCount);
        }

        // reset request's references:
        for_each(m_requests.begin(), m_requests.end(), ResetConnIdIf(connID));      
    }
    else
    {
        TRACE_ERR("http_srv: conn. closing signalled but no closed conn. found!");
    }        
}


/* METHOD *********************************************************************/
/**
  This method will be invoked by Qt framework to read the incoming client HTTP 
  command from its client connection.
*
@param: none
*
@return: none
*******************************************************************************/
void CuteHttpServer::readRequestString()
{
    // who sent the Qt signal?
    QTcpSocket* conn = dynamic_cast<QTcpSocket*>(sender());
    assert(conn != 0);

    int connID = findSavedConn(conn);        
    assert(connID != -1);

    // read from socket
    char buff[1024 + 1];
    size_t readBytes;
    string leftoverBytes;
        
    while(true)
    {       
        readBytes = conn->read(buff, 1024);

        if(readBytes < 0) 
        {
            TRACE_ERR2("http_srv: error when reading from socket:", conn->errorString());
            break;
        }

        if(readBytes == 0)
            continue; // reader timeout

        string input(buff, readBytes);
        size_t pos = 0, lastpos = 0;
        size_t endMarkerSz = 2;
        bool leftover = false;
        size_t offset = 0;

        // partition input into single requests
        while(true)
        {
            // read HTTP lines:
            while(true)
            {
                pos = input.find(c_httpLineEnd, pos + 1);
               
                if(pos != string::npos)
                {
                    // end of req?
                    if(lastpos + 2 == pos)
                        // empty line!
                        break;
                    else
                    {
                        lastpos = pos;
                        continue;
                    }
                }
                else
                {
                    // incomplete!
                    leftover = true;
                    break;
                }
            }
            
            if(leftover)
            {
                TRACE_ERR(" TODO::: ############## - leftover");

                // return internal SVR error at the moment
                CuteSrvRequest dummy;
                string resp = 
                    m_parser.makeErrorResp(dummy, 500, "SORRY:: bad input parsing, lefover found!!!"); 

                // respond with 500
                conn->write(resp.c_str(), resp.size());
                conn->flush();
                break;
            }

            string req;
            string resp;            
            size_t len = 0;

            len = pos + endMarkerSz - offset;
            req.assign(input.c_str(), offset, len);                       

            if(TR_WEBIF) 
                TRACE2("http_srv: Request received=", req); 

            // start processing
            unsigned needMore = processRequest(req.c_str(), connID, resp);

            if(needMore)
            {
                string postData;

                // data already in buffer?
                if(pos + endMarkerSz + needMore <= readBytes)
                {
                    postData.append(input, pos,  endMarkerSz + needMore);
                }
                else
                {
                    TRACE_ERR(" TODO::: ############## - wait for more POST data");

                    // return internal SVR error at the moment
                    CuteSrvRequest dummy;
                    string resp = 
                        m_parser.makeErrorResp(dummy, 500, "SORRY:: bad input parsing, wait for POST data!!!"); 

                    // respond with 500
                    conn->write(resp.c_str(), resp.size());
                    conn->flush();
                    break;
                }

                processRequest(req.c_str(), connID, resp, &postData);
            }

            // respond
            conn->write(resp.c_str(), resp.size());
            conn->flush();

            // next request possible?                                
            pos += endMarkerSz;  

            if(pos == input.size())
                break;

            offset = pos;
        }

        // ????? ??? ? ?? 

        // all messages read!
        break;
    }

    // all data read!
    return;
}


/* METHOD *********************************************************************/
/**
  This method will parse the incoming XML request and start a thread pipelinde 
  to execute it.

  We'll check for the demo licence expiry for each request. The general licence 
  check, however, will be don in the startup code.
*
@param requestStr: client's XML request string 
@param connId: client's TCP connection ID 
@param [out] responseStr: the XML response to be sent back
*
@return: 0 if processing done, numer of lacking bytes if contents follows
*******************************************************************************/
int CuteHttpServer::processRequest(const char* requestStr, int connId, 
                                   string& responseStr, string* contentData)
{
    CuteSrvRequest reqData;    
    reqData.clientConnId = connId;   
    reqData.reqContent = contentData;

    string respData;
    int bytesMissing = 0;
    
    // 1. check validity
    bool ok = m_parser.parseRequest(requestStr, reqData, respData, bytesMissing);

    if(!ok)
    {
        // bad request
        responseStr = m_parser.makeErrorResp(reqData, reqData.httpErr, respData);
        TRACE_ERR2("http_srv: invalid request rejected, clientId=", reqData.clientConnId);
        return 0;
    }  
    
    if(bytesMissing != 0 && contentData == 0)
        // need more data
        return bytesMissing;


    // 2. look what request
    switch(reqData.type)
    {
    case HTTP_GET_REQUEST:
        ok = m_requestDisp.handleGet(reqData, respData);
        break;

    case HTTP_POST_REQUEST:
        ok = m_requestDisp.handlePost(reqData, respData);
        break;

    default:     
        // all the rest
        reqData.httpErr = 501; // not impl!
        ok = false;
    }

    // 3. errors?
    if(!ok)
        responseStr = m_parser.makeErrorResp(reqData, reqData.httpErr, respData); 
    else
        responseStr = m_parser.makeOkResp(reqData, respData);

    // ready
    if(TR_WEBIF) TRACE2("http_srv: response=\n", responseStr);
    return 0;  
}


#if 0
/* METHOD *********************************************************************/
/**
  This method will be indirectly invoked by the ThreadPool in case all input 
  stream data are processed and will terminate the concerned thread pipeline.

  As it is always invoked by a Qt signal, we don't need to guard internal data 
  structures with locks!
*
@param threadGraph: 
*
@return: none
*******************************************************************************/
void CuteHttpServer::requestCompleted(ThreadGraph* threadGraph)
{   
    int requestId = threadGraph->id;

    // close locally
    if(TR_WEBIF) TRACE2("http_srv: request completion notified, reqId=", requestId);

    notifyFinishedRequest(requestId);

    // cleanup and recycle the thread graph:   
    threadGraph->endOfData();
}
#endif


/* METHOD *********************************************************************/
/**
  This method will store the reference to an new TCP conn object.
*
@param conn: the new Qt connection object
*
@return: the index for this conn
*******************************************************************************/
int CuteHttpServer::saveNewConn(QTcpSocket* conn)
{
    vector<QTcpSocket*>::iterator iter = 
        find(m_connections.begin(), m_connections.end(), (QTcpSocket*)0);

    if(iter == m_connections.end())
    {
        // no empty slot available!
        m_connections.push_back(0);
        iter = --m_connections.end();
    }

    // save reference
    *iter = conn;
    m_connCount++;

    // ID equals to the offset:
    return distance(m_connections.begin(), iter);
}


/* METHOD *********************************************************************/
/**
  This method will remove the reference to an TCP conn object.
*
@param conn: the Qt connection object
*
@return: the index for this conn
*******************************************************************************/
int CuteHttpServer::delSavedConn(QTcpSocket* conn)
{
    vector<QTcpSocket*>::iterator iter = 
        find(m_connections.begin(), m_connections.end(), conn);

    assert(iter != m_connections.end()); // don't give me crap!

    // free the slot:
    *iter = 0;
    m_connCount--;

    // ID equals to the offset:
    return distance(m_connections.begin(), iter);
}


/* METHOD *********************************************************************/
/**
  This method will find the stored TCP conn object.
*
@param conn: the Qt connection object
*
@return: the index for this conn, -1 if none found
*******************************************************************************/
int CuteHttpServer::findSavedConn(QTcpSocket* conn)
{
    vector<QTcpSocket*>::iterator iter = 
        find(m_connections.begin(), m_connections.end(), conn);

    if(iter == m_connections.end())
        return -1; 
    else
        // ID equals to the offset:
        return distance(m_connections.begin(), iter);
}


//-------------------------------
//
//  Remaining functionality
//
//-------------------------------


#if 0
/* METHOD *********************************************************************/
/**
  This method will find the result data for a given request.  
*
@param requestId: the internal ID of the request 
*
@return: the session results OR nullptr if no such session exists
*******************************************************************************/
SessionResults* CuteHttpServer::getSessionResultData(const std::string sessionID)
{
    // Sync for concurrent requests? 
    //  - not needed, we only have serialized Qt signals!
    map<string, RequestData>::iterator iter = m_requests.find(sessionID);

    if(iter == m_requests.end())
    {
        if(TR_WEBIF) 
            TRACE2("http_srv: session results not found, unknown session=", sessionID);
        return 0;
    }
    else
        return iter->second.second;
}


/* METHOD *********************************************************************/
/**
  This method will find the result data for a given request
*
@param requestId: the internal ID of the request 
*
@return: the session results OR nullptr if no such session exists
*******************************************************************************/
SessionResults* CuteHttpServer::getSessionResultData(int requestId)
{
    // Sync for concurrent requests? 
    //  - not needed, we only have serialized Qt signals!
    map<string, RequestData>::iterator iter =
                    find_if(m_requests.begin(), m_requests.end(), EqualReqId(requestId));

    if(iter == m_requests.end())
    {
        TRACE_ERR2("http_srv: session results for an requestId not found, requestId=",
                   requestId);
        return 0;
    }
    else
        return iter->second.second;
}
#endif


/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
