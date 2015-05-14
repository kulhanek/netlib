# ==============================================================================
# Description: try to find pthread library
#
# Input: (optional)
#    PTHREAD_INCLUDE_PATH
#    PTHREAD_LIBRARY_PATH
#
# Output:
#    PTHREAD_FOUND
#    PTHREAD_INCLUDE_DIR
#    PTHREAD_LIBRARY_NAME
#    HAVE_PTHREAD
#
#############################################################################

# find pthread library -----------
FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
    /usr/include
    $ENV{PTHREAD_INCLUDE_PATH}
    )

IF(NOT DEFINED PTHREAD_FOUND)
    IF(PTHREAD_INCLUDE_DIR)
      MESSAGE(STATUS "Looking for PThread header file pthread.h ... found.")
    ELSE(PTHREAD_INCLUDE_DIR)
      MESSAGE(STATUS "Looking for PThread header file pthread.h ... not found.")
    ENDIF(PTHREAD_INCLUDE_DIR)
ENDIF(NOT DEFINED PTHREAD_FOUND)

FIND_LIBRARY(PTHREAD_LIBRARY_NAME
    NAMES pthread
    PATHS
    /lib64
    /lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    $ENV{PTHREAD_LIBRARY_PATH}
    )

IF(NOT DEFINED PTHREAD_FOUND)
    IF(PTHREAD_LIBRARY_NAME)
      MESSAGE(STATUS "Looking for PThread library file pthread ... found.")
    ELSE(PTHREAD_LIBRARY_NAME)
      MESSAGE(STATUS "Looking for PThread library file pthread ... not found.")
      SET(PTHREAD_LIBRARY_NAME "")
    ENDIF(PTHREAD_LIBRARY_NAME)
ENDIF(NOT DEFINED PTHREAD_FOUND)

# ------------------------------------------------------------------------------

IF(PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY_NAME)
    SET(PTHREAD_FOUND TRUE CACHE "PThread Lib" INTERNAL)
    SET(HAVE_PTHREAD 1)
    ADD_DEFINITIONS(-DHAVE_PTHREAD)
ELSE(PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY_NAME)
    SET(PTHREAD_FOUND FALSE CACHE "PThread Lib" INTERNAL)
ENDIF(PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY_NAME)

