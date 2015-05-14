#ifndef CmdHeaderH
#define CmdHeaderH
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

//------------------------------------------------------------------------------

#define TRACK_ID        "NET_LIB_"
#define TRACK_LENGTH    8
#define MAGIC_LENGTH    16
#define PROTOCOL_LENGTH 16

//------------------------------------------------------------------------------

//! Command header initiating binary transfer
/*! \ingroup common
    This is an internal class.
*/

class NETLIB_PACKAGE CCmdHeader {
public:
    CCmdHeader(void);

// ----------------------------------------------------------------------------
    //! check if header looks nice
    bool CheckHeader(void) const;

    //! check protocol - server side
    bool CheckProtocolName(const CSmallString& protocol) const;

    //! check protocol - client side
    bool WasProtocolNameOK(void) const;

    //! check password - server side
    bool CheckPassword(const CSmallString& passwd) const;

    //! check password - client side
    bool WasPasswordOK(void) const;

// ----------------------------------------------------------------------------
    //! get length of command data
    unsigned int  GetLength(void) const;

    //! get length of attached compressed data
    unsigned int  GetCompressedLength(void) const;

    //! get command ID
    int  GetCommandID(void) const;

// ----------------------------------------------------------------------------
    //! set command ID
    void SetCommandID(int id);

    //! set protocol name
    void SetProtocolName(const CSmallString& protocol);

    //! inform that protocol name was fine
    void SetProtocolNameOK(void);

    //! inform that protocol name was incorrect
    void SetProtocolNameBad(void);

    //! set password
    void SetPassword(const CSmallString& passwd);

    //! inform that password was fine
    void SetPasswordOK(void);

    //! inform that password was incorrect
    void SetPasswordBad(void);


    //! set length of command data
    void SetLength(unsigned int length);

    //! set length of compressed data
    void SetCompressedLength(unsigned int length);

// section of private data ----------------------------------------------------
private:
    char            Track[TRACK_LENGTH];        // track sequence
    unsigned int    CommandLength;              // command length
    unsigned int    CompressedLength;           // compressed command length
    int             CommandID;                  // command ID
    char            Protocol[PROTOCOL_LENGTH];  // protocol name
    char            Magic[MAGIC_LENGTH];        // magic word - eg. password
    unsigned int    ControlSum;                 // control sum

    //! recalculate control sum
    unsigned int CalcControllSum(void) const;
};

//---------------------------------------------------------------------------

#endif
