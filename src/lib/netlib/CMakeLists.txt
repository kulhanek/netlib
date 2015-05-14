# ==============================================================================
# NETLib CMake File
# ==============================================================================

SET(NETLIB_SRC
        NETLibMainHeader.cpp

    # network support ----------------------------
        common/Operation.cpp
        common/Network.cpp
        common/CmdHeader.cpp
        common/CommandCore.cpp

    # core client --------------------------------
        client/ActionRequest.cpp
        client/ClientCommand.cpp
        client/Client.cpp

    # core server --------------------------------
        server/CmdProcessor.cpp
        server/CmdProcessorList.cpp
        server/CmdProcessorFactory.cpp
        server/ServerCommand.cpp
        server/Server.cpp

    # extra client -------------------------------
        extra/client/ExtraOperation.cpp
        extra/client/COpRegisterClient.cpp
        extra/client/COpUnregisterClient.cpp
        extra/client/COpGetServerErrors.cpp
        extra/client/COpGetServerInfo.cpp
        extra/client/COpShutdownServer.cpp
        extra/client/ExtraClient.cpp

    # extra server -------------------------------
        extra/server/ExtraProcessor.cpp
        extra/server/ExtraFactory.cpp
        extra/server/SOpRegisterClient.cpp
        extra/server/SOpUnregisterClient.cpp
        extra/server/SOpGetServerErrors.cpp
        extra/server/SOpGetServerInfo.cpp
        extra/server/SOpShutdownServer.cpp
        extra/server/RegClient.cpp
        extra/server/RegClientList.cpp
        extra/server/ServerWatcher.cpp
        extra/server/ExtraServer.cpp
        )

#-------------------------------------------------------------------------------

# create static library --------------------------------------------------------
IF(LIBS_STATIC)
    ADD_LIBRARY(netlib_static STATIC ${NETLIB_SRC})
    SET_TARGET_PROPERTIES(netlib_static PROPERTIES
                            OUTPUT_NAME netlib
                            CLEAN_DIRECT_OUTPUT 1)
ENDIF(LIBS_STATIC)

# create shared library --------------------------------------------------------
IF(LIBS_SHARED)
    ADD_DEFINITIONS(-DNETLIB_BUILDING_DLL)
    ADD_LIBRARY(netlib_shared SHARED ${NETLIB_SRC})
    TARGET_LINK_LIBRARIES(netlib_shared ${SYSTEM_LIBS})
    SET_TARGET_PROPERTIES(netlib_shared PROPERTIES
                            OUTPUT_NAME netlib
                            CLEAN_DIRECT_OUTPUT 1
                            VERSION ${NETLIB_VERSION})

INSTALL(TARGETS
            netlib_shared
        DESTINATION
            lib
        )
ENDIF(LIBS_SHARED)

#-------------------------------------------------------------------------------
