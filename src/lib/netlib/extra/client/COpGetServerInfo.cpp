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

#include <stdio.h>
#include <ExtraClient.hpp>
#include <Operation.hpp>
#include <ErrorSystem.hpp>
#include <ExtraOperation.hpp>
#include <SmallTime.hpp>
#include <RegClient.hpp>
#include <XMLElement.hpp>
#include <iostream>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CExtraClient::GetServerInfo(std::ostream& vout)
{
    CClientCommand cmd;
    try{

        // init command
        InitCommand(&cmd,Operation_GetServerInfo);

        // execute command
        ExecuteCommand(&cmd);

        // print response
        GetShortServerInfo(&cmd,vout);
        GetLongServerInfo(&cmd,vout);

    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to process command",e);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

void CExtraClient::GetShortServerInfo(CClientCommand* p_command,ostream& sout)
{
    if( p_command == NULL ){
        INVALID_ARGUMENT("p_command is NULL");
    }

    // now print results retrived from server
    CXMLElement* p_ele = p_command->GetRootResultElement();

    int pt = 0;    // processed transactions
    int it = 0;    // illegal transaction

    bool result = true;

    result &= p_ele->GetAttribute("pt",pt);
    result &= p_ele->GetAttribute("it",it);

    if( result == false ) {
        LOGIC_ERROR("unable to get attributes of short server description");
    }

    // write short summary
    sout << endl;
    sout << "=== Short server summary =======================================================" << endl;
    sout << endl;
    sout << " Number of processed transactions: " << pt << endl;
    sout << " Number of illegal transactions  : " << it << endl;
}

//------------------------------------------------------------------------------

void CExtraClient::GetLongServerInfo(CClientCommand* p_command,ostream& sout)
{
    if( p_command == NULL ){
        INVALID_ARGUMENT("p_command is NULL");
    }
    // now print results retrived from server
    CXMLElement* p_ele = p_command->GetRootResultElement();

    // write long summary
    sout << endl;
    sout << "=== Clients ====================================================================" << endl;
    sout << endl;
    sout << " ID  ST    Registered since      Last operation      Unregistered since     OPs " << endl;
    sout << "---- -- --------------------- --------------------- --------------------- ------" << endl;

    CXMLElement*   p_cl = p_ele->GetChildElementByPath("CLIENTS/CLIENT");

    while( p_cl != NULL ) {
        CRegClient client;

        client.LoadInfo(p_cl);
        client.PrintInfo(sout);

        p_cl = p_cl->GetNextSiblingElement("CLIENT");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

