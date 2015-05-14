#ifndef CmdProcessorListH
#define CmdProcessorListH
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
#include <Operation.hpp>
#include <CmdProcessor.hpp>
#include <CmdProcessorFactory.hpp>
#include <map>

//------------------------------------------------------------------------------

//! List of installed command processors
/*! \ingroup server
 */

class NETLIB_PACKAGE CCmdProcessorList {
public:
    CCmdProcessorList(void);
    ~CCmdProcessorList(void);

// manipulation methods -------------------------------------------------------

    //! register operation and associated command processor factory
    void RegisterProcessor(const COperation& operation,CCmdProcessorFactory* p_factory);

    //! get operation
    const COperation GetOperation(const CSmallString& sop);

    //! get processor associated with command operation
    CCmdProcessor* CreateProcessor(CServerCommand* p_command);

    //! clear operation/factory map
    void ClearFactoryMapping(void);

// section of command processors ----------------------------------------------
private:
    std::map<COperation,CCmdProcessorFactory*>   Map;
};

//---------------------------------------------------------------------------

extern NETLIB_PACKAGE CCmdProcessorList CmdProcessorList;

//---------------------------------------------------------------------------
#endif
