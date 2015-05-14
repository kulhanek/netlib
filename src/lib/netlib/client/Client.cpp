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

#include <Client.hpp>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include <errno.h>
#include <string.h>
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CClient::CClient(void)
{
    NumOfExecutedCommands = 0;
    DebugMode = false; // if true all commands are printed to disk
}

//---------------------------------------------------------------------------

void CClient::SetDebugMode(bool set)
{
    DebugMode = set;
}

//---------------------------------------------------------------------------

CClient::~CClient(void)
{
    CClientCommand* p_command;

    while(true) {
        CommandListMutex.Lock();
            p_command = Commands.RemoveFromBegin();
        CommandListMutex.Unlock();

        if( p_command == NULL ) break;
        p_command->WaitForThread();     // wait until command is terminated
        // do not destroy - the command is not owned by CClient !!
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CClientCommand* CClient::CreateCommand(const COperation& operation)
{
    CClientCommand* p_command = NULL;
    p_command = new CClientCommand(this);
    p_command->SetOperation(operation);
    p_command->DebugMode = DebugMode;
    return(p_command);
}

//------------------------------------------------------------------------------

void CClient::InitCommand(CClientCommand* p_cmd,const COperation& operation)
{
    if( p_cmd == NULL ){
        INVALID_ARGUMENT("p_cmd is NULL");
    }
    // is it fresh command?
    if( p_cmd->GetStatus() != ECS_NEW ){
        LOGIC_ERROR("command does not have status ECS_NEW");
    }

    // init command
    p_cmd->SetClient(this);
    p_cmd->SetOperation(operation);
    p_cmd->DebugMode = DebugMode;
}

//------------------------------------------------------------------------------

void CClient::ExecuteCommand(CClientCommand* p_command,bool async)
{
    if( p_command == NULL ){
        INVALID_ARGUMENT("p_command is NULL");
    }

    bool is_in_list;

    CommandListMutex.Lock();
        is_in_list = Commands.IsInList(p_command);
    CommandListMutex.Unlock();

    if( is_in_list == true ) {
        LOGIC_ERROR("command is already in command queue");
    }

    if( p_command->Client != this ) {
        LOGIC_ERROR("wrong side");
    }

    // add AUTH element -----------------------------
    CXMLElement* p_cele = p_command->GetCommandElementByPath("AUTH",true);
    p_cele->SetAttribute("password",ActionRequest.GetPassword());
    p_cele->SetAttribute("protocol",ActionRequest.GetProtocolName());

    // now start thread -----------------------------
    CommandListMutex.Lock();
        try {
            NumOfExecutedCommands++;
            Commands.InsertToEnd(p_command);
        } catch(...) {
            CommandListMutex.Unlock();
            throw;
        }
    CommandListMutex.Unlock();

    // start command processing
    if( p_command->StartThread() == false ) {
        CommandListMutex.Lock();
            Commands.Remove(p_command);
        CommandListMutex.Unlock();
        RUNTIME_ERROR("unable to start command thread");
    }

    if( async == true ) return;  // return immediatelly

    // wait until command thread is terminated
    p_command->WaitForThread();

    // ExecutedCommand is now NULL !!!
    if( p_command->GetStatus() != ECS_FINISHED ) {
        RUNTIME_ERROR("command was not completed");
    }
    if( p_command->GetMainReturnValue() == false ) {
        RUNTIME_ERROR("server did not process command successfully");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSocket CClient::ConnectToServerLowLevel(void)
{
    CSocket mysocket;

    // create socket
    mysocket = socket(AF_INET,SOCK_STREAM,0);
    if( mysocket == INVALID_SOCKET ) {
        CSmallString error;
        error << "unable to create socket (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(INVALID_SOCKET);
    }

    // THIS DOES NOT WORK ON ALTIX.molmod.enzim.hu
    // It lookups for service in /etc/services even if service is port number
    // addrinfo       *p_addrinfo;
    //  int            nerr;
    //
    //  // get hostname and IP
    //  if( (nerr = getaddrinfo(ServerDesc.GetIP(),CSmallString(ServerDesc.GetPort()),NULL,&p_addrinfo)) != 0 ){
    //     CSmallString error;
    //     error << "unable to decode IP and port (" <<  gai_strerror(nerr) << ")";
    //     ES_ERROR(error, "CClient::ConnectToServerLowLevel");
    //     return(false);
    //     }
    //
    //  int result = connect(mysocket,p_addrinfo->ai_addr,p_addrinfo->ai_addrlen);
    //
    //  freeaddrinfo(p_addrinfo);

    sockaddr_in    server_addr;

    memset(&server_addr,0,sizeof(server_addr));

    server_addr.sin_port = htons(ActionRequest.GetPort());
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ActionRequest.GetIP());

    int result = connect(mysocket,(sockaddr*)&server_addr,sizeof(server_addr));

    if( result == SOCKET_ERROR ) {
        CSmallString error;
        error << "unable to connect to server (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(INVALID_SOCKET);
    }

    return(mysocket);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CClient::GetNumberOfTransactions(void)
{
    return(NumOfExecutedCommands);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

