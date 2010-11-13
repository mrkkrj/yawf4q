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
    This file contains implementation of the CuteMoustache class
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

#if 0
/* METHOD *********************************************************************/
/**
  Constructor of the class. 
*
@param: none
*******************************************************************************/
CuteMoustache::CuteMoustache()
{

}


/* METHOD *********************************************************************/
/**
  Destructor of the class. 
*
*******************************************************************************/
CuteMoustache::~CuteMoustache()
{
    // empty
}
#endif


/* METHOD *********************************************************************/
/**
  This method ...
*
@param mtemplate: ...
@param mdata ...
*
@return: ...
*******************************************************************************/
string CuteMoustache::process(const string& mtemplate, const CuteMstchValMap& mdata)
{
    // 1. parse the template:
    string parseErr;

    if(!parseTempl(mtemplate, parseErr))
    {
        TRACE_ERR2("templ_eng: badly formatted template, errDescr=", parseErr);
        parseErr.insert(0, "Template cannot be parsed!\n");
        return parseErr;
    }

    // 2. render template with data:    

    // look into CuteMstchValMap - we are friends!
    const CuteMstchValMap::mvalue_map& valMap = mdata.m_hash; 

    return renderTemplate(m_mstchTemplRepr, valMap);
}


/* PROTECTED ******************************************************************/

/* PRIVATE ********************************************************************/

