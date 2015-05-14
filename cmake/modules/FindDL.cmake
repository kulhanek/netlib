# ==============================================================================
# Description: try to find dl library
#
# Input: (optional)
#    DL_INCLUDE_PATH
#    DL_LIBRARY_PATH
#
# Output:
#    DL_FOUND
#    DL_INCLUDE_DIR
#    DL_LIBRARY_NAME
#    HAVE_DL
#
#############################################################################

# find dl library -----------------
FIND_PATH(DL_INCLUDE_DIR dlfcn.h
    /usr/include
    $ENV{DL_INCLUDE_PATH}
    )

IF(NOT DEFINED DL_FOUND)
    IF(DL_INCLUDE_DIR)
      MESSAGE(STATUS "Looking for DL header file dlfcn.h ... found.")
    ELSE(DL_INCLUDE_DIR)
      MESSAGE(STATUS "Looking for DL header file dlfcn.h ... not found.")
    ENDIF(DL_INCLUDE_DIR)
ENDIF(NOT DEFINED DL_FOUND)

FIND_LIBRARY(DL_LIBRARY_NAME
    NAMES dl
    PATHS
    /lib64
    /lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    $ENV{DL_LIBRARY_PATH}
    )

IF(NOT DEFINED DL_FOUND)
    IF(DL_LIBRARY_NAME)
      MESSAGE(STATUS "Looking for DL library file dl ... found.")
    ELSE(DL_LIBRARY_NAME)
      MESSAGE(STATUS "Looking for DL library file dl ... not found.")
      SET(DL_LIBRARY_NAME "")
    ENDIF(DL_LIBRARY_NAME)
ENDIF(NOT DEFINED DL_FOUND)

# ------------------------------------------------------------------------------

IF(DL_INCLUDE_DIR AND DL_LIBRARY_NAME)
    SET(DL_FOUND TRUE CACHE "DL Lib" INTERNAL)
    SET(HAVE_DL 1)
    ADD_DEFINITIONS(-DHAVE_DL)
ELSE(DL_INCLUDE_DIR AND DL_LIBRARY_NAME)
    SET(DL_FOUND FALSE CACHE "DL Lib" INTERNAL)
ENDIF(DL_INCLUDE_DIR AND DL_LIBRARY_NAME)

