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

#include <CmdProcessorList.hpp>
#include <ServerCommand.hpp>
#include <ErrorSystem.hpp>

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

CCmdProcessorList CmdProcessorList;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCmdProcessorList::CCmdProcessorList(void)
{

}

//------------------------------------------------------------------------------

CCmdProcessorList::~CCmdProcessorList(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCmdProcessorList::RegisterProcessor(const COperation& operation,CCmdProcessorFactory* p_factory)
{
    Map[operation] = p_factory;
}

//------------------------------------------------------------------------------

const COperation CCmdProcessorList::GetOperation(const CSmallString& sop)
{
    COperation                                      mop(sop);
    map<COperation,CCmdProcessorFactory*>::iterator it = Map.begin();
    map<COperation,CCmdProcessorFactory*>::iterator ie = Map.end();

    while( it != ie ){
        if( (*it).first == mop ) return((*it).first);
        it++;
    }

    return(Operation_IllegalOperation);
}

//------------------------------------------------------------------------------

CCmdProcessor* CCmdProcessorList::CreateProcessor(CServerCommand* p_command)
{
    CCmdProcessorFactory*   p_fac = NULL;
    CCmdProcessor*          p_proc = NULL;

    p_fac = Map[p_command->GetOperation()];
    p_proc = p_fac->CreateProcessor(p_command);

    return(p_proc);
}

//------------------------------------------------------------------------------

void CCmdProcessorList::ClearFactoryMapping(void)
{
    Map.clear();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



