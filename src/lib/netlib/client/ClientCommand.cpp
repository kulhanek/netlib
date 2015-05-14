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
#include <ClientCommand.hpp>
#include <CmdHeader.hpp>
#include <ErrorSystem.hpp>
#include <Client.hpp>
#include <SimpleCompress.hpp>
#include <XMLElement.hpp>
#include <MemBlockFile.hpp>
#include <XMLPrinter.hpp>
#include <XMLParser.hpp>
#include <XMLIterator.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CClientCommand::CClientCommand(void)
{
    Client = NULL;
    DataArray = NULL;
}

//------------------------------------------------------------------------------

CClientCommand::CClientCommand(CClient* p_client)
{
    Client = p_client;
    DataArray = NULL;
}

//---------------------------------------------------------------------------

CClientCommand::~CClientCommand(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CClientCommand::GetMainReturnValue(void)
{
    CXMLElement* p_ele = GetRootErrorElement();
    if( p_ele == NULL ) return(false);
    bool set = false;
    p_ele->GetAttribute("main_result",set);
    return(set);
}

//------------------------------------------------------------------------------

void CClientCommand::SetClient(CClient* p_client)
{
    if( p_client == NULL ){
        INVALID_ARGUMENT("p_client is NULL");
    }
    if( Client != NULL ){
        LOGIC_ERROR("command is already assignet to the client");
    }
    Client = p_client;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CClientCommand::ExecuteThread(void)
{
    CommandStatus = ECS_NEW;

    bool result = true;

    // connect to server
    Socket = Client->ConnectToServerLowLevel();
    if( Socket == INVALID_SOCKET ) {
        CommandStatus = ECS_ABORTED;
        result = false;
    }

    // execute command
    if( result == true ) result &= ProcessCommand();

    // close socket
    if( Socket != INVALID_SOCKET) DisconnectFromServerLowLevel();

    // release command from client
    Client->CommandListMutex.Lock();
    Client->Commands.Remove(this);
    Client->CommandListMutex.Unlock();

    return;
}

//==============================================================================
//------------------------------------------------------------------------------
// CLIENT CLIENT CLIENT CLIENT CLIENT CLIENT CLIENT CLIENT CLIENT CLIENT CLIENT
//------------------------------------------------------------------------------
//==============================================================================

bool CClientCommand::ProcessCommand(void)
{
    DataArray = NULL;

    bool result = true;

    // debug part ===============
    CSmallString   dbg_xml_name;
    CSmallString   sid;
    sid.IntToStr(CommandID,"%04d");
    dbg_xml_name = "client_" + sid + "_" + GetSOperation();

    if( DebugMode == true ) {
        CXMLPrinter xml_printer;
        xml_printer.SetPrintedXMLNode(&Request);
        xml_printer.Print(dbg_xml_name+"_command.xml");
    }

    // prepare request
    CommandStatus = ECS_PREPARING;
    result &= PrepareRequest();

    // write header
    if( result == true ) {
        CommandStatus = ECS_WRITING;
        result &= WriteHeader();
    }

    // get AUTH response
    if( result == true ) {
        CommandStatus = ECS_READING;
        result &= ReadAuth();
    }

    // write request
    if( result == true ) {
        CommandStatus = ECS_WRITING;
        result &= WriteRequest();
    }

    // destroy request data if exist
    if( DataArray != NULL ) {
        delete[] DataArray;
        DataArray = NULL;
    }

    if( result == true ) {
        CommandStatus = ECS_READING;
        result &= ReadResult();
    }

    // destroy result data if exist
    if( DataArray != NULL ) {
        delete[] DataArray;
        DataArray = NULL;
    }

    // inject server errors into client error stack
    CXMLElement* p_ele = GetRootErrorElement();
    ErrorSystem.AddAsRemoteErrors(p_ele);

    if( result == true ) {

        if( DebugMode == true ) {
            CXMLPrinter xml_printer;
            xml_printer.SetPrintedXMLNode(&Result);
            xml_printer.Print(dbg_xml_name+"_result.xml");
        }

        CommandStatus = ECS_FINISHED;
        return(true);
    } else {
        CommandStatus = ECS_ABORTED;
        return(false);
    }
}

//------------------------------------------------------------------------------

bool CClientCommand::DisconnectFromServerLowLevel(void)
{
    if( Socket == INVALID_SOCKET ) return(false);
    closesocket(Socket);
    return(true);
}

//---------------------------------------------------------------------------

bool CClientCommand::PrepareRequest(void)
{
    CXMLPrinter xmlprinter;

    xmlprinter.SetOutputFormat(EXF_BXML);
    xmlprinter.SetPrintedXMLNode(&Request);

    // compress data
    unsigned length = 0;
    unsigned compress_length = 0;

    unsigned char* p_data = xmlprinter.Print(length);
    if( p_data == NULL ) {
        ES_ERROR("memory problem");
        return(false);
    }

    CSimpleCompress compressor;
    // original data are destroyed in all cases
    DataArray = compressor.CompressData(p_data,length,compress_length);
    if( DataArray == NULL ) {
        ES_ERROR("unable to compress data");
        return(false);
    }

    Header.SetCommandID(CommandID);
    Header.SetLength(length);
    Header.SetCompressedLength(compress_length);
    Header.SetProtocolName(Client->ActionRequest.GetProtocolName());
    Header.SetPassword(Client->ActionRequest.GetPassword());

    return(true);
}

//---------------------------------------------------------------------------

bool CClientCommand::WriteHeader(void)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    if( WriteData((unsigned char*)&Header,sizeof(Header)) == false ) {
        ES_ERROR("unable to send command header");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CClientCommand::ReadAuth(void)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    // read header
    if( ReadData((unsigned char*)&Header,sizeof(Header)) == false ) {
        ES_ERROR("unable to read command header");
        return(false);
    }

    // check header data
    if( Header.CheckHeader() == false ) {
        ES_ERROR("command header seems to be incorrect");
        return(false);
    }

    // check if command ID is OK
    if( Header.GetCommandID() != CommandID ) {
        ES_ERROR("command ID mismatch");
        return(false);
    }

    bool isProtocolOK = Header.WasProtocolNameOK();

    if( isProtocolOK == false ) {
        // do not read the rest of data
        ES_ERROR("protocol is incorrect");
        return(false);
    }

    bool isPasswdOK = Header.WasPasswordOK();

    if( isPasswdOK == false ) {
        // do not read the rest of data
        ES_ERROR("password is incorrect");
        return(false);
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CClientCommand::WriteRequest(void)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    if( DataArray == NULL ) {
        ES_ERROR("DataArray is NULL");
        return(false);
    }

    if( WriteData(DataArray,Header.GetCompressedLength()) == false ) {
        ES_ERROR("unable to send request");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CClientCommand::ReadResult(void)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("socket is invalid");
        return(false);
    }

    // read data ------------------------------------------------------------------

    // read header
    if( ReadData((unsigned char*)&Header,sizeof(Header)) == false ) {
        ES_ERROR("unable to read command header");
        return(false);
    }

    // check header data
    if( Header.CheckHeader() == false ) {
        ES_ERROR("command header seems to be incorrect");
        return(false);
    }

    // check if command ID is OK
    if( Header.GetCommandID() != CommandID ) {
        ES_ERROR("command ID mismatch");
        return(false);
    }

    // prepare buffer
    try {
        DataArray = new unsigned char[Header.GetCompressedLength()];
    } catch(...) {
        ES_ERROR("memory error");
        return(false);
    }

    // read buffer
    if( ReadData(DataArray,Header.GetCompressedLength()) == false ) {
        ES_ERROR("unable to read data");
        return(false);
    }

    CSimpleCompress compressor;
    // original data are destroyed in all cases
    DataArray = compressor.DecompressData(DataArray,Header.GetLength(),Header.GetCompressedLength());
    if( DataArray == NULL ) {
        return(false);
    }

    CXMLParser xmlparser;

    xmlparser.SetOutputXMLNode(&Result);

    if( xmlparser.Parse(DataArray,Header.GetLength()) == false ) {
        ES_ERROR("unable to decode result data");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



