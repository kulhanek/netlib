# ==============================================================================
# Description: try to find uuid library
#
# Input: (optional)
#    UUID_INCLUDE_PATH
#    UUID_LIBRARY_PATH
#
# Output:
#    UUID_FOUND
#    UUID_INCLUDE_DIR
#    UUID_LIBRARY_NAME
#    HAVE_UUID
#
#############################################################################

# find uuid library ------------------
FIND_PATH(UUID_INCLUDE_DIR uuid/uuid.h
    /usr/include
    $ENV{UUID_INCLUDE_PATH}
    )

IF(NOT DEFINED UUID_FOUND)
    IF(UUID_INCLUDE_DIR)
      MESSAGE(STATUS "Looking for UUID header file uuid.h ... found.")
    ELSE(UUID_INCLUDE_DIR)
      MESSAGE(STATUS "Looking for UUID header file uuid.h ... not found.")
    ENDIF(UUID_INCLUDE_DIR)
ENDIF(NOT DEFINED UUID_FOUND)

FIND_LIBRARY(UUID_LIBRARY_NAME
    NAMES uuid
    PATHS
    /lib64
    /lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    $ENV{UUID_LIBRARY_PATH}
    )

IF(NOT DEFINED UUID_FOUND)
    IF(UUID_LIBRARY_NAME)
      MESSAGE(STATUS "Looking for UUID library file uuid ... found.")
    ELSE(UUID_LIBRARY_NAME)
      MESSAGE(STATUS "Looking for UUID library file uuid ... not found.")
      SET(UUID_LIBRARY_NAME "")
    ENDIF(UUID_LIBRARY_NAME)
ENDIF(NOT DEFINED UUID_FOUND)

# ------------------------------------------------------------------------------

IF(UUID_INCLUDE_DIR AND UUID_LIBRARY_NAME)
    SET(UUID_FOUND TRUE CACHE INTERNAL "UUID Lib" FORCE)
    SET(HAVE_UUID 1)
    ADD_DEFINITIONS(-DHAVE_UUID)
ELSE(UUID_INCLUDE_DIR AND UUID_LIBRARY_NAME)
    SET(UUID_FOUND FALSE CACHE INTERNAL "UUID Lib" FORCE)
ENDIF(UUID_INCLUDE_DIR AND UUID_LIBRARY_NAME)

# ------------------------------------------------------------------------------


