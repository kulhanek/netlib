#ifndef ServerH
#define ServerH
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
#include <SimpleMutex.hpp>
#include <SimpleList.hpp>
#include <ThreadPool.hpp>
#include <Network.hpp>

// -----------------------------------------------------------------------------

//! Server core class
/*! \ingroup server
    This class implements single operation (command) per connection approach.
*/

class NETLIB_PACKAGE CServer {
public:
    CServer(void);
    virtual ~CServer(void);

// start / stop server methods ------------------------------------------------
    //! init server - select port automatically
    bool InitServer(bool gen_password = true);

    //! init server - listen on port
    bool InitServer(int port);

    //! run in loop and accept requests
    void AcceptServerRequests(void);

    //! terminate main lopp
    void TerminateServer(void);

    //! return number of processed commands
    int GetNumberOfTransactions(void);

    //! return number of illegal commands
    int GetNumberOfIllegalTransactions(void);

    //! set server password
    void SetPassword(const CSmallString& passwd);

    //! get server password
    const CSmallString& GetPassword(void);

    //! set protocol name
    void SetProtocolName(const CSmallString& protocol);

    //! get protocol name
    const CSmallString& GetProtocolName(void) const;

    //! set max length of listening queue - it has impact only before server is started
    bool SetMaxListenQueueLen(int maxcons);

    //! get max length of listening queue
    int GetMaxListenQueueLen(void);

    //! set max number of worker threads - it has impact only before server is started
    bool SetMaxNumOfWorkers(int maxthrds);

    //! get max number of worker threads
    int GetMaxNumOfWorkers(void);

    //! get server port
    int GetPort(void);

    //! increment number of illegal transactions - atomically
    void IncrementIllegalTransaction(void);

    //! write server key description
    bool WriteServerKey(const CSmallString& server_key_name);

// private data section -------------------------------------------------------
private:
    CSmallString                ProtocolName;           // protocol name
    int                         ServerPort;             // network port of server
    int                         MaxListenQueueLen;      // maximum length of listening queue
    CSmallString                Password;               // server magic word
    CSocket                     ListenSocket;           // listenning socket
    CThreadPool                 ThreadPool;             // thread pool
    bool                        Terminated;
    int                         MinPortNumber;          // minimum port number - for auto port selection
    int                         MaxPortNumber;          // maximum port number    

    CSimpleMutex                AccessMutex;

    // this varibale is safe since it is updated be this thread
    int                         NumOfTransactions;

    // this is updated by CCommand and is protected by Mutex
    int                         NumOfIllegalTransactions;

    // get server host name
    const CSmallString GetServerHostName(void);
};

//---------------------------------------------------------------------------
#endif
