#ifndef CmdProcessorH
#define CmdProcessorH
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
#include <SmallString.hpp>
#include <Operation.hpp>

//------------------------------------------------------------------------------

class CServerCommand;
class CXMLElement;

//------------------------------------------------------------------------------

//! Command processor base class.
/*! \ingroup server
    Command processing should be put into the virtual method - ProcessCommand.
*/

class NETLIB_PACKAGE CCmdProcessor {
public:
    CCmdProcessor(CServerCommand* p_cmd);
    virtual ~CCmdProcessor(void);

    //! process command
    virtual bool ProcessCommand(void);

// section of protected data --------------------------------------------------
protected:
    CServerCommand*         Command;            // associated comand
    COperation              Operation;          // and operation

    CXMLElement*            CommandElement;
    CXMLElement*            ResultElement;
};

//---------------------------------------------------------------------------
#endif
