/*******************************************************************************/
/**
@file       CuteHttpHandler.hpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3 
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains the definition of the CuteHttpHandler class.
@see
*
@history
* 2010/28/10  mrkkrj   
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

#ifndef CUTE_HTTP_HANDLER_H
#define CUTE_HTTP_HANDLER_H

/* INCLUDE FILES **************************************************************/

#include <string>

namespace ibkrj 
{ 
namespace yawf4q
{

/* FORWARD DECLARATIONS *******************************************************/

struct CuteSrvRequest;
class CuteMstchValMap;


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
class CuteHttpHandler
{
public:
    CuteHttpHandler(const std::string& name);
    ~CuteHttpHandler();

    const std::string& name() const { return m_name; }

    virtual void process(CuteSrvRequest& reqData, std::string& respData) = 0;
    virtual void process(const std::string& subdispatch, 
                         CuteSrvRequest& reqData, std::string& respData);
    virtual CuteHttpHandler* clone() = 0;

protected:
    std::string render(const CuteMstchValMap& data, const std::string& templName);
    std::string renderThis(const CuteMstchValMap& data, const std::string& templString);

private:
    std::string m_name;
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
