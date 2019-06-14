#ifndef ActionRequestH
#define ActionRequestH
// =============================================================================
// NETLib - Core Services for Client/server Communication over Network
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor,
//    Boston, MA  02110-1301  USA
// =============================================================================

#include <NETLibMainHeader.hpp>
#include <SmallString.hpp>
#include <vector>

//------------------------------------------------------------------------------

//! Client action request
/*! \ingroup client
    The fully qualified server name has folowing syntax:
    protocol://server.name.cz[:port][/action[?key1=value1,key2=value2,...]]
*/

class NETLIB_PACKAGE CActionRequest {
public:
    CActionRequest(void);

// get methods ----------------------------------------------------------------
    //! return qualified network name of server
    const CSmallString GetQualifiedName(void) const;

    //! return network name of server
    const CSmallString& GetName(void) const;

    //! return server IP
    const CSmallString& GetIP(void) const;

    //! return port number
    int                 GetPort(void) const;

    //! return action - action DO NOT start with /
    const CSmallString& GetAction(void) const;

    //! return number of additional parameters
    int GetNumberOfParameters(void) const;

    //! get value for parameter key
    bool GetParameterKeyValue(const CSmallString& key,CSmallString& value);

    //! get value for parameter key
    bool GetParameterKeyValue(const CSmallString& key,int& value);

    //! return used password
    const CSmallString& GetPassword(void) const;

    //! return protocol name
    const CSmallString& GetProtocolName(void) const;

    //! is serverkey
    bool IsServerKey(void) const;

// set methods ----------------------------------------------------------------
    //! set protocol name
    void SetProtocolName(const CSmallString& protocol);

    //! set server description from qualified server name
    void SetQualifiedName(const CSmallString& name);

    //! set network name of server, IP address is set too
    void SetNameOrIP(const CSmallString& name);

    //! set port number
    void SetPort(int port);

    //! set used password
    void SetPassword(const CSmallString& password);

    //! read server key
    void ReadServerKey(const CSmallString& name);

// section of private data ----------------------------------------------------
private:
    class NETLIB_PACKAGE CActionParameter {
    public:
        CSmallString        Key;
        CSmallString        Value;
    };

    CSmallString                    Protocol;      //!< protocol name
    CSmallString                    Name;          //!< name of server
    CSmallString                    IP;            //!< IP of server
    int                             Port;          //!< port number
    CSmallString                    Action;        //!< action string
    CSmallString                    Password;      //!< password
    std::vector<CActionParameter>   Parameters;    //!< list of action parameters
    bool                            ServerKey;

    void GetServerNameAndIP(const CSmallString& addr);
};

//---------------------------------------------------------------------------

#endif
