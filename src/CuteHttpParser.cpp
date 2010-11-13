/*******************************************************************************/
/**
@file       CuteHttpParser.cpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3  
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains implementation of the CuteHttpParser class
*
@history
* 2010/28/10  mrkkrj  
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

/* INCLUDE FILES **************************************************************/

#include "CuteHttpServer.hpp"  // for base datatypes
#include "CuteHttpParser.hpp" 
#include "CuteTrace.hpp"

#include <QString>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QUrl>

#include <algorithm>
#include <cassert>

using  namespace ibkrj::yawf4q;
using namespace std;


/* LOCAL FUNCTIONS ************************************************************/

/* STATIC INITIALIZATION ******************************************************/

namespace
{
    // OPEN TDOD::: same as in HTTP server class ... DRY!
    const string c_httpLineEnd = "\r\n";
}


/* PUBLIC *********************************************************************/

/* METHOD *********************************************************************/
/**
  Constructor of the class. 
*
@param: none
*******************************************************************************/
CuteHttpParser::CuteHttpParser()
{
    // empty
}


/* METHOD *********************************************************************/
/**
  Destructor of the class. 
*
*******************************************************************************/
CuteHttpParser::~CuteHttpParser()
{
    // empty
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
bool CuteHttpParser::parseRequest(const std::string& requestStr, 
                                  CuteSrvRequest& reqData, std::string& errStr, 
                                  int& bytesMissing)
{
    // valid request (only header no content!)
    QHttpRequestHeader req(requestStr.c_str());

    if(!req.isValid())
    {
        errStr = "invalid HTTP request";
        reqData.httpErr = 400;
        return false;
    }

    // translate the type (annoying!):
    if(req.method() == "GET")
        reqData.type = HTTP_GET_REQUEST;
    else if(req.method() == "POST")
        reqData.type = HTTP_POST_REQUEST;
    else
        reqData.type = HHTP_NYI_REQUEST;

    // read the path:
    reqData.requestedURI = req.path().toLatin1().constData();

    // more data needed?
    bytesMissing = 0;

    if(reqData.reqContent != 0)
    {
        // content data already delivered!
        if(reqData.type == HTTP_POST_REQUEST)
        {
            if(!parseFormularData(reqData))
            {
                TRACE_ERR("internal error, cannot parase POST request!!!");
                reqData.httpErr = 500;
                return false;
            }
            else
                return true;           
        }
        else
            return true; // OR false, internal error????
    }

    if(reqData.type == HTTP_POST_REQUEST &&
       req.contentLength() != 0)       
    {      
        QString header = req.toString();
        unsigned headerSz = header.length();

        bytesMissing = req.contentLength() - (requestStr.size() - headerSz);

        if(TR_WEBIF) TRACE2("POST requests, bytes missing=", bytesMissing);
    }

    return true;
}


/* METHOD *********************************************************************/
/**
  This method will ...
*
@param: none
*
@return: ...
*******************************************************************************/
bool CuteHttpParser::parseFormularData(CuteSrvRequest& reqData)
{
    assert(reqData.reqContent != 0);

    // populate the map:

    string& contentData = *reqData.reqContent;    
    replace(contentData.begin(), contentData.end(), '+', ' ');   

    // tokenizer needed! take Qt's
    QString qs(contentData.c_str());
    QStringList tokens = qs.split("&");

    for(int i = 0; i < tokens.size(); i++)
    {        
        QStringList valpair = tokens[i].split("=");

        if(valpair.size() != 2)
            return false;
        
        // URL encoding: is ASCII + percents
        QString val1 = QUrl::fromPercentEncoding(valpair[0].toAscii().constData());
        QString val2 = QUrl::fromPercentEncoding(valpair[1].toAscii().constData());

        // OPEN TODO:: wide strings!!!
        reqData.formularData.insert(make_pair(val1.toLatin1().constData(),
                                              val2.toLatin1().constData()));
    }

    return true;
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
std::string CuteHttpParser::makeErrorResp(CuteSrvRequest& reqData, int errCode,
                                          const std::string& errStr)
{
    // format err string:
    // OPEN TODO:: error pages???
    string errPage("<html><body><h2>");
    errPage.append(errStr);
    errPage.append("</h2></body></html>");

    QHttpResponseHeader err;

    err.setStatusLine(errCode, "");
    err.addValue("Content-Type", "text/html");
    err.setContentLength(errPage.size());    

    string resp = err.toString().toLatin1().constData();

    // add content
    resp.append(errPage);
    resp.append(c_httpLineEnd);

    return resp;
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
std::string CuteHttpParser::makeOkResp(CuteSrvRequest& reqData, const std::string& respStr)
{
    QHttpResponseHeader ok;

    ok.setStatusLine(200);
    ok.addValue("Content-Type", "text/html");
    ok.setContentLength(respStr.size());  

    string resp = ok.toString().toLatin1().constData();;

    // add content
    resp.append(respStr);
    resp.append(c_httpLineEnd);

    return resp;
}


/* METHOD *********************************************************************/
/**
  This method will check an incoming request for validitiy.
*
@param [inout] reqData: data from parsed XML client's request, may be completed 
                        with sessionID if needed
@param [out] error: error description for an invalid request
*
@return: can we serve this request?
*******************************************************************************/
bool CuteHttpParser::checkIfValidRequest(/*const*/CuteSrvRequest& reqData, string& error)
{
    return true;

}


/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
