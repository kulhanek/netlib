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

#include <RegClientList.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRegClientList::CRegClientList(void)
{
    // client ID will start from 1
    // 0 is reserved in MTD for initial data (restart level in restart file)
    CRegClientList::IndexPool.SetIndex(1);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRegClientList::AddClient(CRegClient* p_rcl)
{
    if( p_rcl == NULL ) {
        INVALID_ARGUMENT("p_rcl is NULL");
    }

    AccessMutex.Lock();

        try {
            // get client ID
            int id = IndexPool.GetIndex();

            // set index
            p_rcl->ClientID = id;
            p_rcl->ClientStatus = ERCS_NEW;

            // insert to the list
            InsertToEnd(p_rcl,id,true);
        } catch(...) {
            AccessMutex.Unlock();
            throw;
        }

    AccessMutex.Unlock();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRegClientList::RegisterClient(CRegClient* p_rcl)
{
    if( p_rcl == NULL ) {
        INVALID_ARGUMENT("p_rcl is NULL");
    }

    AccessMutex.Lock();

    try {
        // get client ID
        int id = IndexPool.GetIndex();

        // set index
        p_rcl->ClientID = id;
        p_rcl->ClientStatus = ERCS_REGISTERED;
        p_rcl->RegistrationTime.GetActualTimeAndDate();

        // insert to the list
        InsertToEnd(p_rcl,id,true);
    } catch(...) {
        AccessMutex.Unlock();
        throw;
    }

    AccessMutex.Unlock();
}

//------------------------------------------------------------------------------

CRegClient* CRegClientList::RegisterClient(void)
{
    AccessMutex.Lock();

    CSimpleIterator<CRegClient>    I(this);
    CRegClient*                    p_cl;

    while( (p_cl = I.Current()) != NULL ) {
        if( p_cl->ClientStatus == ERCS_NEW ) {
            p_cl->ClientStatus = ERCS_REGISTERED;
            p_cl->RegistrationTime.GetActualTimeAndDate();
            AccessMutex.Unlock();
            return(p_cl);
        }
        I++;
    }

    AccessMutex.Unlock();

    LOGIC_ERROR("no client in status NEW");
}

//------------------------------------------------------------------------------

void CRegClientList::UnregisterClient(int id)
{
    AccessMutex.Lock();

    CRegClient* p_rcl = Find(id);
    if( p_rcl == NULL ) {
        AccessMutex.Unlock();
        LOGIC_ERROR("client was not found");
    }

    if( p_rcl->ClientStatus != ERCS_REGISTERED ) {
        AccessMutex.Unlock();
        LOGIC_ERROR("client cannot be unregistered (it is not registered)");
    }

    p_rcl->ClientStatus = ERCS_UNREGISTERED;
    p_rcl->UnregistrationTime.GetActualTimeAndDate();

    AccessMutex.Unlock();
}

//------------------------------------------------------------------------------

CRegClient* CRegClientList::FindClient(int id)
{
    AccessMutex.Lock();
        CRegClient* p_rcl = Find(id);
    AccessMutex.Unlock();

    return(p_rcl);
}

//------------------------------------------------------------------------------

CRegClient* CRegClientList::GetClient(int index)
{
    AccessMutex.Lock();
        CRegClient* p_rcl = Get(index);
    AccessMutex.Unlock();

    return(p_rcl);
}

//------------------------------------------------------------------------------

int CRegClientList::GetNumberOfRegClients(void)
{
    AccessMutex.Lock();

        CSimpleIterator<CRegClient>    I(this);
        CRegClient*                    p_cl;

        int count = 0;
        while( (p_cl = I.Current()) != NULL ) {
            if( p_cl->ClientStatus == ERCS_REGISTERED ) count++;
            I++;
        }

    AccessMutex.Unlock();

    return(count);
}

//------------------------------------------------------------------------------

int CRegClientList::GetNumberOfClients(void)
{
    AccessMutex.Lock();

    int count = NumOfMembers();

    AccessMutex.Unlock();

    return(count);
}

//------------------------------------------------------------------------------

void CRegClientList::FlushDeadClients(int limit)
{
    AccessMutex.Lock();

        CSimpleIterator<CRegClient>    I(this);
        CRegClient*                    p_cl;

        CSmallTime curr_time;
        curr_time.GetActualTime();

        while( (p_cl = I.Current()) != NULL ) {
            if( p_cl->ClientStatus == ERCS_REGISTERED ) {
                if( p_cl->NumberOfTransaction == 0 ) {
                    if( curr_time - p_cl->RegistrationTime >  limit ) {
                        p_cl->ClientStatus = ERCS_DEAD;
                        p_cl->UnregistrationTime.GetActualTimeAndDate();
                    }
                } else {
                    if( curr_time - p_cl->LastOpTime >  limit ) {
                        p_cl->ClientStatus = ERCS_DEAD;
                        p_cl->UnregistrationTime.GetActualTimeAndDate();
                    }
                }
            }
            I++;
        }

    AccessMutex.Unlock();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRegClientList::SaveInfo(CXMLElement* p_tele)
{
    if( p_tele == NULL ) {
        INVALID_ARGUMENT("p_tele is NULL");
    }

    CXMLElement* p_cele = p_tele->CreateChildElement("CLIENTS");

    AccessMutex.Lock();

        CSimpleIterator<CRegClient>    I(this);
        CRegClient*                    p_cl;

        while( (p_cl = I.Current()) != NULL ) {
            try{
                CXMLElement* p_iele = p_cele->CreateChildElement("CLIENT");
                p_cl->SaveInfo(p_iele);
            } catch(...){
                AccessMutex.Unlock();
                throw;
            }

            I++;
        }

    AccessMutex.Unlock();
}

//------------------------------------------------------------------------------

int CRegClientList::GetNumberOfActiveRegistration(void)
{
    int active = 0;

    AccessMutex.Lock();

    CSimpleIterator<CRegClient>    I(this);
    CRegClient*                    p_cl;

    while( (p_cl = I.Current()) != NULL ) {
        if( p_cl->GetClientStatus() == ERCS_REGISTERED ) active++;
        I++;
    }

    AccessMutex.Unlock();

    return(active);
}

//------------------------------------------------------------------------------

void CRegClientList::PrintInfo(void)
{
    CTerminalStr tout;
    PrintInfo(tout);
}

//------------------------------------------------------------------------------

void CRegClientList::PrintInfo(std::ostream& sout)
{
    // write long summary
    sout << endl;
    sout << "=== Clients ====================================================================" << endl;
    sout << endl;
    sout << " ID  ST    Registered since       Last operation      Unregistered since    OPs " << endl;
    sout << "---- -- --------------------- --------------------- --------------------- ------" << endl;

    AccessMutex.Lock();

    try {
        CSimpleIterator<CRegClient>    I(this);
        CRegClient*                    p_cl;

        while( (p_cl = I.Current()) != NULL ) {
            p_cl->PrintInfo(sout);
            I++;
        }

    } catch(...) {
        // to be sure that mutex will be unlocked
    }

    AccessMutex.Unlock();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


