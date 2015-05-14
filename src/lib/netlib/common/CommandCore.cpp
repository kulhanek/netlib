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

#include <errno.h>
#include <CommandCore.hpp>
#include <CmdHeader.hpp>
#include <ErrorSystem.hpp>
#include <SimpleCompress.hpp>
#include <XMLElement.hpp>
#include <MemBlockFile.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>

// AIX fix ---------------------
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

//---------------------------------------------------------------------------

CIndexCounter CCommandCore::CommandIDCounter;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCommandCore::CCommandCore(void)
{
    CommandID  = CommandIDCounter.GetIndex();
    CommandStatus     = ECS_NEW;
    Terminated = false;

    DebugMode = false;

    Socket = INVALID_SOCKET;
}

//---------------------------------------------------------------------------

CCommandCore::~CCommandCore(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ECommandStatus CCommandCore::GetStatus(void)
{
    return(CommandStatus);
}

//---------------------------------------------------------------------------

void CCommandCore::SetOperation(const COperation& operation)
{
    CXMLElement* p_ele = GetRootCommandElement();
    p_ele->SetAttribute("command",operation.GetStringForm());
}

//---------------------------------------------------------------------------

const CSmallString  CCommandCore::GetSOperation(void)
{
    CXMLElement* p_ele = GetRootCommandElement();
    if( p_ele == NULL ) return("");

    CSmallString soperation;
    if( p_ele->GetAttribute("command",soperation) == false ) {
        return("");
    }
    return(soperation);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXMLElement* CCommandCore::GetRootCommandElement(void)
{
    CXMLElement* p_ele;
    p_ele = Request.GetChildElementByPath("ROOT/COMMAND",true);
    return(p_ele);
}

//---------------------------------------------------------------------------

CXMLElement* CCommandCore::GetCommandElementByPath(const CSmallString& path,bool create)
{
    CXMLElement* p_ele;
    p_ele = Request.GetChildElementByPath("ROOT/COMMAND/"+path,create);
    return(p_ele);
}

//---------------------------------------------------------------------------

CXMLElement* CCommandCore::GetRootResultElement(void)
{
    CXMLElement* p_ele;
    p_ele = Result.GetChildElementByPath("ROOT/RESULTS",true);
    return(p_ele);
}

//---------------------------------------------------------------------------

CXMLElement* CCommandCore::GetResultElementByPath(const CSmallString& path,bool create)
{
    CXMLElement* p_ele;
    p_ele = Result.GetChildElementByPath("ROOT/RESULTS/"+path,create);
    return(p_ele);
}

//---------------------------------------------------------------------------

CXMLElement* CCommandCore::GetRootErrorElement(void)
{
    CXMLElement* p_ele;
    p_ele = Result.GetChildElementByPath("ROOT/ERRORS",true);
    return(p_ele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCommandCore::ReadData(unsigned char* p_data,unsigned int length)
{
    fd_set ready_to_read;

    timeval wait_time;

    unsigned int read_len = 0;

    while( read_len != length ) {
        if( Terminated == true ) {
            ES_ERROR("thread terminated");
            return(false);
        }

        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 200000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_read);
        FD_SET(Socket,&ready_to_read);

        int result = select(Socket+1,&ready_to_read,NULL,NULL,&wait_time);
        if( result == 0 ) continue;

        // data ready?
        if( FD_ISSET(Socket,&ready_to_read) == 0 ) {
            continue;
        }
        int send_len = recv(Socket,&p_data[read_len],length-read_len,MSG_DONTWAIT);
        if( send_len == SOCKET_ERROR ) {
            if( errno == EAGAIN ) continue;
            ES_ERROR("recv failed");
            return(false);
        }
        if( send_len == 0 ) return(false);
        read_len += send_len;
    }

    bool result = read_len == length;
    if( result == false ) {
        ES_ERROR("transaction was not completed");
        return(false);
    }

    return(result);
}

//---------------------------------------------------------------------------

bool CCommandCore::WriteData(unsigned char* p_data,unsigned int length)
{
    fd_set ready_to_write;

    timeval wait_time;

    unsigned int wrote_len = 0;

    while( wrote_len != length ) {
        if( Terminated == true ) {
            ES_ERROR("thread terminated");
            return(false);
        }

        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 200000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_write);
        FD_SET(Socket,&ready_to_write);

        int result = select(Socket+1,NULL,&ready_to_write,NULL,&wait_time);
        if( result == 0 ) continue;

        // write ready?
        if( FD_ISSET(Socket,&ready_to_write) == 0 ) {
            continue;
        }
        int send_len = send(Socket,&p_data[wrote_len],length-wrote_len,MSG_DONTWAIT);
        if( send_len == SOCKET_ERROR ) {
            if( errno == EAGAIN ) continue;
            ES_ERROR("send failed");
            return(false);
        }
        wrote_len += send_len;
    }

    bool result = wrote_len == length;
    if( result == false ) {
        ES_ERROR("transaction was not completed");
        return(false);
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



