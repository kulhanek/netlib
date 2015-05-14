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

#include <stdio.h>
#include <signal.h>
#include <ErrorSystem.hpp>
#include <PrmUtils.hpp>
#include <SmallTime.hpp>
#include <CmdProcessorList.hpp>
#include <ExtraOperation.hpp>
#include <ExtraFactory.hpp>

#include "TestServer.hpp"
#include "TestFactory.hpp"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

CTestServer TestServer;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTestServer::CTestServer(void)
{
    SetProtocolName("test");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTestServer::Init(int argc,char* argv[])
{
// encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

// should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

// attach verbose stream to cout and set desired verbosity level
    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::debug);
    } else {
        vout.Verbosity(CVerboseStr::high);
    }

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# test-server started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if(Options.GetArgControlFile() != "-") {
        vout << "# Control file name : " << Options.GetArgControlFile() << endl;
    } else {
        vout << "# Control file name : - (standard input)" << endl;
    }

// set SIGINT hadler to cleanly shutdown server ----------
    signal(SIGINT,CtrlCSignalHandler);

// process control file ----------------------------------
    if(Options.GetArgControlFile() != "-") {
        if(Controls.Read(Options.GetArgControlFile()) == false) {
            ES_ERROR("unable to open and read control file");
            return(SO_USER_ERROR);
        }
    } else {
        if(Controls.Parse(stdin) == false) {
            ES_ERROR("unable to read control file from standard input");
            return(SO_USER_ERROR);
        }
    }

    if( ProcessServerControl(Controls,vout) == false ){
       return( SO_USER_ERROR );
    }

    if( Controls.CountULines() > 0 ){
       ES_ERROR("unprocessed items found in control file");
       Controls.Dump(stderr,true);
       return( SO_USER_ERROR );
    }

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTestServer::Run(void)
{
    vout << endl;
    vout << "::::::::::::::::::::::::::::::::::: Test Server ::::::::::::::::::::::::::::::::" << endl;

// register operations
    // clear default mapping from ExtraServer
    CmdProcessorList.ClearFactoryMapping();

    // register new operation
    CmdProcessorList.RegisterProcessor(Operation_GetServerInfo,&TestFactory);

// start server listening
    if( StartServer(vout) == false ){
        return(false);
    }

// print server statistics
    vout << endl;
    vout << "=== Short server summary =======================================================" << endl;
    vout << endl;

    vout << "Number of processed transactions: " << GetNumberOfTransactions() << endl;
    vout << "Number of illegal transactions  : " << GetNumberOfIllegalTransactions() << endl;

    return(true);
}

//------------------------------------------------------------------------------

void CTestServer::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# test-server terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        ErrorSystem.PrintErrors(vout);
    }

    vout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTestServer::CtrlCSignalHandler(int signal)
{
    TestServer.vout << endl;
    TestServer.vout << "Ctrl+C signal recieved. ";
    TestServer.vout << "Initiating server shutdown ... " << endl;
    TestServer.vout << "Waiting for server finalization ... " << endl;
    TestServer.TerminateServer();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
