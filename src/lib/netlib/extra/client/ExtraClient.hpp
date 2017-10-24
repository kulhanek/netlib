#ifndef ExtraClientH
#define ExtraClientH
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
#include <Client.hpp>
#include <Password.hpp>
#include <XMLElement.hpp>
#include <ostream>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

//! Extra client base class
/*! \ingroup eclient
*/

class NETLIB_PACKAGE CExtraClient : public CClient {
public:
    CExtraClient(void);

// section of private data ----------------------------------------------------
public:

// helper methods operations --------------------------------------------------
    //! read password from keybord or file and update client CActionRequest
    bool ReadPassword(bool read_pswfile,
                      const CSmallString& pswfile,bool verbose);

// supported operations -------------------------------------------------------
    //! register client
    bool RegisterClient(int& client_id);

    //! unregister client
    bool UnregisterClient(int client_id);

    //! get status information from server
    bool GetServerInfo(std::ostream& vout);

    //! print server short info on already executed command
    void GetShortServerInfo(CClientCommand* p_command,std::ostream& sout);

    //! print long server info on already executed command
    void GetLongServerInfo(CClientCommand* p_command,std::ostream& sout);

    //! shutdown server
    bool ShutdownServer(std::ostream& vout,bool nopass=false);

    //! return server error stack
    bool GetServerErrors(std::ostream& vout);
};

//------------------------------------------------------------------------------

#endif
