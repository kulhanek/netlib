#ifndef ServerWatcherH
#define ServerWatcherH
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

#include <NetLibMainHeader.hpp>
#include <SmartThread.hpp>
#include <PrmFile.hpp>
#include <SmallTime.hpp>

//------------------------------------------------------------------------------

class CRegClientList;

//------------------------------------------------------------------------------

//! Server watcher
/*! \ingroup eserver

  [watcher]
  enabled           (on/off) - determine is the watcher is started or not
  deadinterval      (int)    - time in seconds for unregistering dead clients
  errorinterval     (int)    - time in seconds to flush CErrorSystem log into logfile
  logname           (string) - file name of logfile

*/
class  NETLIB_PACKAGE CServerWatcher : public CSmartThread {
public:
// constructor -----------------------------------------------------------------
    CServerWatcher(CRegClientList* p_list);

    //! read watcher common setup
    bool ProcessWatcherControl(CPrmFile& confile);

// section of private data -----------------------------------------------------
private:
    bool            Enabled;
    int             FlushDeadClientsInterval;   // in seconds
    CRegClientList* ClientList;

    int             FlushErrorStackInterval;    // in seconds
    CSmallTime      LastFlushErrorStackTime;
    CSmallString    ErrorLogFileName;

    // main loop
    virtual void __hipolyspec ExecuteThread(void);
};

//------------------------------------------------------------------------------

#endif
