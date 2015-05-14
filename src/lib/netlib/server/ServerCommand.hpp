#ifndef CommandH
#define CommandH
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
#include <CommandCore.hpp>
#include <Runnable.hpp>
#include <Network.hpp>

//------------------------------------------------------------------------------

class CServer;

//------------------------------------------------------------------------------

#define CMD_ERROR(cmd,error) \
        {\
            ErrorSystem.AddError(error,__func__,__FILE__,__LINE__); \
            cmd->WriteErrorMsg(error,__func__,__FILE__,__LINE__); \
        }

#define CMD_ERROR_FROM_EXCEPTION(cmd,error,e) \
        {\
            CSmallString text; \
            text << error << " (reason: " << e.what() << ")"; \
            ErrorSystem.AddError(text,__func__,__FILE__,__LINE__); \
            cmd->WriteErrorMsg(text,__func__,__FILE__,__LINE__); \
        }

//------------------------------------------------------------------------------

//! Server command data container
/*! \ingroup server
    The command execution is done by CRunnable in CServer::ThreadPool
*/

class NETLIB_PACKAGE CServerCommand : public CRunnable, public CCommandCore {
public:
    CServerCommand(CServer* p_server,CSocket p_socket,sockaddr_in& clientInfo);
    virtual ~CServerCommand(void);

// server part ----------------------------------------------------------------
    //! get command operation object
    const COperation  GetOperation(void);

    //! write error message plus method location to error stream
    void WriteErrorMsg(const CSmallString& error,
                       const CSmallString& fce,
                       const CSmallString& file,int line);

    //! write main return value
    void WriteMainReturnValue(bool finished_ok);

    //! get server
    CServer* GetServer(void);

    //! get client name
    const CSmallString& GetClientName(void);

// section of private data ----------------------------------------------------
private:
    CServer*                Server;             // server
    CSmallString            ClientIP;           // client IP
    CSmallString            ClientName;         // client network name
    unsigned char*          DataArray;
    bool                    IsProtocolOK;
    bool                    IsPasswdOK;

    virtual void Run(void);

    bool ReadHeader(void);
    bool WriteAuth(void);
    bool ReadRequest(void);
    bool ProcessRequest(void);
    bool WriteResult(void);
    bool WaitForEndOfStream(void);
};

//---------------------------------------------------------------------------
#endif

