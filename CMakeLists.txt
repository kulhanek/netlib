# ==============================================================================
# NETLib CMake File
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

PROJECT(NETLib)

# ==============================================================================
# conditional setup ------------------------------------------------------------
# ==============================================================================

SET(LIBS_SHARED     ON  CACHE BOOL "Should the shared version of cats library be built?")
SET(LIBS_STATIC     OFF CACHE BOOL "Should the static version of cats library be built?")
SET(TRY_QT_LIB      ON CACHE BOOL "Should the qt lib be used?")

# ==============================================================================
# project setup ----------------------------------------------------------------
# ==============================================================================

# project version --------------------------------
SET(NETLIB_VERSION 1.0.x)

# what languages we use --------------------------
ENABLE_LANGUAGE(CXX)

# ==============================================================================
# external dependencies --------------------------------------------------------
# ==============================================================================

IF(NOT DEFINED COMPOSITE_PROJECT)
    SET(DEVELOPMENT_ROOT "$ENV{DEVELOPMENT_ROOT}")
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

IF(NOT DEVELOPMENT_ROOT)
    MESSAGE(FATAL_ERROR "DEVELOPMENT_ROOT variable is not set!")
ENDIF(NOT DEVELOPMENT_ROOT)

# ==============================================================================
# external libraries -----------------------------------------------------------
# ==============================================================================

# HIPOLY ====================
SET(HIPOLY_ROOT ${DEVELOPMENT_ROOT}/projects/hipoly/1.0)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/compress SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/error SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/event SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/file SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/list SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/option SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/string SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/stream SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/thread SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/time SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/xml SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/color SYSTEM)
LINK_DIRECTORIES(${HIPOLY_ROOT}/lib)
SET(HIPOLY_LIB_NAME hipoly)

# PRMFile ===================
SET(PRMFILE_ROOT ${DEVELOPMENT_ROOT}/projects/prmfile/1.0)
INCLUDE_DIRECTORIES(${PRMFILE_ROOT}/src/lib/cprmfile SYSTEM)
LINK_DIRECTORIES(${PRMFILE_ROOT}/lib)
SET(PRMFILE_CLIB_NAME cprmfile)


IF(DEFINED WIN32)
	SET(SYSTEM_LIBS
	            ${PRMFILE_CLIB_NAME}
	            ${HIPOLY_LIB_NAME}
	            ws2_32
	            )
ELSE()
	SET(SYSTEM_LIBS
	            ${PRMFILE_CLIB_NAME}
	            ${HIPOLY_LIB_NAME}
	            )
ENDIF()	            

# ==============================================================================
# output directories -----------------------------------------------------------
# ==============================================================================

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)

# ==============================================================================
# compiler setups --------------------------------------------------------------
# ==============================================================================

IF(NOT DEFINED COMPOSITE_PROJECT)
    # release setup
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")

    # debug setup
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Werror -pedantic-errors -Wundef -Wno-long-long")

    SET(CMAKE_BUILD_TYPE Release)
    #SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

# ==============================================================================
# project subdirectories  ------------------------------------------------------
# ==============================================================================

ADD_SUBDIRECTORY(src)
