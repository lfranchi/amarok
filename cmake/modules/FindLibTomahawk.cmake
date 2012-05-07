# - Try to find the libtomahawk library
# Once done this will define
#
#  TOMAHAWK_FOUND - system has libtomahawk
#  TOMAHAWK_INCLUDE_DIRS - the libtomahawk include directory
#  TOMAHAWK_LIBRARY, the path to libtomahawk

set( TOMAHAWK_INCLUDE_DIRS
    /home/llg/Programacao/tomahawk/src/
    /home/llg/Programacao/tomahawk/src/libtomahawk/
    /home/llg/Programacao/tomahawk/src/libtomahawk/playlist/
    /home/llg/Programacao/tomahawk/src/libtomahawk/playlist/dynamic/
    /home/llg/Programacao/tomahawk/src/libtomahawk/playlist/dynamic/topbar/
    /home/llg/Programacao/tomahawk/src/libtomahawk/playlist/dynamic/database/
    /home/llg/Programacao/tomahawk/src/libtomahawk/sip/
    /home/llg/Programacao/tomahawk/src/libtomahawk/utils/
    /home/llg/Programacao/tomahawk/src/libtomahawk/network/
    /home/llg/Programacao/tomahawk/src/libtomahawk/accounts/
    /home/llg/Programacao/tomahawk/src/libtomahawk/database/
    /home/llg/Programacao/tomahawk/src/accounts/
    /home/llg/Programacao/tomahawk/src/accounts/jabber/
    /home/llg/Programacao/tomahawk/src/accounts/zeroconf/
    /home/llg/Programacao/tomahawk/src/accounts/twitter/
    /home/llg/Programacao/tomahawk/build/src/
    /home/llg/Programacao/tomahawk/build/
)

find_library( TOMAHAWK_LIBRARY NAMES tomahawklib
    PATHS
    /usr/lib/
    /home/llg/Programacao/tomahawk/build/
)

if(TOMAHAWK_INCLUDE_DIRS AND TOMAHAWK_LIBRARY)
   set(TOMAHAWK_FOUND TRUE)
   message(STATUS "Found libtomahawk: ${TOMAHAWK_INCLUDE_DIRS}, ${TOMAHAWK_LIBRARY}")
else(TOMAHAWK_INCLUDE_DIRS AND TOMAHAWK_LIBRARY)
   set(TOMAHAWK_FOUND FALSE)   
   if (TOMAHAWK_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find required package libtomahawk")
   endif(TOMAHAWK_FIND_REQUIRED)
endif(TOMAHAWK_INCLUDE_DIRS AND TOMAHAWK_LIBRARY)

mark_as_advanced(TOMAHAWK_INCLUDE_DIRS TOMAHAWK_LIBRARY)