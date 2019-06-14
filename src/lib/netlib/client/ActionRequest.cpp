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
// ==============================================================================

#include <ActionRequest.hpp>
#include <Network.hpp>
#include <ErrorSystem.hpp>
#include <errno.h>
#include <string.h>
#include <PrmFile.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

#define MAX_NET_NAME    1024

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CActionRequest::CActionRequest(void)
{
    Name = "localhost";
    IP = "127.0.0.1";
    Port = INVALID_PORT;
    Protocol = "netlib";
    ServerKey = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CSmallString CActionRequest::GetQualifiedName(void) const
{
    CSmallString ret_str;
    if( (IP == NULL) && (Name == NULL) ) return(ret_str);

    ret_str = Protocol + "://" + Name + ":" + CSmallString(Port);
    if( Action != NULL ) ret_str += "/" + Action;

    if( Parameters.size() > 0 ) ret_str += "?";

    for(size_t i = 0; i < Parameters.size(); i++){
        ret_str += Parameters[i].Key;
        ret_str += "=";
        ret_str += Parameters[i].Value;
        if( i+1 != Parameters.size() ) ret_str += ",";
    }

    return(ret_str);
}

//------------------------------------------------------------------------------

const CSmallString& CActionRequest::GetIP(void) const
{
    return(IP);
}

//------------------------------------------------------------------------------

int CActionRequest::GetPort(void) const
{
    return(Port);
}

//------------------------------------------------------------------------------

const CSmallString& CActionRequest::GetName(void) const
{
    return(Name);
}

//------------------------------------------------------------------------------

const CSmallString& CActionRequest::GetAction(void) const
{
    return(Action);
}

//------------------------------------------------------------------------------

const CSmallString& CActionRequest::GetPassword(void) const
{
    return(Password);
}

//------------------------------------------------------------------------------

const CSmallString& CActionRequest::GetProtocolName(void) const
{
    return(Protocol);
}

//------------------------------------------------------------------------------

bool CActionRequest::IsServerKey(void) const
{
    return(ServerKey);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CActionRequest::SetProtocolName(const CSmallString& protocol)
{
    Protocol = protocol;
}

//------------------------------------------------------------------------------

void CActionRequest::SetQualifiedName(const CSmallString& name)
{
    ServerKey = false;

    // string is empty
    if( name == NULL ) {
        INVALID_ARGUMENT("qualified name is NULL");
    }

    int len = name.GetLength();

    // find first :
    int first = name.IndexOf(':');
    if( (first == -1) ||
            (first + 2 >= len) ||
            (name[first+1] != '/') ||
            (name[first+2] != '/') ) {
        CSmallString error;
        error << Protocol << ":// protocol is not specified";
        LOGIC_ERROR(error);
    }

    if( Protocol != name.GetSubStringFromTo(0,first-1) ) {
        CSmallString error;
        error << "protocol mismatch, required: '" << Protocol << "' specified: '" << name.GetSubStringFromTo(0,first-1) << "'";
        LOGIC_ERROR(error);
    }

    if( first + 3 >= len ) {
        LOGIC_ERROR("server name is missing");
    }

    // now extract server name ---------------------
    int lname_beg = first+3;
    int lname_end = name.Scan(":/",lname_beg);
    lname_end--;   // remove : or /
    if( lname_end < 0 ) {
        lname_end = len - 1;
    }

    if( lname_end < lname_beg ) {
        LOGIC_ERROR("server name is empty");
    }

    // resolve name and IP --------------------------------------------------------

    CSmallString addr = name.GetSubStringFromTo(lname_beg,lname_end);

    if( (addr == "serverkey") || (addr == "key") ) {
        ServerKey = true;
    } else {
        GetServerNameAndIP(addr);
    }

    if( lname_end + 1 >= len ) return; // nothing more to encode

    // is there a port specified? ------------------
    int lport_beg = name.Scan(":/",lname_end+1);
    int lport_end = lport_beg - 1;
    if( (lport_beg >= 0) && (name[lport_beg] == ':') ) {
        lport_beg++;
        if( lport_beg >= len ) {
            LOGIC_ERROR("delimiter : was specified but port number is missing");
        }
        // extract protocol
        lport_end = name.Scan("/",lport_beg);
        lport_end--;
        if( lport_end < 0 ) {
            lport_end = name.GetLength() - 1;
        }

        if( lport_end < lport_beg ) {
            LOGIC_ERROR("delimiter : was specified but port number is missing");
        }

        CSmallString sport;
        sport = name.GetSubStringFromTo(lport_beg,lport_end);
        if( sport.ToInt(Port) == false) {
            LOGIC_ERROR("specified port '" + sport + "' is not a number");
        }
    }

    if( lport_end + 2 >= len ) return; // nothing more to encode

    CSmallString tmp_string;

    tmp_string = name.GetSubStringFromTo(lport_end + 2,name.GetLength()-1);

    // decode action
    int lparam_beg;

    lparam_beg = tmp_string.Scan("?");

    if( lparam_beg < 0 ) {
        Action = tmp_string;    // only action
        return;
    }

    if( lparam_beg == 0 ) {
        LOGIC_ERROR("action is not specified");
    }

    Action = tmp_string.GetSubStringFromTo(0,lparam_beg-1);

    do {
        len = tmp_string.GetLength();
        if( lparam_beg+1 > len-1) {
            LOGIC_ERROR("delimiter ? or , was specified but key and value are missing");
        }
        tmp_string = tmp_string.GetSubStringFromTo(lparam_beg+1,tmp_string.GetLength()-1);

        CSmallString key,value;

        int key_end;
        int value_end;

        key_end = tmp_string.Scan("=");

        if( key_end < 0 ) {
            LOGIC_ERROR("key is not provided (delimiter = is missing)");
        }

        value_end = tmp_string.Scan(",");

        if( value_end < 0 ) {
            value_end = tmp_string.GetLength();
        }

        if( key_end == 0 ) {
            LOGIC_ERROR("key is not specified");
        }

        key = tmp_string.GetSubStringFromTo(0,key_end-1);
        if( value_end-1 >= key_end+1 ) {
            value = tmp_string.GetSubStringFromTo(key_end+1,value_end-1);
        }

        CActionParameter param;

        param.Key = key;
        param.Value = value;
        Parameters.push_back(param);

        lparam_beg = tmp_string.Scan(",");

    } while(lparam_beg >= 0);

}

//------------------------------------------------------------------------------

void CActionRequest::SetNameOrIP(const CSmallString& name)
{
    Name = name;
}

//------------------------------------------------------------------------------

void CActionRequest::SetPort(int port)
{
    Port = port;
}

//------------------------------------------------------------------------------

void CActionRequest::SetPassword(const CSmallString& password)
{
    Password = password;
}

//------------------------------------------------------------------------------

void CActionRequest::ReadServerKey(const CSmallString& name)
{
    CPrmFile ctrl_file;

    // process control file ----------------------------------
    if( ctrl_file.Read(name) == false ) {
        CSmallString error;
        error << "unable to open server key file (" << strerror(errno) << ")";
        RUNTIME_ERROR(error);
    }

    if( ctrl_file.OpenSection("server-key") == false ) {
        CSmallString error;
        error << "unable to open [server-key] section";
        LOGIC_ERROR(error);
    }

    // all items are optional
    ctrl_file.GetStringByKey("name",Name);
    GetServerNameAndIP(Name);

    ctrl_file.GetIntegerByKey("port",Port);
    ctrl_file.GetStringByKey("password",Password);

    if( ctrl_file.CountULines() > 0 ) {
        CSmallString error;
        error << "unprocessed items in server key file";
        LOGIC_ERROR(error);
    }
}

//------------------------------------------------------------------------------

void CActionRequest::GetServerNameAndIP(const CSmallString& addr)
{
    addrinfo       *p_addrinfo;
    int            nerr;
    // get hostname and IP
    if( (nerr = getaddrinfo(addr,NULL,NULL,&p_addrinfo)) != 0 ) {
        CSmallString error;
        error << "unable to decode server name (" <<  gai_strerror(nerr) << ")";
        RUNTIME_ERROR(error);
    }

    // get server name
    char s_name[MAX_NET_NAME];
    memset(s_name,0,MAX_NET_NAME);

    if( (nerr = getnameinfo(p_addrinfo->ai_addr,p_addrinfo->ai_addrlen,s_name,MAX_NET_NAME-1,NULL,0,0)) != 0 ) {
        CSmallString error;
        error << "unable to get server name (" <<  gai_strerror(nerr) << ")";
        freeaddrinfo(p_addrinfo);
        RUNTIME_ERROR(error);
    }
    Name = s_name;

    // get server IP
    if( (nerr = getnameinfo(p_addrinfo->ai_addr,p_addrinfo->ai_addrlen,s_name,MAX_NET_NAME-1,NULL,0,NI_NUMERICHOST)) != 0 ) {
        CSmallString error;
        error << "unable to get server IP (" <<  gai_strerror(nerr) << ")";
        freeaddrinfo(p_addrinfo);
        RUNTIME_ERROR(error);
    }
    IP = s_name;

    freeaddrinfo(p_addrinfo);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CActionRequest::GetNumberOfParameters(void) const
{
    return(Parameters.size());
}

//------------------------------------------------------------------------------

bool CActionRequest::GetParameterKeyValue(const CSmallString& key,CSmallString& value)
{
    for( size_t i = 0; i < Parameters.size(); i++ ) {
        if( Parameters[i].Key == key ) {
            value = Parameters[i].Value;
            return(true);
        }
    }
    value = NULL;
    return(false);
}

//------------------------------------------------------------------------------

bool CActionRequest::GetParameterKeyValue(const CSmallString& key,int& value)
{
    CSmallString svalue;
    if( GetParameterKeyValue(key,svalue) == false ){
        CSmallString error;
        error << "unable get value of key '" << key << "'";
        ES_TRACE_ERROR(error);
        return(false);
    }

    if( svalue.IsInt() == false ){
        CSmallString error;
        error << "value '" << svalue << "' is not an integer number";
        ES_ERROR(error);
        return(false);
    }

    value = svalue.ToInt();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


