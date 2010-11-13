/*******************************************************************************/
/**
@file       CuteMstchData.hpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3 
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains the definition of the CuteMoustache class.
@see
*
@history
* 2010/28/10  mrkkrj  
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

#ifndef CUTE_MOUSTACHE_DATA_H
#define CUTE_MOUSTACHE_DATA_H

/* INCLUDE FILES **************************************************************/

#include <string>
#include <map>
#include <list>
#include <vector>

namespace ibkrj 
{ 
namespace yawf4q
{

/* FORWARD DECLARATIONS *******************************************************/

/* CONSTANT DECLARATIONS ******************************************************/

/* CLASS DECLARATIONS *********************************************************/

/* CLASS DECLARATION **********************************************************/
/**
  A helper class for storing multivariant maps which may in turn contain lists 
  of multivariant maps, like:

    {
      "header": "Colors",
      "items": [
          {"name": "red", "first": true, "url": "#Red"},
          {"name": "green", "link": true, "url": "#Green"},
          {"name": "blue", "link": true, "url": "#Blue"}
      ],
      "empty": false
    }

  Wait, isn't that JSON data???
*
@note: didn't use boost::variant as I (1st) wanted to stay boost-free, and (2nd)
       VisualStudio 2005 cannot compile recursive variants! 
*
@author: ib-krajewski.de 
*******************************************************************************/
class CuteMstchValMap
{
public:
    CuteMstchValMap();
    ~CuteMstchValMap();

    void addString(const std::string& valname, const std::string& value);
    void addInt(const std::string& valname, int value);
    void addBool(const std::string& valname, bool value);

    void addToList(const std::string& listname, size_t listpos, 
                   const std::string& valname, const std::string& value);
    void addToList(const std::string& listname, size_t listpos, 
                   const std::string& valname, const char* value);
    void addToList(const std::string& listname, size_t listpos, 
                   const std::string& valname, int value);
    void addToList(const std::string& listname, size_t listpos, 
                   const std::string& valname, bool value);

private:  
    // visibility:
    friend class CuteMoustache;

    // data
    struct mvalue;
    typedef std::map<std::string, mvalue> mvalue_map;
    typedef std::list<mvalue_map> mvalue_list;

    enum Tag { none, boolean, integer, stringPtr, listPtr };
    struct mvalue
    {
        Tag t;
        union mvalueU
        {
            std::string* strVal; // don't free in destructor because of the STL map!
            mvalue_list* listVal; // ibidem
            int intVal;
            bool boolVal;
        } u;

        mvalue() : t(none) {}
        void clear(); 
    };

    // top level map:
    mvalue_map m_hash;

    // methods
    mvalue_map& getListNode(const std::string& listname, size_t listpos);
};


/* INLINE METHODS *************************************************************/

/* TEMPLATE METHODS ***********************************************************/

/* OPERATORS ******************************************************************/

/* TYPE DECLARATIONS **********************************************************/

}}
 
#endif /* XXX_H */

/******************************************************************************/
/* END OF FILE */
/******************************************************************************/
