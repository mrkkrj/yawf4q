/*******************************************************************************/
/**
@file       CuteHttpDispatcher.cpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3  
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains implementation of the CuteHttpDispatcher class
*
@history
* 2010/28/10  mrkkrj  
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

/* INCLUDE FILES **************************************************************/

#include "CuteHttpDispatcher.hpp" 
#include "CuteHttpServer.hpp" // fro CuteSrvRequest 
#include "CuteHttpHandler.hpp"
#include "CuteTrace.hpp"

#include <QString>

//#include <algorithm>
#include <cassert>
#include <fstream>

using namespace ibkrj::yawf4q;
using namespace std;


/* LOCAL FUNCTIONS ************************************************************/

/* STATIC INITIALIZATION ******************************************************/

namespace 
{
    string c_configFileSep = " :: ";
}

map<string, CuteHttpHandler*>* CuteHttpDispatcher::m_prototypes;
map<string, SessionData*> CuteHttpDispatcher::m_sessions;
CuteHttpDispatcher* CuteHttpDispatcher::m_instance = 0; // ??? needed?


/* PUBLIC *********************************************************************/

/* METHOD *********************************************************************/
/**
  Constructor of the class. 
*
@param: none
*******************************************************************************/
CuteHttpDispatcher::CuteHttpDispatcher()
{
    // OPEN:: make configurable?
    string filename = "Yawf4qConfig.txt";

    ifstream is;
    is.open(filename.c_str());
    is.setf(ios::skipws); // eat white!!! ??? not working under Windows???

    if(!is)
    { 
        // tell me why...
        // OPEN TODO::: 

        TRACE_ERR2("http_serv: cannot open the server config file:", filename);
        return; 
    }

    // scan the config entries
    string* tmp;

    for(;;)
    {
        if(readCfgEntryLine(is, tmp) != true)
            break; // EOF

        size_t pos = tmp->find(c_configFileSep);
        
        if(pos == string::npos)
        {
            TRACE_ERR2("http_srv: skipping bad config line:", *tmp);
        }
        else
        {
            string re = tmp->substr(0, pos);
            string handler = tmp->substr(pos + c_configFileSep.size());

            // OPEN TODO::: trim spaces!!! !!!!!!!

            QRegExp rex(re.c_str());
            if(!rex.isValid())
            {
                TRACE_ERR2("http_srv: skipping bad config line:", *tmp);
            }
            else
            {                    
                m_handlers.push_back(make_pair(rex, handler));
                TRACE3("http_srv: handle installed, path/name=", re, handler);
            }            
        }    

        // line handled:
        delete tmp;
    }

    // done
    is.close();
}


/* METHOD *********************************************************************/
/**
  Destructor of the class. 
*
*******************************************************************************/
CuteHttpDispatcher::~CuteHttpDispatcher()
{
    // empty
}


/* METHOD *********************************************************************/
/**
  This method will dispatch the GET request to its handler
*
@param [inout] reqData: ...
@param [out] error: error description for an invalid request
*
@return: can we serve this request?
*******************************************************************************/
bool CuteHttpDispatcher::handleGet(CuteSrvRequest& reqData, string& respData)
{
    // find the reqistered handler
    if(m_handlers.empty())
    {            
        // greeting screen!
        respData = "Welcome to Cu-te HTTP Server!!!!!!!!!!!!!!!!";
        return true;
    }

    CuteHttpHandler* handler = findHandler(reqData, respData);

    if(handler == 0)
        return false;

    try
    {
        handler->process(reqData, respData);

        delete handler;
        return true;
    }
    catch(...)
    {
        // OPEN TODO::: better catch, better error screen!!
        delete handler;

        respData = "GET Request handler trew an exception!";
        reqData.httpErr = 500;
        return false;
    }
}


