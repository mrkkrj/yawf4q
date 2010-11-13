/*******************************************************************************/
/**
@file       CuteHttpParser.hpp
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

#ifndef CUTE_HTTP_PARSER_H
#define CUTE_HTTP_PARSER_H

/* INCLUDE FILES **************************************************************/

#include <string>

namespace ibkrj 
{ 
namespace yawf4q
{

/* FORWARD DECLARATIONS *******************************************************/

struct CuteSrvRequest;

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
class CuteHttpParser
{
public:
    CuteHttpParser();
    ~CuteHttpParser();

    bool parseRequest(const std::string& requestStr, CuteSrvRequest& reqData, 
                      std::string& errStr, int& bytesMissing);

    std::string makeErrorResp(CuteSrvRequest& reqData, int errCode,
                              const std::string& errStr);
    std::string makeOkResp(CuteSrvRequest& reqData, const std::string& respStr);

private:
    bool checkIfValidRequest(CuteSrvRequest& reqData, std::string& error);
    bool parseFormularData(CuteSrvRequest& reqData);
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
