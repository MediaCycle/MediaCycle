# - Find LIBDISPATCH
# Find the native LIBDISPATCH includes and library
# This module defines
#  LIBDISPATCH_INCLUDE_DIR, where to find dispatch/queue.h, etc.
#  LIBDISPATCH_LIBRARIES, the libraries needed to use LIBDISPATCH.
#  LIBDISPATCH_FOUND, If false, do not try to use LIBDISPATCH.
# also defined, but not for general use are
#  LIBDISPATCH_LIBRARY, where to find the LIBDISPATCH library.
#  LIBDISPATCH_VERSION, the version of the LIBDISPATCH library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2014 – UMONS - Numediart
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(LIBDISPATCH_INCLUDE_DIR dispatch/queue.h)

IF(NOT APPLE)
FIND_LIBRARY(LIBDISPATCH_LIBRARY NAMES dispatch)
ENDIF(NOT APPLE)

# handle the QUIETLY and REQUIRED arguments and set LIBDISPATCH_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
IF(APPLE)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBDISPATCH DEFAULT_MSG LIBDISPATCH_INCLUDE_DIR)
ELSE()
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBDISPATCH DEFAULT_MSG LIBDISPATCH_LIBRARY LIBDISPATCH_INCLUDE_DIR)
ENDIF()

IF(LIBDISPATCH_FOUND)
  SET(LIBDISPATCH_LIBRARIES ${LIBDISPATCH_LIBRARY})
IF(NOT APPLE)
  GET_FILENAME_COMPONENT(LIBDISPATCH_LINK_DIRECTORIES ${LIBDISPATCH_LIBRARY} PATH)
ENDIF()
ENDIF(LIBDISPATCH_FOUND)

# Deprecated declarations.
SET (NATIVE_LIBDISPATCH_INCLUDE_PATH ${LIBDISPATCH_INCLUDE_DIR} )
IF(LIBDISPATCH_LIBRARY AND NOT APPLE)
  GET_FILENAME_COMPONENT (NATIVE_LIBDISPATCH_LIB_PATH ${LIBDISPATCH_LIBRARY} PATH)
ENDIF(LIBDISPATCH_LIBRARY AND NOT APPLE)

MARK_AS_ADVANCED(LIBDISPATCH_LIBRARY LIBDISPATCH_INCLUDE_DIR LIBDISPATCH_LINK_DIRECTORIES)