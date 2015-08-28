# - Find Essentia
# Find the native Essentia includes and library
# This module defines
#  ESSENTIA_INCLUDE_DIR, where to find essentia/essentia.h, etc.
#  ESSENTIA_LIBRARIES, the libraries needed to use Essentia.
#  ESSENTIA_FOUND, If false, do not try to use Essentia.
# also defined, but not for general use are
#  ESSENTIA_LIBRARY, where to find the Essentia library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
# Author: Christian Frisson
# Copyright (c) 2015 – UMONS - Numediart
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

FIND_PATH(ESSENTIA_INCLUDE_DIR essentia/essentia.h)

SET(ESSENTIA_NAMES ${ESSENTIA_NAMES} essentia)
FIND_LIBRARY(ESSENTIA_LIBRARY NAMES ${ESSENTIA_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set ESSENTIA_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Essentia DEFAULT_MSG ESSENTIA_LIBRARY ESSENTIA_INCLUDE_DIR)

IF(ESSENTIA_FOUND)
  SET(ESSENTIA_LIBRARIES ${ESSENTIA_LIBRARY})
  GET_FILENAME_COMPONENT(ESSENTIA_LINK_DIRECTORIES ${ESSENTIA_LIBRARY} PATH)
ENDIF(ESSENTIA_FOUND)

# Deprecated declarations.
SET (NATIVE_ESSENTIA_INCLUDE_PATH ${ESSENTIA_INCLUDE_DIR} )
IF(ESSENTIA_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_ESSENTIA_LIB_PATH ${ESSENTIA_LIBRARY} PATH)
ENDIF(ESSENTIA_LIBRARY)

MARK_AS_ADVANCED(ESSENTIA_LIBRARY ESSENTIA_INCLUDE_DIR ESSENTIA_LINK_DIRECTORIES)
