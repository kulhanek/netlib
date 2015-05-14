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
#include <ExtraOperation.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CExtraClient::GetServerErrors(std::ostream& vout)
{
    CClientCommand cmd;
    try{

        // init command
        InitCommand(&cmd,Operation_GetServerErrors);

        // execute command
        ExecuteCommand(&cmd);

        // print response
        CXMLElement* p_ele = cmd.GetResultElementByPath("ERRORS",false);
        if( p_ele == NULL ) {
            LOGIC_ERROR("unable to open ERRORS element");
        }

        CErrorSystem errcp;
        errcp.LoadErrors(p_ele);
        errcp.PrintErrors(vout);

    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to process command",e);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

