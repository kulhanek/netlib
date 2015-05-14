# ==============================================================================
# NETLib CMake File
# ==============================================================================

#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(lib/netlib SYSTEM)
INCLUDE_DIRECTORIES(lib/netlib/common SYSTEM)
INCLUDE_DIRECTORIES(lib/netlib/client SYSTEM)
INCLUDE_DIRECTORIES(lib/netlib/server SYSTEM)
INCLUDE_DIRECTORIES(lib/netlib/extra/client SYSTEM)
INCLUDE_DIRECTORIES(lib/netlib/extra/server SYSTEM)

# include subdirectories -------------------------------------------------------
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(test)
