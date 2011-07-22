# Locate Cocoa
# This module defines
# COCOA_LIBRARY
# COCOA_FOUND, if false, do not try to link to gdal 
# COCOA_INCLUDE_DIR, where to find the headers
#
# $COCOA_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COCOA_DIR
#
# Created by Eric Wing. 

# Cocoa on OS X looks different than Cocoa for Windows,
# so I am going to case the two.

IF(APPLE)
  FIND_PATH(COCOA_INCLUDE_DIR Cocoa/Cocoa.h)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
ELSE()
  FIND_PATH(COCOA_INCLUDE_DIR Cocoa.h
    $ENV{COCOA_DIR}/include
    $ENV{COCOA_DIR}
    NO_DEFAULT_PATH
  )
  FIND_PATH(COCOA_INCLUDE_DIR Cocoa.h
    PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
    NO_DEFAULT_PATH
    PATH_SUFFIXES include
  )
  FIND_PATH(COCOA_INCLUDE_DIR Cocoa.h)

  FIND_LIBRARY(COCOA_LIBRARY Cocoa
    $ENV{COCOA_DIR}/lib
    $ENV{COCOA_DIR}
    NO_DEFAULT_PATH
  )
  FIND_LIBRARY(COCOA_LIBRARY Cocoa
    PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
    NO_DEFAULT_PATH
    PATH_SUFFIXES lib64 lib
  )
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
ENDIF()


SET(COCOA_FOUND "NO")
IF(COCOA_LIBRARY AND COCOA_INCLUDE_DIR)
  SET(COCOA_FOUND "YES")
ENDIF()

IF(OSG_BUILD_PLATFORM_IPHONE OR OSG_BUILD_PLATFORM_IPHONE_SIMULATOR)
    SET(COCOA_FOUND "NO")
ENDIF()

IF(APPLE)
    #Quicktime is not supported under 64bit OSX build so we need to detect it and disable it.
    #First check to see if we are running with a native 64-bit compiler (10.6 default) and implicit arch
    IF(NOT CMAKE_OSX_ARCHITECTURES AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(COCOA_FOUND "NO")
    ELSE()
        #Otherwise check to see if 64-bit is explicitly called for.
        LIST(FIND CMAKE_OSX_ARCHITECTURES "x86_64" has64Compile)
        IF(NOT has64Compile EQUAL -1)
            SET(COCOA_FOUND "NO")
        ENDIF()
    ENDIF()
ENDIF()
