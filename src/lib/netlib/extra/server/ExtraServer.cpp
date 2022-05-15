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

#include <ExtraServer.hpp>
#include <ExtraFactory.hpp>
#include <CmdProcessorList.hpp>
#include <ExtraOperation.hpp>
#include <ErrorSystem.hpp>
#include <FileSystem.hpp>
#include <iomanip>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtraServer::CExtraServer(void)
    : RegClients(), ServerWatcher(&RegClients)
{
    Port = INVALID_PORT;
    DoNotShutdown = false;
    TargetRegs = -1;
    ServerKeyIsReady = false;

    // register operations
    CmdProcessorList.RegisterProcessor(Operation_GetServerInfo,&ExtraFactory);
    CmdProcessorList.RegisterProcessor(Operation_ShutdownServer,&ExtraFactory);
    CmdProcessorList.RegisterProcessor(Operation_GetServerErrors,&ExtraFactory);
    CmdProcessorList.RegisterProcessor(Operation_RegisterClient,&ExtraFactory);
    CmdProcessorList.RegisterProcessor(Operation_UnregisterClient,&ExtraFactory);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CExtraServer::ProcessServerControl(CPrmFile& confile,ostream& vout)
{
    vout << "\n=== [server] ===================================================================" << endl;
    if( confile.OpenSection("server") == false ) {
        ES_ERROR("unable to open [server] section");
        return(false);
    }

    if( confile.GetStringByKey("serverkey",ServerKeyName) == true  ) {
        vout << "Server key file name  (serverkey)              = " << ServerKeyName << endl;
        // remove old server key
        CFileSystem::RemoveFile(ServerKeyName,true);
        ServerKeyIsReady = false;
    } else {

        if( confile.GetIntegerByKey("port",Port) == true ) {
            vout << "Listening port (port)                          = " << setw(6) << Port << endl;
            if( Port == 0 ) {
                ES_ERROR("port cannot be zero");
                return(false);
            }
            if( (Port < 1025) || (Port > 65535) ) {
                ES_ERROR("port is not in legal range <1025;65535>");
                return(false);
            }
        } else {
            ES_ERROR("port has to be specified if serverkey is not specified");
            return(false);
        }

        CSmallString   passwd;

        if( confile.GetStringByKey("password",passwd) == true ) {
            vout << "Password (password)                            = " << passwd << endl;
            SetPassword(passwd);
        } else {
            ES_ERROR("password has to be specified if serverkey is not specified");
            return(false);
        }

    }

    if( confile.GetLogicalByKey("donotshutdown",DoNotShutdown) == true ) {
        vout << "Do not automatically shutdown (donotshutdown)  = " << setw(6) << bool_to_str(DoNotShutdown) << endl;
    } else {
        vout << "Do not automatically shutdown (donotshutdown)  = " << setw(6) << bool_to_str(DoNotShutdown) << "                (default)" << endl;
    }

    if( confile.GetIntegerByKey("targetregs",TargetRegs) == true ) {
        vout << "Target num of finished registr. (targetregs)   = " << setw(6) << TargetRegs << endl;
    } else {
        vout << "Target num of finished registr. (targetregs)   = " << setw(6) << TargetRegs << "                (default)" << endl;
    }

    int queue_len = GetMaxListenQueueLen();
    if( confile.GetIntegerByKey("maxqueuelen",queue_len) == true ) {
        vout << "Listening queue length (maxqueuelen)           = " << setw(6) << queue_len << endl;
    } else {
        vout << "Listening queue length (maxqueuelen)           = " << setw(6) << queue_len << "                (default)" << endl;
    }
    if( SetMaxListenQueueLen(queue_len) == false ) {
        return(false);
    }

    int maxthreads = GetMaxNumOfWorkers();
    if( confile.GetIntegerByKey("maxthreads",maxthreads) == true ) {
        vout << "Max number of worker threads (maxthreads)      = " << setw(6) << maxthreads << endl;
    } else {
        vout << "Max number of worker threads (maxthreads)      = " << setw(6) << maxthreads << "                (default)" << endl;
    }
    if( SetMaxNumOfWorkers(maxthreads) == false ) {
        return(false);
    }

    // optional setup
    ServerWatcher.ProcessWatcherControl(confile);

    return(true);
}

//------------------------------------------------------------------------------

const CSmallString& CExtraServer::GetServerKeyName(void) const
{
    return(ServerKeyName);
}

//------------------------------------------------------------------------------

bool CExtraServer::IsServerKeyReady(void)
{
    return(ServerKeyIsReady);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CExtraServer::StartServer(std::ostream& vout)
{
    // start server listening
    vout << endl;
    vout << "Starting server ... " << endl;

    if( ServerKeyName != NULL ) {
        // start server
        if( InitServer(true) == false ) {
            ES_ERROR("unable to init server");
            return(false);
        }
        // write server key
        if( WriteServerKey(ServerKeyName) == false ) {
            ES_ERROR("unable to write server key");
            return(false);
        }
        ServerKeyIsReady = true;
    } else {
        if( InitServer(Port) == false ) {
            ES_ERROR("unable to init server");
            return(false);
        }
    }

    // start watcher
    ServerWatcher.StartThread();

    // wait until server is terminated
    vout << "Waiting for server termination ... " << endl;
    AcceptServerRequests();

    // stop watcher
    ServerWatcher.TerminateThread();
    ServerWatcher.WaitForThread();

    vout << endl;
    vout << "Server was terminated." << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
