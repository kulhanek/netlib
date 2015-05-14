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

#include <ExtraClient.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtraClient::CExtraClient(void)
{
}

//------------------------------------------------------------------------------

bool CExtraClient::ReadPassword(bool read_pswfile,
                                const CSmallString& pswfile,bool verbose)
{
    // read password
    if( read_pswfile == false ) {
        fprintf(stderr,"%s's password: ",(const char*)ActionRequest.GetName());
        fflush(stderr);
    } else {
        if( verbose == true ) {
            fprintf(stdout,"%s's password: ",(const char*)ActionRequest.GetName());
            fflush(stdout);
        }
    }

    CPassword pswd;

    bool result;
    if( read_pswfile == false ) {
        result = pswd.ReadFromCmdLine();
    } else {
        if( verbose == true ) fprintf(stdout,"taken from file '%s'\n",(const char*)pswfile);
        result = pswd.ReadFromFile(pswfile);
    }

    if( result == false ) {
        return(false);
    }

    ActionRequest.SetPassword(pswd.GetPassword());

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
