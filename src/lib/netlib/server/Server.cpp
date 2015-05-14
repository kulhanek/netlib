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
#include <ErrorSystem.hpp>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <ServerCommand.hpp>
#include <sys/utsname.h>
#include <fstream>
#include <sys/stat.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

// maximum trials for free port opening
#define MAX_TRIALS       200

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CServer::CServer(void)
{
    // server information
    ProtocolName = "netlib";
    ServerPort = INVALID_PORT;

    Terminated = false;

    NumOfTransactions = 0;
    NumOfIllegalTransactions = 0;

    MaxListenQueueLen = 30;
    ThreadPool.SetMaxThreadCount(5);

    // variable initialization
    ListenSocket = INVALID_SOCKET;
}

//---------------------------------------------------------------------------

CServer::~CServer(void)
{
    if( ListenSocket != INVALID_SOCKET ) {
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CServer::InitServer(bool gen_password)
{
    // generate password if necessary
    if( gen_password ) {
        // set random generator
        time_t now = time(NULL);
        srandom(now);
        Password.SetLength(MAGIC_LENGTH);
        int pos = 0;
        do {
            char c = random()%(126-33) + 33;
            if( (c == '*') || (c == '#') || (c == '!') || (c == '"') || (c == '\'') ) continue;
            Password[pos++] = c;
        } while(pos < MAGIC_LENGTH);
    }

    // select free port
    return(InitServer(0));
}

//------------------------------------------------------------------------------

bool CServer::InitServer(int port)
{
    Terminated = false;

    // port 0 means to select a free port

    if( port != 0 ) {
        // check if port is in correct range
        if( (port <= 1024) && (port > 65535) ) {
            ES_ERROR("server port is not in correct range <1024,65535>");
            return(false);
        }
    }
    ServerPort = port;

    int result;
    if( ListenSocket != INVALID_SOCKET ) {
        ES_ERROR("server socket already exists");
        return(false);
    }

    // create socket
    ListenSocket = socket(AF_INET,SOCK_STREAM,0);
    if( ListenSocket == INVALID_SOCKET ) {
        CSmallString error;
        error << "unable to open server socket: " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }

    // bind to port
    sockaddr_in    server_addr;

    if( ServerPort > 0 ) {
        memset(&server_addr,0,sizeof(server_addr));
        server_addr.sin_port = htons(ServerPort);
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;

        result = bind(ListenSocket,(sockaddr*)&server_addr,sizeof(server_addr));
        if( result == SOCKET_ERROR ) {
            CSmallString error;
            error << "unable to bind to port: " << strerror(errno);
            ES_ERROR(error);
            closesocket(ListenSocket);
            ListenSocket = INVALID_SOCKET;
            return(false);
        }
    } else {
        std::vector<int>     used_ports;
        // set random generator
        time_t now = time(NULL);
        srandom(now);
        do {
            ServerPort = random()%(65535-1025) + 1025;
            //! TODO - check if the port was not already generated
            used_ports.push_back(ServerPort);

            memset(&server_addr,0,sizeof(server_addr));
            server_addr.sin_port = htons(ServerPort);
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = INADDR_ANY;

            result = bind(ListenSocket,(sockaddr*)&server_addr,sizeof(server_addr));
        } while( (result == SOCKET_ERROR) && (used_ports.size() < MAX_TRIALS )  );

        if( result == SOCKET_ERROR ) {
            CSmallString error;
            error << "unable to find free port (last error) : " << strerror(errno);
            ES_ERROR(error);
            closesocket(ListenSocket);
            ListenSocket = INVALID_SOCKET;
            return(false);
        }
    }

    // start listenning
    result = listen(ListenSocket,MaxListenQueueLen);
    if(result == SOCKET_ERROR) {
        CSmallString error;
        error << "unable to listen on server port: " << strerror(errno);
        ES_ERROR(error);
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

void CServer::TerminateServer(void)
{
    Terminated = true;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CServer::GetNumberOfTransactions(void)
{
    return(NumOfTransactions);
}

//------------------------------------------------------------------------------

int CServer::GetNumberOfIllegalTransactions(void)
{
    return(NumOfIllegalTransactions);
}

//------------------------------------------------------------------------------

void CServer::SetPassword(const CSmallString& passwd)
{
    Password = passwd;
}

//------------------------------------------------------------------------------

const CSmallString& CServer::GetPassword(void)
{
    return(Password);
}

//------------------------------------------------------------------------------

void CServer::SetProtocolName(const CSmallString& protocol)
{
    ProtocolName = protocol;
}

//------------------------------------------------------------------------------

const CSmallString& CServer::GetProtocolName(void) const
{
    return(ProtocolName);
}

//------------------------------------------------------------------------------

bool CServer::SetMaxListenQueueLen(int maxcons)
{
    // check if number of maximal conncetions is correct range
    if( maxcons < 0 ) {
        ES_ERROR("number of connection has to greater than zero");
        return(false);
    }
    MaxListenQueueLen = maxcons;
    return(true);
}

//------------------------------------------------------------------------------

int CServer::GetMaxListenQueueLen(void)
{
    return(MaxListenQueueLen);
}

//------------------------------------------------------------------------------

bool CServer::SetMaxNumOfWorkers(int maxthrds)
{
    return(ThreadPool.SetMaxThreadCount(maxthrds));
}

//------------------------------------------------------------------------------

int CServer::GetMaxNumOfWorkers(void)
{
    return(ThreadPool.GetMaxThreadCount());
}

//------------------------------------------------------------------------------

int CServer::GetPort(void)
{
    return(ServerPort);
}

//------------------------------------------------------------------------------

void CServer::IncrementIllegalTransaction(void)
{
    AccessMutex.Lock();
        NumOfIllegalTransactions++;
    AccessMutex.Unlock();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CServer::WriteServerKey(const CSmallString& server_key_name)
{
    ofstream ofs;
    ofs.open(server_key_name);
    if( ! ofs ){
        CSmallString error;
        error << "unable to open server key file (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    ofs << "[server-key]" << endl;
    ofs << "name     " << GetServerHostName() << endl;
    ofs << "port     " << ServerPort << endl;
    ofs << "password \"" << Password << "\"" << endl << endl;

    ofs.close();

    // change mode of server key
    chmod(server_key_name,S_IRUSR|S_IWUSR);

    return(true);
}

//------------------------------------------------------------------------------

const CSmallString CServer::GetServerHostName(void)
{
    struct addrinfo hints, *info, *p;
    int gai_result;

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
        CSmallString error;
        error << "getaddrinfo: " << gai_strerror(gai_result);
        ES_ERROR(error);
        return(hostname);
    }
    CSmallString hname;
    for(p = info; p != NULL; p = p->ai_next) {
        hname = p->ai_canonname;
    }

    freeaddrinfo(info);
    return(hname);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CServer::AcceptServerRequests(void)
{
    fd_set     ready_to_read;
    timeval    wait_time;

    while( Terminated != true ) {
        // it is necessary to reset time since it is modified by select
        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 200000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_read);
        FD_SET(ListenSocket,&ready_to_read);

        int result = select(ListenSocket+1,&ready_to_read,NULL,NULL,&wait_time);
        if( result == 0 ) continue;
        if( result == -1 ) {
            CSmallString error;
            error << "unable to select :" << strerror(errno);
            ES_ERROR(error);
            break;
        }

        // process events

        // new connection ?
        if( FD_ISSET(ListenSocket,&ready_to_read) != 0 ) {
            // accept client connection
            sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);

            CSocket accept_socket = accept(ListenSocket,(sockaddr*)&client_addr,&addrlen);

            if( accept_socket != INVALID_SOCKET ) {
                CServerCommand* p_client = new CServerCommand(this,accept_socket,client_addr);
                if( p_client != NULL ) {
                    p_client->SetAutoDelete(true);
                    NumOfTransactions++;
                    ThreadPool.AddTask(p_client);
                } else {
                    // write error message here - safely !!!!
                }
            }
        }
    }

    // destroy all server commands
    ThreadPool.WaitForDone();

    // close socket
    closesocket(ListenSocket);
    ListenSocket = INVALID_SOCKET;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


