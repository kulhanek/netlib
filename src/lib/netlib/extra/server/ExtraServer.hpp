#ifndef ExtraServerH
#define ExtraServerH
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

#include <Server.hpp>
#include <RegClientList.hpp>
#include <PrmFile.hpp>
#include <ServerWatcher.hpp>
#include <iostream>

//------------------------------------------------------------------------------

//! Extra server base class
/*! \ingroup eserver

  CPrmFile section read and process by ProcessServerControl.

  [server]
  serverkey       (string)  - name of file containing serverkey
  port            (int)     - port number 1024,65535
  password        (string)  - password
  donotshutdown   (on/off)  - do not shutdown when all clients are unregistered
  maxqueuelen     (int)     - maximal length of queue holding incomming requests
  maxthreads      (int)     - number of thread workers in server thread pool

*/
class CExtraServer : public CServer {
public:
// constructor -----------------------------------------------------------------
    CExtraServer(void);

    //! read server common setup
    bool ProcessServerControl(CPrmFile& confile,std::ostream& vout=std::cout);

    //! start server
    bool StartServer(std::ostream& vout = std::cout);

    //! get server key name
    const CSmallString& GetServerKeyName(void) const;

    //! is server key ready
    bool IsServerKeyReady(void);

// section of public data ------------------------------------------------------
public:
    CRegClientList          RegClients;         // registered clients
    bool                    DoNotShutdown;

// section of private data -----------------------------------------------------
private:
    CSmallString            ServerKeyName;
    int                     Port;
    CServerWatcher          ServerWatcher;
    bool                    ServerKeyIsReady;
};

//------------------------------------------------------------------------------

#endif