/* METHOD *********************************************************************/
/**
  This method ...
*
@param mtemplate: ...
@param [out] parseErr ...
*
@return: ...
*******************************************************************************/
bool CuteMoustache::parseTempl(const string& mtemplate, string& parseErr)
{
    // OPEN TODO::: use some parser support like YACC, Sprite etc???
    size_t pos = 0, lastpos = 0;
    enum State { start, lpar, rpar, end } state;

    for(state = start; state != end;)
    {
        lastpos = pos;

        switch(state)
        {
        case start:
        case rpar:
        {
            pos = mtemplate.find("{{", lastpos);

            if(pos == string::npos)
            {
                // no more variables, save the whole tail
                pos = mtemplate.size();
            }
       
            if(lastpos != pos)
            {
                // save the static text
                MstchValue* node = new MstchValue;
                m_mstchTemplRepr.push_back(node);

                CuteMstchValMap::mvalue val;
                val.u.strVal = new string;
                val.u.strVal->assign(mtemplate.begin() + lastpos, mtemplate.begin() + pos);
                m_mstchTemplRepr.back()->setVal(val);
            }

            state = lpar;
            pos += 2; // skip {{
            break;
        }

        case lpar:            
            pos = mtemplate.find("}}", lastpos);

            if(pos == string::npos)
            {
                parseErr = "unbalanced parentheses (R)";
                return false;
            }
            else
            {
                string varName;
                varName.assign(mtemplate.begin() + lastpos, mtemplate.begin() + pos);

                MstchNode* node = 0;
                if(varName[0] == '#' || varName[0] == '/')
                {
                    varName.erase(0, 1);
                    node = new MstchSection(varName);
                }
                else
                    node = new MstchVariable(varName);

                m_mstchTemplRepr.push_back(node);

                state = rpar;
                pos += 2; // skip }}
                break;
            }

        default:
            TRACE_ERR("templ_eng: internal error, unexpected state!");
            parseErr = "internal error, unexpected state!";
            return false;
        }

        if(pos >= mtemplate.size())
        {
            state = end;
            break;
        }
    }

    // parsed!    

    // OPEN TODO:: check the section delimiters
#if 0
    if(templIter == m_mstchTemplRepr.end())
    {
        TRACE_ERR2("templ_eng: unpaired section delimiters, name=", secname);                        
        parseErr = "Template cannot be parsed, unpaired section delimiters!\n ";
        return parseErr;
    }
#endif 

    return true;
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param mtemplate: ...
@param mdata ...
*
@return: ...
*******************************************************************************/
string CuteMoustache::renderTemplate(node_lst& templNodes, 
                                     const CuteMstchValMap::mvalue_map& valMap)                                     
{
    map<string, CuteMstchValMap::mvalue>::const_iterator mvalIter;

    node_lst::iterator templIter;
    string ret;

    // -- all the nodes form a linear structure!

    for(templIter = templNodes.begin(); templIter != templNodes.end(); templIter++)
    {
        bool listValue = false;

        // 1. has to be evaluated first?
        if(!(*templIter)->isReady())
        {            
            mvalIter = valMap.find((*templIter)->name());

            if(mvalIter == valMap.end())
            {
                // data not there: ignore the variable (mustache homepage example!)
                TRACE2("templ_eng: cannot find value for a variable, name=", 
                       (*templIter)->name());

                //ret.append("val=XXX"); 
                (*templIter)->reset();
            }
            else
            {
                // list input?
                if(mvalIter->second.t == CuteMstchValMap::listPtr)
                {
                    // need to descend!
                    ret.append(renderList(templNodes, templIter, mvalIter->second));
                    listValue = true;
                }
                else
                {
                    // simple case:
                    (*templIter)->setVal(mvalIter->second);           
                }
            }
        }
        
        // 2. add to output        
        (*templIter)->exportTo(ret);


        // 3. skip a section?
        if((*templIter)->skip() || // -- no data
            listValue == true)     // -- already rendered
        {
            node_lst::iterator lastPos = templIter;
            templIter = findSectionEnd(templNodes, templIter); // forward!

            if(templIter == templNodes.end())
            {
                TRACE_ERR2("templ_eng: unpaired section delimiters, name=", (*lastPos)->name());                        
                string parseErr = "Template cannot be parsed, unpaired section delimiters!\n ";
                return parseErr;
            }  
        }
    }

    // OK!
    return ret;
}

/* METHOD *********************************************************************/
/**
  This method ...
*
@param mtemplate: ...
@param mdata ...
*
@return: ...
*******************************************************************************/
string CuteMoustache::renderList(node_lst& templNodes, node_lst::iterator sectionStart, 
                                 const CuteMstchValMap::mvalue& listValue)
{
    // always: evaluate the section!
    (*sectionStart)->setVal(listValue);

    // sanity checks:
    node_lst::iterator sectionEnd = findSectionEnd(templNodes, sectionStart);

    if(sectionEnd == templNodes.end())
        return "ERROR: unpaired section delimiters";

    if(distance(sectionStart, sectionEnd) == 1)
        return ""; // empty section!

    // construct the sub-tempate to be looped over: 
    node_lst section;

    node_lst::iterator start = sectionStart;
    start++;
    node_lst::iterator end = sectionEnd;
    //end--;

    section.assign(start, end);

    // loop over the input list    
    CuteMstchValMap::mvalue_list* listData = listValue.u.listVal;
    string ret;

    for(CuteMstchValMap::mvalue_list::iterator iter = listData->begin();
        iter != listData->end();
        iter++)
    {       
        // descend
        ret += renderTemplate(section, *iter);
        // ret.append("\n");  --- needed?
    }

    // ok    
    return ret;
}


/* METHOD *********************************************************************/
/**
  This method ...
*
@param mtemplate: ...
@param mdata ...
*
@return: ...
*******************************************************************************/
CuteMoustache::node_lst::iterator 
    CuteMoustache::findSectionEnd(node_lst& templNodes, node_lst::iterator& sectionStart)
{
    if(sectionStart == templNodes.end())
        return templNodes.end();

    node_lst::iterator templIter = sectionStart;
    const string& secname = (*templIter)->name();
    
    for(templIter++; templIter != templNodes.end(); templIter++)                
    {                    
        if((*templIter)->name() == secname)
            break;                
    }

    return templIter;
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
void CuteMoustache::MstchValue::setVal(const CuteMstchValMap::mvalue& val)
{
    // always string data
    assert(val.u.strVal != 0);
    m_text = *val.u.strVal;
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
void CuteMoustache::MstchValue::exportTo(std::string& out)
{
    out.append(m_text);
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
CuteMoustache::MstchVariable::MstchVariable(std::string& name)
    : m_name(name)
{
    // empty
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
void CuteMoustache::MstchVariable::setVal(const CuteMstchValMap::mvalue& val)
{
    // m_ready = true; --- as to be able to rebind!!!

    switch(val.t)
    {
    case CuteMstchValMap::stringPtr:
        assert(val.u.strVal != 0);
        m_valueStrg = *val.u.strVal;
        break;

    case CuteMstchValMap::integer:
        m_valueStrg = QString::number(val.u.intVal).toAscii().constData();
        break;

    case CuteMstchValMap::boolean:
        if(val.u.boolVal)
            m_valueStrg = "True";
        else
            m_valueStrg = "False";
        break;

    default:
        assert(false);
        m_valueStrg = "tag=XXX";
    }
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
void CuteMoustache::MstchVariable::exportTo(std::string& out)
{
    out.append(m_valueStrg);
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
bool CuteMoustache::MstchSection::skip() 
{
    string xxx;
    MstchVariable::exportTo(xxx);

    // quick HACK !!!
    if(xxx == "True")
        return false;
    else
    if(xxx == "False")
        return true;
    else
    {
        // data not there: ignore the variable (mustache homepage example!)
        TRACE2("templ_eng: section data not set, expected True/False, name=", name());
        return true; // skip!
    }
}


/* METHOD *********************************************************************/
/**
  Internal impl.
*
*******************************************************************************/
void CuteMoustache::MstchSection::setVal(const CuteMstchValMap::mvalue& val)
{
    if((val.t) != CuteMstchValMap::listPtr)
    {
        if(val.t != CuteMstchValMap::boolean)
        {
            TRACE_ERR2("templ_eng: bad data for a section, expected bool/list, name=",
                       name());
            m_valueStrg = "False";
            return;
        }

        MstchVariable::setVal(val);
        return;
    }

    // else: render a list!
    assert(val.u.listVal != 0);

    if(val.u.listVal->empty())
    {
        // no data, skip!
        m_valueStrg = "False";
        return;
    }
    
    // else: may loop through the list!
    m_valueStrg = "True";
}


/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
