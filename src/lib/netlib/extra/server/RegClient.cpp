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

#include <RegClient.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <iomanip>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRegClient::CRegClient(void)
{
    ClientID = 0;
    NumberOfTransaction = 0;
    ClientStatus = ERCS_NEW;
    Command = NULL;
}

//------------------------------------------------------------------------------

CRegClient::CRegClient(const CSmallString& client_name,
                       const CSmallString& job_id)
{
    ClientID = 0;
    NumberOfTransaction = 0;
    ClientStatus = ERCS_NEW;
    ClientName = client_name;
    InfJobID = job_id;
    Command = NULL;
}

//------------------------------------------------------------------------------

CRegClient::~CRegClient(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CRegClient::GetClientID(void)
{
    return(ClientID);
}

//------------------------------------------------------------------------------

ERegClientStatus CRegClient::GetClientStatus(void)
{
    return(ClientStatus);
}

//------------------------------------------------------------------------------

void CRegClient::PrintInfo(ostream& vout)
{
    CSmallString sreg_time;
    CSmallString sunreg_time;
    CSmallString slast_op_time;
    CSmallString status_str;

    switch(ClientStatus) {
    case ERCS_NEW:
        sreg_time = "                   ";
        sunreg_time = "                   ";
        slast_op_time = "                   ";
        status_str = " N";
        break;
    case ERCS_REGISTERED:
        sreg_time = RegistrationTime.GetSDateAndTime();
        sunreg_time = "                   ";
        if( NumberOfTransaction > 0 ) {
            slast_op_time = LastOpTime.GetSDateAndTime();
        } else {
            slast_op_time = "                   ";
        }
        status_str = " R";
        break;
    case ERCS_UNREGISTERED:
        sreg_time = RegistrationTime.GetSDateAndTime();
        sunreg_time = UnregistrationTime.GetSDateAndTime();
        if( NumberOfTransaction > 0 ) {
            slast_op_time = LastOpTime.GetSDateAndTime();
        } else {
            slast_op_time = "                   ";
        }
        status_str = " U";
        break;
    case ERCS_DEAD:
        sreg_time = RegistrationTime.GetSDateAndTime();
        sunreg_time = UnregistrationTime.GetSDateAndTime();
        if( NumberOfTransaction > 0 ) {
            slast_op_time = LastOpTime.GetSDateAndTime();
        } else {
            slast_op_time = "                   ";
        }
        status_str = " D";
        break;
    case ERCS_TERMINATED:
        sreg_time = RegistrationTime.GetSDateAndTime();
        sunreg_time = UnregistrationTime.GetSDateAndTime();
        if( NumberOfTransaction > 0 ) {
            slast_op_time = LastOpTime.GetSDateAndTime();
        } else {
            slast_op_time = "                   ";
        }
        status_str = " T";
        break;
    default:
        ES_ERROR("unknown client status");
        break;
    }

    // override by extra status if requested
    if( ExtraStatus != NULL ){
        status_str = ExtraStatus;
    }

    // write result
    vout << right << setw(4) << ClientID << " ";
    vout << setw(2) << status_str << " ";
    vout << setw(21) << sreg_time << " ";
    vout << setw(21) << slast_op_time << " ";
    vout << setw(21) << sunreg_time << " ";
    vout << setw(6) << NumberOfTransaction << endl;


    // write result
    if( InfJobID != NULL ) {
        vout << "<blue>      #  " << left << setw(30) << ClientName << "   (";
        vout << InfJobID << ")</blue>" << endl;
    } else {
        vout << "<blue>      #  " << left << setw(30) << ClientName << "</blue>" << endl;
    }
}

//------------------------------------------------------------------------------

void CRegClient::LoadInfo(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    bool result = true;
    int status;

    result &= p_ele->GetAttribute("id",ClientID);
    result &= p_ele->GetAttribute("st",status);

    if( result == false ) {
        LOGIC_ERROR("unable to set attribute(s)");
    }

    ClientStatus = (ERegClientStatus)status;

    switch(ClientStatus) {
        case ERCS_NEW:
            break;
        case ERCS_UNREGISTERED:
        case ERCS_DEAD:
        case ERCS_TERMINATED:
            result &= p_ele->GetAttribute("ut",UnregistrationTime);
        case ERCS_REGISTERED:
            result &= p_ele->GetAttribute("rt",RegistrationTime);
            result &= p_ele->GetAttribute("lo",LastOpTime);
            result &= p_ele->GetAttribute("nt",NumberOfTransaction);
            result &= p_ele->GetAttribute("cn",ClientName);
            result &= p_ele->GetAttribute("ji",InfJobID);
            result &= p_ele->GetAttribute("es",ExtraStatus);
            break;
        default:
            LOGIC_ERROR("unknown client status");
    }

    if( result == false ) {
        LOGIC_ERROR("unable to set attribute(s)");
    }
}

//------------------------------------------------------------------------------

void CRegClient::SaveInfo(CXMLElement* p_ele)
{
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    int  status = (int)ClientStatus;

    p_ele->SetAttribute("id",ClientID);
    p_ele->SetAttribute("st",status);

    switch(ClientStatus) {
    case ERCS_NEW:
        break;
    case ERCS_UNREGISTERED:
    case ERCS_DEAD:
    case ERCS_TERMINATED:
        p_ele->SetAttribute("ut",UnregistrationTime);
    case ERCS_REGISTERED:
        p_ele->SetAttribute("rt",RegistrationTime);
        p_ele->SetAttribute("lo",LastOpTime);
        p_ele->SetAttribute("nt",NumberOfTransaction);
        p_ele->SetAttribute("cn",ClientName);
        p_ele->SetAttribute("ji",InfJobID);
        p_ele->SetAttribute("es",ExtraStatus);
        break;
    default:
        LOGIC_ERROR("unknown client status");
        return;
    }
}

//------------------------------------------------------------------------------

void CRegClient::RegisterOperation(void)
{
    LastOpTime.GetActualTimeAndDate();
    NumberOfTransaction++;
}

//------------------------------------------------------------------------------

void CRegClient::SetCommand(CServerCommand* p_cmd)
{
    Command = p_cmd;
}

//------------------------------------------------------------------------------

CServerCommand* CRegClient::GetCommand(void)
{
    return(Command);
}

//------------------------------------------------------------------------------

void CRegClient::SetExtraStatus(const CSmallString& es)
{
    ExtraStatus = es;
}

//------------------------------------------------------------------------------

const CSmallString& CRegClient::GetExtraStatus(void)
{
    return(ExtraStatus);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


