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

#include <CmdHeader.hpp>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCmdHeader::CCmdHeader(void)
{
    memset(Track,0,TRACK_LENGTH);
    strncpy(Track,TRACK_ID,TRACK_LENGTH);

    memset(Magic,0,MAGIC_LENGTH);
    memset(Protocol,0,PROTOCOL_LENGTH);

    CommandLength = 0;
    CompressedLength = 0;
    CommandID = 0;

    ControlSum = CalcControllSum();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCmdHeader::CheckHeader(void) const
{
    unsigned int cs =  CalcControllSum();
    return(cs == ControlSum);
}

//------------------------------------------------------------------------------

bool CCmdHeader::CheckProtocolName(const CSmallString& protocol) const
{
    return( strncmp(Protocol,protocol,PROTOCOL_LENGTH) == 0 );
}

//------------------------------------------------------------------------------

bool CCmdHeader::WasProtocolNameOK(void) const
{
    for(int i=0; i < PROTOCOL_LENGTH; i++) {
        if( Protocol[i] != 'P' ) return(false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CCmdHeader::CheckPassword(const CSmallString& passwd) const
{
    return( strncmp(Magic,passwd,MAGIC_LENGTH) == 0 );
}

//------------------------------------------------------------------------------

bool CCmdHeader::WasPasswordOK(void) const
{
    for(int i=0; i < MAGIC_LENGTH; i++) {
        if( Magic[i] != 'T' ) return(false);
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

unsigned int CCmdHeader::GetLength(void) const
{
    return(CommandLength);
}

//---------------------------------------------------------------------------

unsigned int CCmdHeader::GetCompressedLength(void) const
{
    return(CompressedLength);
}

//---------------------------------------------------------------------------

int CCmdHeader::GetCommandID(void) const
{
    return(CommandID);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCmdHeader::SetCommandID(int id)
{
    CommandID = id;
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetProtocolName(const CSmallString& protocol)
{
    if( protocol == NULL ) {
        memset(Protocol,0,PROTOCOL_LENGTH);
        ControlSum = CalcControllSum();
        return;
    }
    memset(Protocol,0,PROTOCOL_LENGTH);
    strncpy(Protocol,protocol,PROTOCOL_LENGTH);
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetProtocolNameOK(void)
{
    memset(Protocol,'P',PROTOCOL_LENGTH);
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetProtocolNameBad(void)
{
    memset(Protocol,'U',PROTOCOL_LENGTH);
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetPassword(const CSmallString& passwd)
{
    if( passwd == NULL ) {
        memset(Magic,0,MAGIC_LENGTH);
        ControlSum = CalcControllSum();
        return;
    }
    memset(Magic,0,MAGIC_LENGTH);
    strncpy(Magic,passwd,MAGIC_LENGTH);
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetPasswordOK(void)
{
    memset(Magic,'T',MAGIC_LENGTH);
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetPasswordBad(void)
{
    memset(Magic,'F',MAGIC_LENGTH);
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetLength(unsigned int length)
{
    CommandLength = length;
    ControlSum = CalcControllSum();
}

//---------------------------------------------------------------------------

void CCmdHeader::SetCompressedLength(unsigned int length)
{
    CompressedLength = length;
    ControlSum = CalcControllSum();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

unsigned int CCmdHeader::CalcControllSum(void) const
{
    unsigned int cs = 0;
    for(int i=0; i < TRACK_LENGTH; i++) cs += Track[i];
    cs += CommandLength;
    cs += CompressedLength;
    cs += CommandID;
    for(int i=0; i < MAGIC_LENGTH; i++) cs += Magic[i];
    for(int i=0; i < PROTOCOL_LENGTH; i++) cs += Protocol[i];

    return(cs);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
