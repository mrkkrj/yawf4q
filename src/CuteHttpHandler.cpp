/*******************************************************************************/
/**
@file       CuteHttpHandler.cpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3  
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains implementation of the CuteHttpHandler class
*
@history
* 2010/28/10  mrkkrj  
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

/* INCLUDE FILES **************************************************************/

#include "CuteHttpHandler.hpp" 
#include "CuteHttpServer.hpp" // fro CuteSrvRequest
#include "CuteMoustache.hpp"  // for templating support
#include "CuteTrace.hpp"

#include <QString>

//#include <algorithm>
#include <cassert>
#include <fstream>

using  namespace ibkrj::yawf4q;
using namespace std;


/* LOCAL FUNCTIONS ************************************************************/

/* STATIC INITIALIZATION ******************************************************/

/* PUBLIC *********************************************************************/

/* METHOD *********************************************************************/
/**
  Constructor of the class. 
*
@param: none
*******************************************************************************/
CuteHttpHandler::CuteHttpHandler(const string& name)
    : m_name(name)
{
    // register at the dispatcher!
    CuteHttpDispatcher::registerHandler(name, this);
}


/* METHOD *********************************************************************/
/**
  Destructor of the class. 
*
*******************************************************************************/
CuteHttpHandler::~CuteHttpHandler()
{
    // empty
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param subdispatch: ...
@param [inout] reqData: ...
@param [out] respData: ...
*
@return: none
*******************************************************************************/
void CuteHttpHandler::process(const string& subdispatch, CuteSrvRequest& reqData, 
                              string& respData)
{
    // if invoked: configured but not implemented in the subclass!
    reqData.httpErr = 500;
    respData = "Resource configured but no handler provided!";
}


/* PROTECTED ******************************************************************/

/* METHOD *********************************************************************/
/**
  This method ...
*
@param data: ...
@param templName: ...
*
@return: ...
*******************************************************************************/
string CuteHttpHandler::render(const CuteMstchValMap& data, const string& templName)
{
    // OPEN TODO:: cache read/parsed templates!!!

    // read from file
    ifstream ifs(templName.c_str());
  
    string templString((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

    if(templString.empty())
        throw "Error reading template data from disc!!!";

    // process:
    return renderThis(data, templString);
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param data: ...
@param templString: ...
*
@return: ...
*******************************************************************************/
string CuteHttpHandler::renderThis(const CuteMstchValMap& data, const string& templString)
{
    // OPEN TODO:: cache parsed templates!!!

    CuteMoustache ms;
    return ms.process(templString, data); 
}


/* PRIVATE ********************************************************************/


/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
