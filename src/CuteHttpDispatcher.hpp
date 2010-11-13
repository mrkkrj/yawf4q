/*******************************************************************************/
/**
@file       CuteHttpDispatcher.hpp
@version    $Header$
@author     Author: mrkkrj (ib-krajewski.de)
@licence    released under the terms of LGPL v3 
*
@platforms  C++03, Visual Studio 2005, Windows XP
*
@description
    This file contains the definition of the CuteHttpDispatcher class.
@see
*
@history
* 2010/28/10  mrkkrj 
*       created, initial implementation
*
********************************************************************************
*******************************************************************************/

#ifndef CUTE_HTTP_DISPATCHER_H
#define CUTE_HTTP_DISPATCHER_H

/* INCLUDE FILES **************************************************************/

#include <string>
#include <list>
#include <map>
#include <iosfwd>
#include <QRegExp>


namespace ibkrj 
{ 
namespace yawf4q
{

/* FORWARD DECLARATIONS *******************************************************/

struct CuteSrvRequest;
class CuteHttpHandler;
class SessionData;


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
class CuteHttpDispatcher
{
public:
    CuteHttpDispatcher();
    ~CuteHttpDispatcher();

    bool handleGet(CuteSrvRequest& reqData, std::string& respData);
    bool handlePost(CuteSrvRequest& reqData, std::string& respData);

    static void registerHandler(const std::string& name, CuteHttpHandler* proto);

    // ??? not working?
    static CuteHttpDispatcher* initialized();


private:
    CuteHttpHandler* findHandler(CuteSrvRequest& reqData, std::string& respData) const;
    CuteHttpHandler* getHttpHandlerInstance(const std::string& handlerId) const;
    bool readCfgEntryLine(std::istream& is, std::string*& out);

    // data members:
    std::list<std::pair<QRegExp, std::string> > m_handlers;
    static std::map<std::string, CuteHttpHandler*>* m_prototypes;
    static std::map<std::string, SessionData*> m_sessions;
    static CuteHttpDispatcher* m_instance;


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
