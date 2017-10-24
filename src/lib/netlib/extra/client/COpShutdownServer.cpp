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
#include <stdlib.h>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

#define PASS_LENGTH 5

bool CExtraClient::ShutdownServer(std::ostream& vout,bool nopass)
{
    if( ! nopass ){
        CSmallString passphrase;

        // set random generator
        time_t now = time(NULL);
        srand(now);
        passphrase.SetLength(PASS_LENGTH);
        int pos = 0;
        int attempts = 0;
        do {
            attempts++;
            if( attempts % 1000 == 0 ){
                time_t now = time(NULL);
                srand(now);
            }
            char c = rand()%(126-33) + 33;
            if( (c == '*') || (c == '#') || (c == '!') || (c == '"') || (c == '\'') ) continue;
            passphrase[pos++] = c;
        } while(pos < PASS_LENGTH);

        vout << endl;
        vout << "Generated security passphrase : " << passphrase << endl;
        vout << "Rewrite passphrase to shutdown: ";
        string user;
        cin >> user;
        if( user != string(passphrase) ){
            vout << "Passphrases do not match - exiting ..." << endl;
            return(false);
        } else {
            vout << "Passphrases match - shutting down the server ..." << endl;
        }
    }

    CClientCommand cmd;
    try{

        // init command
        InitCommand(&cmd,Operation_ShutdownServer);

        // execute command
        ExecuteCommand(&cmd);

        // print response
        GetShortServerInfo(&cmd,vout);

    } catch(std::exception& e) {
        ES_ERROR_FROM_EXCEPTION("unable to process command",e);
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
