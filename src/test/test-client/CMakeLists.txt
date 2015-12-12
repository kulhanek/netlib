# ==============================================================================
# NETLib - Core Services for Client/server Communication over Network
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TEST_CLIENT_SRC
        main.cpp
        TestClient.cpp
        TestClientOptions.cpp
        COpGetResponse.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(test-client ${TEST_CLIENT_SRC})
ADD_DEPENDENCIES(test-client netlib_shared)

TARGET_LINK_LIBRARIES(test-client
        netlib
        ${PRMFILE_CLIB_NAME}
        ${HIPOLY_LIB_NAME}
        )


