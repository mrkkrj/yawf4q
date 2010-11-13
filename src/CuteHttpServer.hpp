/*******************************************************************************/
/**
@file       CuteHttpServer.hpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3 
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains the definition of the CuteHttpServer class.
@see
*
@history
* 2010/28/10  mrkkrj   
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

#ifndef CUTE_HTTP_SERVER_H
#define CUTE_HTTP_SERVER_H

/* INCLUDE FILES **************************************************************/

#include <qobject.h>

#include <string>
#include <vector>
#include <map>

#include "CuteHttpParser.hpp"
#include "CuteHttpDispatcher.hpp"


/* FORWARD DECLARATIONS *******************************************************/

class QTcpServer;
class QTcpSocket;

namespace ibkrj 
{ 
namespace yawf4q
{

struct RequestData;     // OPEN TODO::: remove this old data types!!!
struct CuteSrvRequest;  // OPEN TODO::: needed?


/* CONSTANT DECLARATIONS ******************************************************/

/* CLASS DECLARATIONS *********************************************************/

/* CLASS DECLARATION **********************************************************/
/**
  This class is ....
*
@author: ib-krajewski.de 
*
@pattern
*******************************************************************************/
class CuteHttpServer
    : public QObject
{
    Q_OBJECT

public:
    CuteHttpServer(int serverPort = 0);
    ~CuteHttpServer();

    static CuteHttpServer* instance() { return m_instance; }

    bool start(std::string& errString);
    void stop();

public slots:
    // none
   
private slots:
    // will be only invoked via Qt signals:
    void acceptNewConn();
    void removeClosedConn();
    void readRequestString();
    //void requestCompleted(ThreadGraph* threadGraph);

signals:
    //void notifyReqCompleted(ThreadGraph* threadGraph);

private:
    int processRequest(const char* requestStr, int connId, std::string& responseStr,
                       std::string* contentData = 0);

    int saveNewConn(QTcpSocket* conn);
    int delSavedConn(QTcpSocket* conn);
    int findSavedConn(QTcpSocket* conn);
       
    void notifyFinishedRequest(int requestId);
    void shutdownAppl();

    // data members:
    QTcpServer* m_tcpServer; 
    CuteHttpParser m_parser;
    CuteHttpDispatcher m_requestDisp;

    int m_serverPort;
    std::string m_serverServingMode;

private:
    std::map<std::string, RequestData> m_requests;
    std::vector<QTcpSocket*> m_connections;
    
    static CuteHttpServer* m_instance;
    unsigned m_connCount;
    unsigned m_maxNoOfClients;
};


/* INLINE METHODS *************************************************************/

/* TEMPLATE METHODS ***********************************************************/

/* OPERATORS ******************************************************************/

/* TYPE DECLARATIONS **********************************************************/

struct RequestData
{
    int reqID;
    int internalReqId;
    int clientConnId;
};

struct CuteSrvRequest
{
    int reqID;
    int clientConnId;
    int type;
    std::string* reqContent;
    std::map <std::string, std::string> formularData;

    std::string requestedURI;
    int httpErr;

    CuteSrvRequest() 
        : reqID(-1), clientConnId(-1), type(-1), reqContent(0), httpErr(-1) {}   
};


// no Qt definitions!!! why???
enum HttpReqType
{
    HTTP_OPTIONS_REQUEST,
    HTTP_GET_REQUEST,
    HTTP_HEAD_REQUEST,
    HTTP_POST_REQUEST,
    HTTP_PUT_REQUEST,
    HTTP_DELETE_REQUEST,
    HTTP_TRACE_REQUEST,
    HTTP_CONNECT_REQUEST,
    HHTP_EXTENSION_REQUESTs, // ??? which ones?
    HHTP_INVALID_REQUEST,
    HHTP_NYI_REQUEST        // marker for not implemented!
};


}}

#endif /* XXX_H */

/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
