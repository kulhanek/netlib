#ifndef ClientCommandH
#define ClientCommandH
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

#include <NETLibMainHeader.hpp>
#include <XMLDocument.hpp>
#include <SmartThread.hpp>
#include <Operation.hpp>
#include <IndexCounter.hpp>
#include <Network.hpp>
#include <CmdHeader.hpp>
#include <CommandCore.hpp>

//------------------------------------------------------------------------------

class CClient;

//------------------------------------------------------------------------------

//! Client command data container and worker thread
/*! \ingroup client
*/

class NETLIB_PACKAGE CClientCommand : public CCommandCore, private CSmartThread {
public:
            CClientCommand(void);
            CClientCommand(CClient* p_client);
    virtual ~CClientCommand(void);

// client part ----------------------------------------------------------------
    //! retreive command result
    bool GetMainReturnValue(void);

// section of private data ----------------------------------------------------
private:
    CClient*    		Client;
    unsigned char*  	DataArray;

    virtual void ExecuteThread(void);

    void SetClient(CClient* p_client);

    bool ProcessCommand(void);
    bool PrepareRequest(void);
    bool WriteHeader(void);
    bool ReadAuth(void);
    bool WriteRequest(void);
    bool ReadResult(void);
    bool DisconnectFromServerLowLevel(void);

    friend class CClient;
};

//---------------------------------------------------------------------------
#endif

