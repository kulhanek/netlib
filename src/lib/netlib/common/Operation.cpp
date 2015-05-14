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

#include <Operation.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

DEFINE_OPERATION(Operation_IllegalOperation,
                 "{ILLEGAL_OPERATION:92c162d9-433b-4f8a-9b66-ef8f39741d89}");

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

COperation::COperation(const CSmallString& sextuuid)
{
    int namelength=0;

    for(unsigned int i=1; i < sextuuid.GetLength(); i++) {
        if( sextuuid.GetBuffer()[i] == ':'  ) {
            namelength = i-1;
            break;
        }
    }

    // set name -------------------------------------
    if(namelength == 0) return;
    if( ClassName.SetLength(namelength) == false) return;
    ClassName = sextuuid.GetSubString(1,namelength);

    // set UUID
    SetFromStringForm(sextuuid.GetSubString(namelength+2,sextuuid.GetLength()-3-namelength));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CSmallString  COperation::GetStringForm(void) const
{
    CSmallString string;
    string = "{" + ClassName + ":" + CUUID::GetStringForm() + "}";
    return(string);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


