/*******************************************************************************/
/**
@file       CuteMoustache.hpp
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

#ifndef CUTE_MOUSTACHE_H
#define CUTE_MOUSTACHE_H

/* INCLUDE FILES **************************************************************/

#include "CuteMstchData.hpp"

#include <string>
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
  This class is a trivial and partial, quick and dirty impl. of the mustache 
  templating language for the YAWF's CuteHttpServer only!!!

  Not performance optimized, not bug free!!
*
@see: http://mustache.github.com/
*
@author: ib-krajewski.de 
*******************************************************************************/
class CuteMoustache
{
public:
    //CuteMoustache();
    //~CuteMoustache();
    std::string process(const std::string& mtemplate, const CuteMstchValMap& mdata);

private: 
    bool parseTempl(const std::string& mtemplate, std::string&parseErr);

    // data members
    class MstchNode
    {
    public: 
        virtual ~MstchNode() {}
        virtual bool isReady() = 0;
        virtual void setVal(const CuteMstchValMap::mvalue& val) = 0;
        virtual void reset() = 0;
        virtual void exportTo(std::string& out) = 0;
        virtual const std::string& name() { return ""; }
        virtual bool skip() { return false; }
    };

    // static text value
    class MstchValue : public MstchNode
    {
    public: 
        virtual bool isReady() { return true; }
        virtual void setVal(const CuteMstchValMap::mvalue& val);
        virtual void reset() { m_text.clear(); }
        virtual void exportTo(std::string& out);
    private:
        std::string m_text;
    };

    // template value to be filled in
    class MstchVariable : public MstchNode
    {
    public: 
        MstchVariable(std::string& name);        
        virtual bool isReady() { return false; }
        virtual void setVal(const CuteMstchValMap::mvalue& val);
        virtual void reset() { m_valueStrg.clear(); }
        virtual void exportTo(std::string& out);
        virtual const std::string& name() { return m_name; }

    protected:
        std::string m_valueStrg;

    private:
        std::string m_name;
    };

    // sections to be enabled/disabled/repated
    class MstchSection : public MstchVariable
    {
    public: 
        MstchSection(std::string& name) : MstchVariable(name) {};
        virtual void setVal(const CuteMstchValMap::mvalue& val);
        virtual bool skip();
        virtual void exportTo(std::string& out) {};
    };

    // the node's structure is kept FLAT!
    std::vector<MstchNode*> m_mstchTemplRepr;    

    // operations on the node list:
    typedef std::vector<MstchNode*> node_lst;

    std::string renderTemplate(node_lst& templ, const CuteMstchValMap::mvalue_map& valMap);
    std::string renderList(node_lst& templNodes, node_lst::iterator sectionStart, 
                           const CuteMstchValMap::mvalue& listVal);
    node_lst::iterator findSectionEnd(node_lst& templNodes, node_lst::iterator& sectionStart);
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
