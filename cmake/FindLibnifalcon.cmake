# - Find LIBNIFALCON
# Find the native LIBNIFALCON includes and library
# This module defines
#  LIBNIFALCON_INCLUDE_DIR, where to find falcon/core/FalconCore.h, etc.
#  LIBNIFALCON_LIBRARIES, the libraries needed to use LIBNIFALCON.
#  LIBNIFALCON_FOUND, If false, do not try to use LIBNIFALCON.
# also defined, but not for general use are
#  LIBNIFALCON_LIBRARY, where to find the libnifalcon library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
# Author: Christian Frisson
# Copyright (c) 2013 – UMONS - Numediart
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

FIND_PATH(LIBNIFALCON_INCLUDE_DIR falcon/core/FalconCore.h)

SET(LIBNIFALCON_NAMES ${LIBNIFALCON_NAMES} nifalcon)
FIND_LIBRARY(LIBNIFALCON_LIBRARY NAMES ${LIBNIFALCON_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set LIBNIFALCON_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBNIFALCON DEFAULT_MSG LIBNIFALCON_LIBRARY LIBNIFALCON_INCLUDE_DIR)

IF(LIBNIFALCON_FOUND)
  SET(LIBNIFALCON_LIBRARIES ${LIBNIFALCON_LIBRARY})
  GET_FILENAME_COMPONENT(LIBNIFALCON_LINK_DIRECTORIES ${LIBNIFALCON_LIBRARY} PATH)
ENDIF(LIBNIFALCON_FOUND)

# Deprecated declarations.
SET (NATIVE_LIBNIFALCON_INCLUDE_PATH ${LIBNIFALCON_INCLUDE_DIR} )
IF(LIBNIFALCON_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_LIBNIFALCON_LIB_PATH ${LIBNIFALCON_LIBRARY} PATH)
ENDIF(LIBNIFALCON_LIBRARY)

MARK_AS_ADVANCED(LIBNIFALCON_LIBRARY LIBNIFALCON_INCLUDE_DIR LIBNIFALCON_LINK_DIRECTORIES)
