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

void CExtraProcessor::ShutdownServer(void)
{
    CExtraServer* p_server = dynamic_cast<CExtraServer*>(Command->GetServer());
    if( p_server == NULL ){
        LOGIC_ERROR("p_server is not inherited from CExtraServer");
    }

    // complete response to client
    ResultElement->SetAttribute("pt",p_server->GetNumberOfTransactions());
    ResultElement->SetAttribute("it",p_server->GetNumberOfIllegalTransactions());

    // initiate server shutdown
    p_server->TerminateServer();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

