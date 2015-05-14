#ifndef OperationH
#define OperationH
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
#include <UUID.hpp>

//------------------------------------------------------------------------------

//! operation identificator
/*! \ingroup common
    This is merged and simplified version based on KERBEROS COperation and CExtUUID.
*/

class NETLIB_PACKAGE COperation : public CUUID {
public:
    COperation(const CSmallString& sextuuid);

    //! return string form of ExtUUID
    const CSmallString  GetStringForm(void) const;

// section of private data ----------------------------------------------------
private:
    CSmallString    ClassName;
};

//------------------------------------------------------------------------------

//! declare operation
#define DECLARE_OPERATION(name) extern NETLIB_PACKAGE COperation name

//! define operation
#define DEFINE_OPERATION(name,sextuuid) COperation name(sextuuid)

//------------------------------------------------------------------------------

//! illegal operation
DECLARE_OPERATION(Operation_IllegalOperation);

//------------------------------------------------------------------------------

#endif
