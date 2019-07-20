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
#include <ServerCommand.hpp>
#include <CmdProcessor.hpp>
#include <CmdProcessorList.hpp>
#include <CmdHeader.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <SimpleCompress.hpp>
#include <Server.hpp>
#include <XMLParser.hpp>
#include <XMLPrinter.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CServerCommand::CServerCommand(CServer* p_server,CSocket p_socket,
        sockaddr_in& clientInfo)
{
    Server = p_server;
    Socket = p_socket;

    CommandStatus = ECS_NEW;
    IsProtocolOK = false;
    IsPasswdOK = false;

    DataArray = NULL;
    DebugMode = false;

    ClientIP = inet_ntoa((in_addr)clientInfo.sin_addr);
    hostent *h = gethostbyaddr((const char*)&clientInfo.sin_addr,4,AF_INET);
    if( h != NULL ) {
        ClientName = h->h_name;
    }

}

//---------------------------------------------------------------------------

CServerCommand::~CServerCommand(void)
{
    if( Socket != INVALID_SOCKET) closesocket(Socket);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const COperation CServerCommand::GetOperation(void)
{
    CXMLElement* p_ele = GetRootCommandElement();
    if( p_ele == NULL ) return(Operation_IllegalOperation);

    CSmallString soperation;
    if( p_ele->GetAttribute("command",soperation) == false ) {
        return(Operation_IllegalOperation);
    }

    return(CmdProcessorList.GetOperation(soperation));
}

//---------------------------------------------------------------------------

void CServerCommand::WriteErrorMsg(const CSmallString& error,
                                   const CSmallString& fce,
                                   const CSmallString& file,int line)
{
    CXMLElement* p_ele = GetRootErrorElement();
    CXMLElement* p_sele = p_ele->CreateChildElement("ERROR");

    CSmallTimeAndDate time;
    time.GetActualTimeAndDate();

    p_sele->SetAttribute("text",error);
    p_sele->SetAttribute("fce",fce);
    p_sele->SetAttribute("file",file);
    p_sele->SetAttribute("line",line);
    p_sele->SetAttribute("time",time);

    WriteMainReturnValue(false);
}

//---------------------------------------------------------------------------

void CServerCommand::WriteMainReturnValue(bool finished_ok)
{
    CXMLElement* p_ele = GetRootErrorElement();
    if( p_ele == NULL ) return;
    p_ele->SetAttribute("main_result",finished_ok);
}

//---------------------------------------------------------------------------

CServer* CServerCommand::GetServer(void)
{
    return(Server);
}

//---------------------------------------------------------------------------

const CSmallString& CServerCommand::GetClientName(void)
{
    return(ClientName);
}

//==============================================================================
//------------------------------------------------------------------------------
// SERVER SERVER SERVER SERVER SERVER SERVER SERVER SERVER SERVER SERVER SERVER
//------------------------------------------------------------------------------
//==============================================================================

void CServerCommand::Run(void)
{
    DataArray = NULL;

    bool result = true;

    // read header
    CommandStatus = ECS_READING;
    result &= ReadHeader();

    // write AUTH response
    if( result == true ) {
        CommandStatus = ECS_WRITING;
        result &= WriteAuth();
    }

    // read request
    if( result == true ) {
        CommandStatus = ECS_READING;
        result &= ReadRequest();
    }

    // destroy request data if exist
    if( DataArray != NULL ) {
        delete[] DataArray;
        DataArray = NULL;
    }

    // process data
    if( result == true ) {
        CommandStatus = ECS_PROCESSING;
        result &= ProcessRequest();
    }

    // write result
    if( result == true ) {
        CommandStatus = ECS_WRITING;
        result &= WriteResult();
    }

    if( result == true ) {
        result = WaitForEndOfStream();
    }

    // destroy result data if exist
    if( DataArray != NULL ) {
        delete[] DataArray;
        DataArray = NULL;
    }

    if( result == true ) {
        CommandStatus = ECS_FINISHED;
    } else {
        CommandStatus = ECS_ABORTED;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CServerCommand::ReadHeader(void)
{

    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    // read header;
    if( ReadData((unsigned char*)&Header,sizeof(Header)) == false ) {
        CSmallString error;
        error << "unable to read header from client '" << ClientIP << " (" << ClientName << ")'";
        ES_ERROR(errpr);
        return(false);
    }

    if( Header.CheckHeader() == false ) {
        CSmallString error;
        error << "mismatch in header from client '" << ClientIP << " (" << ClientName << ")'";
        ES_ERROR(error);
        return(false);
    }

    IsProtocolOK = Header.CheckProtocolName(Server->GetProtocolName());
    if( IsProtocolOK == false ) {
        Server->IncrementIllegalTransaction();
        ES_ERROR("bad protocol");
    }

    IsPasswdOK = Header.CheckPassword(Server->GetPassword());
    if( IsPasswdOK == false ) {
        Server->IncrementIllegalTransaction();
        ES_ERROR("bad password");
    }

    CommandID = Header.GetCommandID();

    return(true);
}

//------------------------------------------------------------------------------

bool CServerCommand::WriteAuth(void)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    // send auth data -------------------------------------------------------------
    if( IsProtocolOK == true ) {
        Header.SetProtocolNameOK();
    } else {
        Header.SetProtocolNameBad();
    }

    if( IsPasswdOK == true ) {
        Header.SetPasswordOK();
    } else {
        Header.SetPasswordBad();
    }

    if( WriteData((unsigned char*)&Header,sizeof(Header)) == false ) {
        ES_ERROR("unable to send command header");
        return(false);
    }

    return(IsPasswdOK && IsProtocolOK);
}

//------------------------------------------------------------------------------

bool CServerCommand::ReadRequest(void)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    // prepare buffer
    DataArray = new unsigned char[Header.GetCompressedLength()];
    if( DataArray == NULL ) {
        ES_ERROR("memory problem");
        return(false);
    }

    // read data
    if( ReadData(DataArray,Header.GetCompressedLength()) == false ) {
        ES_ERROR("unable to read data");
        return(false);
    }

    CSimpleCompress compressor;
    // original data are destroyed in all cases
    DataArray = compressor.DecompressData(DataArray,Header.GetLength(),Header.GetCompressedLength());

    if( DataArray == NULL ) {
        ES_ERROR("unable to decompress data");
        return(false);
    }

    // load data
    CXMLParser xmlparser;

    xmlparser.SetOutputXMLNode(&Request);

    if( xmlparser.Parse(DataArray,Header.GetLength()) == false ) {
        ES_ERROR("unable to encode data");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CServerCommand::ProcessRequest(void)
{
    // create server processor
    CCmdProcessor* p_processor;
    p_processor = CmdProcessorList.CreateProcessor(this);

    if( p_processor == NULL ) {
        // set eror message
        CMD_ERROR(this,"unable to get command processor");
        return(true);
    }

    // process command
    bool result = p_processor->ProcessCommand();

    delete p_processor;

    WriteMainReturnValue(result);

    if( result == false ) {
        CMD_ERROR(this,"command processor execution failed");
    }

    // this return value has to be true - otherwise client will be disconnected
    return(true);
}

//------------------------------------------------------------------------------

bool CServerCommand::WriteResult(void)
{
    // if write error occurs then the client is disconnect
    // therefore none error meassage can reach client endpoint

    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    CXMLPrinter xmlprinter;

    xmlprinter.SetOutputFormat(EXF_BXML);
    xmlprinter.SetPrintedXMLNode(&Result);

    // prepare one block data
    unsigned int length = 0;
    unsigned int compress_length = 0;

    DataArray = xmlprinter.Print(length);
    if( DataArray == NULL ) {
        ES_ERROR("unable to get data as one block");
        return(false);
    }
    Result.RemoveAllChildNodes();

    CSimpleCompress compressor;
    // original data are destroyed in all cases

    DataArray = compressor.CompressData(DataArray,length,compress_length);
    if( DataArray == NULL ) {
        ES_ERROR("unable to compress data");
        return(false);
    }

    // send all data -------------------------------------------------------------
    Header.SetLength(length);
    Header.SetCompressedLength(compress_length);

    if( WriteData((unsigned char*)&Header,sizeof(Header)) == false ) {
        ES_ERROR("unable to send command header");
        return(false);
    }

    if( WriteData(DataArray,compress_length) == false ) {
        ES_ERROR("unable to send result data");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CServerCommand::WaitForEndOfStream(void)
{
    //   No, keep selecting for 'read' and wait until 'read' returns zero. That's
    // how a closed connection is indicated to a non-blocking program.

    fd_set     ready_to_read;
    timeval    wait_time;

    for(int i=0; i < 200; i++ ) {

        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 50000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_read);
        FD_SET(Socket,&ready_to_read);

        int result = select(Socket+1,&ready_to_read,NULL,NULL,&wait_time);
        if( result <= 0 ) continue;

        // data ready?
        if( FD_ISSET(Socket,&ready_to_read) == 0 ) {
            continue;
        }
        char buffer[100];
        // FIXME? - we had MSG_DONTWAIT here but it is actually needed if select is used?
        int recv_len = recv(Socket,buffer,100,0);
        if( recv_len == SOCKET_ERROR ) {
            if( errno == EAGAIN ) continue;
            ES_ERROR("recv failed");
            return(false);
        }
        if( recv_len == 0 ) break;  // connection was terminated by a client
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
