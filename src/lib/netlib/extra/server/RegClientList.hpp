#ifndef RegClientListH
#define RegClientListH
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
#include <SimpleList.hpp>
#include <RegClient.hpp>
#include <IndexCounter.hpp>
#include <SimpleMutex.hpp>
#include <iostream>

//------------------------------------------------------------------------------

class CXMLElement;

//------------------------------------------------------------------------------

//! Registered client list
/*! \ingroup eserver
*/

class NETLIB_PACKAGE CRegClientList : protected CSimpleList<CRegClient> {
public:
    CRegClientList(void);

    //! add new client to the list
    void AddClient(CRegClient* p_rcl);

    //! register new client to the list
    void RegisterClient(CRegClient* p_rcl);

    //! register new client
    CRegClient* RegisterClient(void);

    //! unregister client
    void UnregisterClient(int id);

    //! find client
    CRegClient* FindClient(int id);

    //! return number of clients
    int GetNumberOfClients(void);

    //! return number of registered clients
    int GetNumberOfRegClients(void);

    //! return client of index
    CRegClient* GetClient(int index);

    //! save info about clients
    void SaveInfo(CXMLElement* p_iele);

    //! return number of active registration
    int GetNumberOfActiveRegistration(void);

    //! flush dead clients
    void FlushDeadClients(int limit);

    //! print info about registered clients
    void PrintInfo(void);

    //! print info about registered clients
    void PrintInfo(std::ostream& sout);

// section of private data ----------------------------------------------------
private:
    CIndexCounter   IndexPool;      // index pool for registeed clients

protected:
    CSimpleMutex    AccessMutex;    // mutex for list accesses
};

//------------------------------------------------------------------------------

#endif