/* METHOD *********************************************************************/
/**
  This method will ...
*
@param [inout] reqData: ...
@param [out] respData: ...
*
@return: can we serve this request?
*******************************************************************************/
bool CuteHttpDispatcher::handlePost(CuteSrvRequest& reqData, string& respData)
{
    CuteHttpHandler* handler = findHandler(reqData, respData);

    if(handler == 0)
        return false;

    try
    {
        handler->process(reqData, respData);

        delete handler;
        return true;
    }
    catch(...)
    {
        // OPEN TODO::: better catch, better error screen!!
        delete handler;

        respData = "POST Request handler trew an exception!";
        reqData.httpErr = 500;
        return false;
    }
}


/* METHOD *********************************************************************/
/**
  This method will ...
*
*******************************************************************************/
CuteHttpDispatcher* CuteHttpDispatcher::initialized() 
{
    if(m_instance == 0)
        m_instance = new CuteHttpDispatcher;

    return m_instance;
}


/* PROTECTED ******************************************************************/

/* PRIVATE ********************************************************************/

/* METHOD *********************************************************************/
/**
  This method will ...
*
@param [inout] reqData: ...
@param [out] error: error description for an invalid request
*
@return: can we serve this request?
*******************************************************************************/
CuteHttpHandler* CuteHttpDispatcher::findHandler(CuteSrvRequest& reqData, 
                                                 string& respData) const
{
    // find the reqistered handler
    list<pair<QRegExp, string> >::const_iterator iter;

    for(iter = m_handlers.begin(); iter != m_handlers.end(); iter++)
    {
        if(iter->first.indexIn(reqData.requestedURI.c_str()) != -1)
            // matched!
            break;
    }

    if(iter == m_handlers.end())
    {
        TRACE2("http_srv: handler not configured for that request, request=", reqData.requestedURI);

        reqData.httpErr = 404; //  not found!
        respData = "Bad server configuration, handler not configured for that request";        
        return 0;
    }
    else
    {
        // TODO:: try... delete: use smart ptr???
        
        // get a fresh instance!
        CuteHttpHandler* handler = getHttpHandlerInstance(iter->second);

        if(handler == 0)
        {
            TRACE2("http_srv: handler implementation not found, name=", iter->second);

            reqData.httpErr = 500; //  config error
            respData = "Bad server configuration, handler implementation not found!";
            return 0;
        }
        else
            return handler;
    }
}


/* METHOD *********************************************************************/
/**
  This method will ...
*
@param [inout] reqData: ...
@param [out] error: error description for an invalid request
*
@return: can we serve this request?
*******************************************************************************/
CuteHttpHandler* CuteHttpDispatcher::getHttpHandlerInstance(const std::string& handlerId) const
{
    // not initalized!
    if(m_prototypes == 0)
        return 0;

    map<string, CuteHttpHandler*>::
        iterator iter = m_prototypes->find(handlerId);

    if(iter == m_prototypes->end())
        return 0;
    else
        return iter->second->clone();
}


/* METHOD *********************************************************************/
/**
  This method will ...
*
@param [inout] reqData: ...
@param [out] error: error description for an invalid request
*
@return: can we serve this request?
*******************************************************************************/
void CuteHttpDispatcher::registerHandler(const string& name, CuteHttpHandler* proto)
{
    if(m_prototypes == 0)
        m_prototypes = new map<string, CuteHttpHandler*>;

    m_prototypes->insert(make_pair(name, proto));
}


/* METHOD *********************************************************************/
/**
  This method will ...
*
@param [inout] reqData: ...
@param [out] err
*
@return: can we serve this request?
*******************************************************************************/
bool CuteHttpDispatcher::readCfgEntryLine(std::istream& is, string*& out)
{
    char buff[181];
    
    // read line(s)
    while(is)
    {
        is.getline(buff, 181);
        if(strlen(buff) == 180)
        {
            TRACE_ERR("Crap in the config file: line >= 180 !!!");
            continue;
        }

        if(buff[0] == '#')
            continue; // comment

        if(::strspn(buff, " ") == ::strlen(buff))
            continue; // blank line

        out = new string(buff); 
        return true;
    }

    return false; // eof
}


/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
