#ifndef RegClientH
#define RegClientH
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
#include <SmallTimeAndDate.hpp>
#include <iostream>

//------------------------------------------------------------------------------

enum ERegClientStatus {
    ERCS_NEW,           // new item in the list
    ERCS_REGISTERED,    // client is registered
    ERCS_UNREGISTERED,  // client was unregistered
    ERCS_DEAD,          // client is dead
    ERCS_TERMINATED     // client is terminated upon request
};

//------------------------------------------------------------------------------

class CXMLElement;
class CServerCommand;

//------------------------------------------------------------------------------

//! Registered client class
/*! \ingroup eserver
*/

class NETLIB_PACKAGE CRegClient {
public:
    CRegClient(void);
    CRegClient(const CSmallString& client_name,const CSmallString& job_id);
    virtual ~CRegClient(void);

    //! return client ID
    int GetClientID(void);

    //! return client status
    ERegClientStatus GetClientStatus(void);

    //! print info about client
    void PrintInfo(std::ostream& vout);

    //! load client info
    void LoadInfo(CXMLElement* p_ele);

    //! save client info
    void SaveInfo(CXMLElement* p_ele);

    //! register operation
    void RegisterOperation(void);

    //! set command related to the client
    void SetCommand(CServerCommand* p_cmd);

    //! get command related to the client
    CServerCommand* GetCommand(void);

    //! set extra status
    void SetExtraStatus(const CSmallString& es);

    //! get extra status
    const CSmallString& GetExtraStatus(void);

// section of private data ----------------------------------------------------
private:
    int                 ClientID;               // client ID
    ERegClientStatus    ClientStatus;           // current status of client
    CSmallTimeAndDate   RegistrationTime;       // registration time
    CSmallTimeAndDate   UnregistrationTime;     // unregistration time (dead/terminated)
    CSmallTimeAndDate   LastOpTime;             // time of last operation
    int                 NumberOfTransaction;    // number of transactions
    CSmallString        ClientName;             // client DNS name
    CSmallString        InfJobID;               // client Infinity job ID
    CServerCommand*     Command;                // command related to the client
    CSmallString        ExtraStatus;            // extra status of client

    friend class CRegClientList;
};

//------------------------------------------------------------------------------

#endif
