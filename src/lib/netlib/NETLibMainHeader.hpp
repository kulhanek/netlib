#ifndef NETLibMainHeaderH
#define NETLibMainHeaderH
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

#include <HiPoLyMainHeader.hpp>

#define NETLIB_VERSION "1.0.SVNVERSION (DATE) [Client/server communication library]"

//------------------------------------------------------------------------------

/*! \defgroup NetLib
@{
    \defgroup common Common
    \defgroup client Client
    \defgroup server Server
    \defgroup extra Extra
    @{
        \defgroup eclient Extra client
        \defgroup eserver Extra server
    @}
@}
*/

/*! \defgroup Test
@{
    \defgroup tclient Test client
    \defgroup tserver Test server
@}
*/

//------------------------------------------------------------------------------

extern const char* LibBuildVersion_NETLIB;

//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#ifdef NETLIB_BUILDING_DLL
#ifdef __GNUC__
#define NETLIB_DLL_PUBLIC __attribute__((dllexport))
#else
#define NETLIB_DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define NETLIB_DLL_PUBLIC __attribute__((dllimport))
#else
#define NETLIB_DLL_PUBLIC __declspec(dllimport)
#endif
#define NETLIB_DLL_LOCAL
#endif
#else
#if __GNUC__ >= 4
#define NETLIB_DLL_PUBLIC __attribute__ ((visibility("default")))
#define NETLIB_DLL_LOCAL  __attribute__ ((visibility("hidden")))
#else
#define NETLIB_DLL_PUBLIC
#define NETLIB_DLL_LOCAL
#endif
#endif

#define NETLIB_PACKAGE NETLIB_DLL_PUBLIC

//------------------------------------------------------------------------------

#endif
