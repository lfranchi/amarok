# - Try to find the libtomahawk library
# Once done this will define
#
#  TOMAHAWK_FOUND - system has libtomahawk
#  TOMAHAWK_INCLUDE_DIRS - the libtomahawk include directory
#  TOMAHAWK_LIBRARY, the path to libtomahawk

set( TOMAHAWK_SOURCE_DIR /home/domme/dev/sources/tomahawk/)
set( TOMAHAWK_BINARY_DIR /home/domme/dev/build/tomahawk/ ) #For config.h

set( TOMAHAWK_INCLUDE_DIRS
    ${TOMAHAWK_BINARY_DIR}/src/
    ${TOMAHAWK_SOURCE_DIR}/src/libtomahawk/
    ${TOMAHAWK_SOURCE_DIR}/src/ # For HeadlessCheck
)

find_library( TOMAHAWK_LIBRARY NAMES tomahawklib
    PATHS
    /usr/lib/
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