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

#include <ServerWatcher.hpp>
#include <ExtraFactory.hpp>
#include <CmdProcessorList.hpp>
#include <ExtraOperation.hpp>
#include <ErrorSystem.hpp>
#include <RegClientList.hpp>
#include <unistd.h>
#include <errno.h>
#include <SmallTimeAndDate.hpp>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CServerWatcher::CServerWatcher(CRegClientList* p_list)
{
    Enabled = true;
    FlushDeadClientsInterval = 0;
    FlushErrorStackInterval  = 30;
    ErrorLogFileName         = "errors.log";
    ClientList               = p_list;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CServerWatcher::ProcessWatcherControl(CPrmFile& confile)
{
    printf("\n=== [watcher] ==================================================================\n");
    if( confile.OpenSection("watcher") == false ) {
        // this is optional section
        printf("Watcher service is enabled (enabled)           = %6s                (default)\n",bool_to_str(Enabled));
        printf("Flush dead clients interval (deadinterval) [s] = %6d                (default)\n",FlushDeadClientsInterval);
        printf("Flush error stack interval (errorinterval) [s] = %6d                (default)\n",FlushErrorStackInterval);
        printf("Error log file (logname)                       = %-15s       (default)\n",(const char*)ErrorLogFileName);
        return(true);
    }

    if( confile.GetLogicalByKey("enabled",Enabled) == true ) {
        printf("Watcher service is enabled (enabled)           = %6s\n",bool_to_str(Enabled));
    } else {
        printf("Watcher service is enabled (enabled)           = %6s                (default)\n",bool_to_str(Enabled));
    }

    if( confile.GetIntegerByKey("deadinterval",FlushDeadClientsInterval) == true ) {
        printf("Flush dead clients interval (deadinterval) [s] = %6d\n",FlushDeadClientsInterval);
    } else {
        printf("Flush dead clients interval (deadinterval) [s] = %6d                (default)\n",FlushDeadClientsInterval);
    }

    if( confile.GetIntegerByKey("errorinterval",FlushErrorStackInterval) == true ) {
        printf("Flush error stack interval (errorinterval) [s] = %6d\n",FlushErrorStackInterval);
    } else {
        printf("Flush error stack interval (errorinterval) [s] = %6d                (default)\n",FlushErrorStackInterval);
    }

    if( confile.GetIntegerByKey("logname",FlushErrorStackInterval) == true ) {
        printf("Error log file (logname)                       = %s\n",(const char*)ErrorLogFileName);
    } else {
        printf("Error log file (logname)                       = %-15s       (default)\n",(const char*)ErrorLogFileName);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CServerWatcher::ExecuteThread(void)
{
    if( FlushErrorStackInterval > 0 ) {
        FILE* p_fout = fopen(ErrorLogFileName,"w");
        if( p_fout == NULL ) {
            CSmallString error;
            error << "unable to open log file '" << ErrorLogFileName << "' (" << strerror(errno) << ")";
            ES_ERROR(error);
            return;
        }
        CSmallTimeAndDate current;
        current.GetActualTimeAndDate();

        fprintf(p_fout,"# === ERROR Stack ==============================================================\n");
        fprintf(p_fout,"# Server started at:    %s\n",(const char*)current.GetSDateAndTime());
        fflush(p_fout);
        fclose(p_fout);
    }

    LastFlushErrorStackTime.GetActualTime();

    while( ! ThreadTerminated ) {

        if( ClientList && (FlushDeadClientsInterval > 0) ) ClientList->FlushDeadClients(FlushDeadClientsInterval);

        CSmallTime currtime;
        currtime.GetActualTime();
        if( (currtime - LastFlushErrorStackTime > FlushErrorStackInterval) &&
                (FlushErrorStackInterval > 0) ) {
            LastFlushErrorStackTime = currtime;
            // append to error log and clear error stack
            ErrorSystem.AppendToErrLog(ErrorLogFileName,true);
        }

        usleep(500000); // sleep for 500 ms
    }

    if( FlushErrorStackInterval > 0 ) {
        FILE* p_fout = fopen(ErrorLogFileName,"a");
        if( p_fout == NULL ) {
            CSmallString error;
            error << "unable to open log file '" << ErrorLogFileName << "' (" << strerror(errno) << ")";
            ES_ERROR(error);
            return;
        }

        ErrorSystem.AppendToErrLog(ErrorLogFileName,true);

        CSmallTimeAndDate current;
        current.GetActualTimeAndDate();

        fprintf(p_fout,"# Server terminated at: %s\n",(const char*)current.GetSDateAndTime());
        fflush(p_fout);
        fclose(p_fout);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
