/*******************************************************************************/
/**
@file       CuteMoustache.cpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3  
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains implementation of the CuteMstchValMap class
*
@history
* 2010/28/10  mrkkrj 
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

/* INCLUDE FILES **************************************************************/

#include "CuteMoustache.hpp" 
#include "CuteTrace.hpp"

#include <QString>

#include <algorithm>
#include <cassert>

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
CuteMstchValMap::CuteMstchValMap()     
{
    // empty
}


/* METHOD *********************************************************************/
/**
  Destructor of the class. 
*
*******************************************************************************/
CuteMstchValMap::~CuteMstchValMap()     
{
    // delete all string/sublist pointers!
    mvalue_map::iterator iter;

    for(iter = m_hash.begin(); iter != m_hash.end(); iter++)
    {
        iter->second.clear();
    }
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addString(const std::string& valname, const std::string& value)
{
    mvalue mval;
    mval.u.strVal = new string(value); // OPEN TODO:: quick hack!
    mval.t = stringPtr;
    m_hash.insert(m_hash.end(), make_pair(valname, mval));
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addInt(const std::string& valname, int value)
{
    mvalue mval;
    mval.u.intVal = value;
    mval.t = integer;
    m_hash.insert(m_hash.end(), make_pair(valname, mval));
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addBool(const std::string& valname, bool value)
{
    mvalue mval;
    mval.u.boolVal = value;
    mval.t = boolean;
    m_hash.insert(m_hash.end(), make_pair(valname, mval));
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addToList(const std::string& listname, size_t listpos, 
                                const std::string& valname, const std::string& value)
{
    // find the list
    mvalue_map& lnode = getListNode(listname, listpos);

    mvalue mval;
    mval.u.strVal = new string(value); // OPEN TODO:: quick hack!
    mval.t = stringPtr;

    lnode.insert(lnode.end(), make_pair(valname, mval));
}


/* METHOD *********************************************************************/
/**
  This method ... needed as the compiler ratehr casts to bools as to promote to string!
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addToList(const std::string& listname, size_t listpos, 
                                const std::string& valname, const char* value)
{
    addToList(listname, listpos, valname, string(value));
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addToList(const std::string& listname, size_t listpos, 
                                const std::string& valname, int value)
{
    // find the list
    mvalue_map& lnode = getListNode(listname, listpos);

    mvalue mval;
    mval.u.intVal = value;
    mval.t = integer;

    lnode.insert(lnode.end(), make_pair(valname, mval));
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param valname: ...
@param value: ...
*
@return: none
*******************************************************************************/
void CuteMstchValMap::addToList(const std::string& listname, size_t listpos, 
                                const std::string& valname, bool value)
{
    // find the list
    mvalue_map& lnode = getListNode(listname, listpos);

    mvalue mval;
    mval.u.boolVal = value;
    mval.t = boolean;

    lnode.insert(lnode.end(), make_pair(valname, mval));
}


/* PROTECTED ******************************************************************/

/* PRIVATE ********************************************************************/

/* METHOD *********************************************************************/
/**
  Internal impl.

  A list is a named mval with this example struct

      "items": [                                            <- listname
          {"name": "red", "first": true, "url": "#Red"},    <- listnodes
          {"name": "green", "link": true, "url": "#Green"}, <- ... 
          {"name": "blue", "link": true, "url": "#Blue"}    <- ... 
      ]
*
*******************************************************************************/
CuteMstchValMap::mvalue_map& 
    CuteMstchValMap::getListNode(const std::string& listname, size_t listpos)
{
    // lok for list name
    mvalue_map::iterator iter = m_hash.find(listname);
    mvalue_list* lst;

    if(iter == m_hash.end())
    {
        mvalue mval;
        mval.u.listVal = new mvalue_list;
        mval.t = listPtr;

        lst = mval.u.listVal;
        m_hash.insert(m_hash.end(), make_pair(listname, mval));        
    }
    else
    {
        assert(iter->second.t == listPtr);
        lst = iter->second.u.listVal;
    }

    // look for a list node
    if(lst->size() > listpos)
    {
        mvalue_list::iterator iter = lst->begin();
        advance(iter, listpos);
        return *iter;
    }
    else
    {
        lst->push_back(mvalue_map());
        return *(--lst->end());
    }
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
void CuteMstchValMap::mvalue::clear()
{ 
    switch(t)
    {
    case stringPtr: 
    {
        delete u.strVal; 
        u.strVal = 0;
        break;
    }
    case listPtr: 
    {
        mvalue_list::iterator iter;

        for(iter = u.listVal->begin(); iter != u.listVal->end(); iter++)
        {
            iter->clear(); // OPEN TODO:: use for_each
        }

        delete u.listVal;
        u.listVal = 0;
        break;
    }

    default:
        break;
    }
}


/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
