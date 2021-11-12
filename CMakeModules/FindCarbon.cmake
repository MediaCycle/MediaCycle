# Locate Carbon
# This module defines
# CARBON_LIBRARY
# CARBON_FOUND, if false, do not try to link to gdal 
# CARBON_INCLUDE_DIR, where to find the headers
#
# $CARBON_DIR is an environment variable that would
# correspond to the ./configure --prefix=$CARBON_DIR
#
# Created by Eric Wing. 

# Carbon on OS X looks different than Carbon for Windows,
# so I am going to case the two.

IF(APPLE)
  FIND_PATH(CARBON_INCLUDE_DIR Carbon/Carbon.h)
  FIND_LIBRARY(CARBON_LIBRARY Carbon)
ELSE()
  FIND_PATH(CARBON_INCLUDE_DIR Carbon.h
    $ENV{CARBON_DIR}/include
    $ENV{CARBON_DIR}
    NO_DEFAULT_PATH
  )
  FIND_PATH(CARBON_INCLUDE_DIR Carbon.h
    PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
    NO_DEFAULT_PATH
    PATH_SUFFIXES include
  )
  FIND_PATH(CARBON_INCLUDE_DIR Carbon.h)

  FIND_LIBRARY(CARBON_LIBRARY Carbon
    $ENV{CARBON_DIR}/lib
    $ENV{CARBON_DIR}
    NO_DEFAULT_PATH
  )
  FIND_LIBRARY(CARBON_LIBRARY Carbon
    PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
    NO_DEFAULT_PATH
    PATH_SUFFIXES lib64 lib
  )
  FIND_LIBRARY(CARBON_LIBRARY Carbon)
ENDIF()


SET(CARBON_FOUND "NO")
IF(CARBON_LIBRARY AND CARBON_INCLUDE_DIR)
  SET(CARBON_FOUND "YES")
ENDIF()

IF(OSG_BUILD_PLATFORM_IPHONE OR OSG_BUILD_PLATFORM_IPHONE_SIMULATOR)
    SET(CARBON_FOUND "NO")
ENDIF()

IF(APPLE)
    #Quicktime is not supported under 64bit OSX build so we need to detect it and disable it.
    #First check to see if we are running with a native 64-bit compiler (10.6 default) and implicit arch
    IF(NOT CMAKE_OSX_ARCHITECTURES AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(CARBON_FOUND "NO")
    ELSE()
        #Otherwise check to see if 64-bit is explicitly called for.
        LIST(FIND CMAKE_OSX_ARCHITECTURES "x86_64" has64Compile)
        IF(NOT has64Compile EQUAL -1)
            SET(CARBON_FOUND "NO")
        ENDIF()
    ENDIF()
ENDIF()
