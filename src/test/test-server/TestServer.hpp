#ifndef TestServerH
#define TestServerH
// =============================================================================
// NETLib - Core Services for Client/server Communication over Network
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <ExtraServer.hpp>
#include <PrmFile.hpp>
#include <RegClientList.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

#include "TestServerOptions.hpp"

//------------------------------------------------------------------------------

//! Test server main class
/*! \ingroup tserver
*/
class CTestServer : public CExtraServer {
public:
    // constructor
    CTestServer(void);

 // main methods ---------------------------------------------------------------
    //! init options
    int Init(int argc,char* argv[]);

    //! main part of program
    bool Run(void);

    //! finalize program
    void Finalize(void);

// section of public data ------------------------------------------------------
public:
    CTestServerOptions  Options;

// section of private data -----------------------------------------------------
private:
    CPrmFile            Controls;

    // output ------------------------------------
    CTerminalStr        Console;
    CVerboseStr         vout;

    //! Ctrl+C signal handler
    static void CtrlCSignalHandler(int signal);

    friend class CVSProcessor;
 };

//------------------------------------------------------------------------------

extern CTestServer TestServer;

//------------------------------------------------------------------------------

#endif
