#ifndef ClientH
#define ClientH
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
#include <SmallTime.hpp>
#include <SmartThread.hpp>
#include <Operation.hpp>
#include <ActionRequest.hpp>
#include <Network.hpp>
#include <SimpleMutex.hpp>
#include <ClientCommand.hpp>

//---------------------------------------------------------------------------

class CClientCommand;

//------------------------------------------------------------------------------

/*! \brief Client core class
    \ingroup client
*/

class NETLIB_PACKAGE CClient {
public:
    CClient(void);
    virtual ~CClient(void);

// execution methods ----------------------------------------------------------
    //! create a new command
    CClientCommand* CreateCommand(const COperation& operation);

    //! init an existing command object
    void InitCommand(CClientCommand* p_cmd,const COperation& operation);

    //! execute command
    /*!
      if async is true, then p_command MUST be allocated on heap by the new operator,
      command is never own by CClient
    */
    void ExecuteCommand(CClientCommand* p_command,bool async=false);

    //! set debug mode
    void SetDebugMode(bool set);

// server information ---------------------------------------------------------
    //! return number of executed commands
    int GetNumberOfTransactions(void);

public:
    CActionRequest              ActionRequest;          //!< request description

// private data section ------------------------------------------------------
private:
    bool                        DebugMode;              //!< it will print transmitted data to disk
    CSimpleList<CClientCommand> Commands;               //!< processing commands
    int                         NumOfExecutedCommands;  //!< number of executed commands
    CSimpleMutex                CommandListMutex;       //!< access mutex to Commands list

    //! connect to server
    CSocket ConnectToServerLowLevel(void);

    friend class CClientCommand;
};

//---------------------------------------------------------------------------
#endif
