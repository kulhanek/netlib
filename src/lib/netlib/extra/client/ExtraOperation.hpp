#ifndef ExtraOperationH
#define ExtraOperationH
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

//------------------------------------------------------------------------------

//! return status of server
DECLARE_OPERATION(NETLIB_PACKAGE,Operation_GetServerInfo);

//! shutdown server
DECLARE_OPERATION(NETLIB_PACKAGE,Operation_ShutdownServer);

//! get all server errors
DECLARE_OPERATION(NETLIB_PACKAGE,Operation_GetServerErrors);

//! register client
DECLARE_OPERATION(NETLIB_PACKAGE,Operation_RegisterClient);

//! unregister client
DECLARE_OPERATION(NETLIB_PACKAGE,Operation_UnregisterClient);

//------------------------------------------------------------------------------

#endif
