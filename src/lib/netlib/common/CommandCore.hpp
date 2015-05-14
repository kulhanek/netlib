#ifndef CommandCoreH
#define CommandCoreH
// =============================================================================
// NETLib - Core Services for Client/server Communication over Network
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <XMLDocument.hpp>
#include <SmartThread.hpp>
#include <Operation.hpp>
#include <IndexCounter.hpp>
#include <Network.hpp>
#include <CmdHeader.hpp>

//------------------------------------------------------------------------------

class CClient;
class CClientContext;

//---------------------------------------------------------------------------

//! Command processing status
/*! \ingroup common
*/
enum ECommandStatus {
    ECS_ABORTED     = -1,
    ECS_NEW         = 1,
    ECS_PREPARING   = 2,
    ECS_WRITING     = 3,
    ECS_PROCESSING  = 4,
    ECS_READING     = 5,
    ECS_FINISHED    = 6
};

//------------------------------------------------------------------------------

//! Command data container
/*! \ingroup common
    It contains both client and server parts.
*/

class NETLIB_PACKAGE CCommandCore {
public:
    CCommandCore(void);
    virtual ~CCommandCore(void);

// common part ----------------------------------------------------------------
    //! associate operation with command data
    void SetOperation(const COperation& operation);

    //! get command operation full name
    const CSmallString  GetSOperation(void);

    //! return current status of commad
    ECommandStatus GetStatus(void);

    //! return root command data element
    /**
      never return NULL!
    */
    CXMLElement* GetRootCommandElement(void);

    //! return command data element specified by path
    /**
      never return NULL if create is true
    */
    CXMLElement* GetCommandElementByPath(const CSmallString& path,bool create);

    //! return root result data element
    /**
      never return NULL!
    */
    CXMLElement* GetRootResultElement(void);

    //! return result data element specified by path
    /**
      never return NULL if create is true
    */
    CXMLElement* GetResultElementByPath(const CSmallString& path,bool create);

    //! return root error element
    /**
      never return NULL!
    */
    CXMLElement* GetRootErrorElement(void);

// section of protected data ----------------------------------------------------
protected:
    static CIndexCounter    CommandIDCounter;
    bool                    DebugMode;
    int                     CommandID;
    ECommandStatus          CommandStatus;
    CXMLDocument            Request;
    CXMLDocument            Result;
    CSocket                 Socket;
    CCmdHeader              Header;
    bool                    Terminated;

    //! read binary data from network stream
    bool ReadData(unsigned char* p_data,unsigned int length);

    //! write binary data to network stream
    bool WriteData(unsigned char* p_data,unsigned int length);
};

//---------------------------------------------------------------------------
#endif

