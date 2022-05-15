// =============================================================================
// ChemInfo - Chemoinformatics Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <ExtraProcessor.hpp>
#include <ExtraServer.hpp>
#include <XMLElement.hpp>
#include <ServerCommand.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CExtraProcessor::UnregisterClient(void)
{
    CExtraServer* p_server = dynamic_cast<CExtraServer*>(Command->GetServer());
    if( p_server == NULL ){
        LOGIC_ERROR("p_server is not inherited from CExtraServer");
    }

    int client_id = -1;

    // get client ID
    if( CommandElement->GetAttribute("client_id",client_id) == false ) {
        LOGIC_ERROR("unable to get client_id");
    }

    //  and unregister it from list
    p_server->RegClients.UnregisterClient(client_id);

    if( p_server->RegClients.GetNumberOfActiveRegistration() == 0 ) {
        if( p_server->DoNotShutdown == false ) p_server->TerminateServer();
    }

    if( p_server->TargetRegs > 0 ){
        if( (p_server->RegClients.GetNumberOfClients() >= p_server->TargetRegs) &&
            (p_server->RegClients.GetNumberOfActiveRegistration() == 0) ){
            p_server->TerminateServer();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
