# ==============================================================================
# NETLib - Core Services for Client/server Communication over Network
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TEST_SERVER_SRC
        main.cpp
        TestServer.cpp
        TestServerOptions.cpp
        TestProcessor.cpp
        TestFactory.cpp
        OpGetResponse.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(test-server ${TEST_SERVER_SRC})
ADD_DEPENDENCIES(test-server netlib_shared)

TARGET_LINK_LIBRARIES(test-server
        netlib
        ${PRMFILE_CLIB_NAME}
        ${HIPOLY_LIB_NAME}
        )
