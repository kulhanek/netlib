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

#include <ExtraOperation.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

DEFINE_OPERATION(Operation_GetServerInfo,
                 "{GET_SERVER_INFO:6edd285e-797e-469e-82a6-5c73a43236b6}");

DEFINE_OPERATION(Operation_ShutdownServer,
                 "{SHUTDOWN_SERVER:a187b807-a2f4-4f56-a5b7-f270c2f8ad8b}");

DEFINE_OPERATION(Operation_GetServerErrors,
                 "{GET_SERVER_ERRORS:75f5fb48-ef77-41d1-b834-a68013685316}");

DEFINE_OPERATION(Operation_RegisterClient,
                 "{REGISTER_CLIENT:d482b8b2-d6fb-4585-8e0b-30654550dd38}");

DEFINE_OPERATION(Operation_UnregisterClient,
                 "{UNREGISTER_CLIENT:2b7dbfb7-9313-41ff-b5ac-a6ddcf1b0769}");

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


