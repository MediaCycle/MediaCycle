# - Find OpenNI
# Find the native OPENNI includes and library
# This module defines
#  OPENNI_INCLUDE_DIR, where to find XnOPENNI.h, etc.
#  OPENNI_LIBRARIES, the libraries needed to use OPENNI.
#  OPENNI_FOUND, If false, do not try to use OPENNI.
# also defined, but not for general use are
#  OPENNI_LIBRARY, where to find the OPENNI library.
#  OPENNI_VERSION, the version of the OPENNI library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2012 – UMONS - Numediart
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

FIND_PATH(OPENNI_INCLUDE_DIR XnOpenNI.h PATH_SUFFIXES "ni" "OpenNI")

FIND_LIBRARY(OPENNI_LIBRARY NAMES OpenNI)

# handle the QUIETLY and REQUIRED arguments and set OPENNI_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENNI DEFAULT_MSG OPENNI_LIBRARY OPENNI_INCLUDE_DIR)

IF(OPENNI_FOUND)
  SET(OPENNI_LIBRARIES ${OPENNI_LIBRARY})
  GET_FILENAME_COMPONENT(OPENNI_LINK_DIRECTORIES ${OPENNI_LIBRARY} PATH)
ENDIF(OPENNI_FOUND)

# Deprecated declarations.
SET (NATIVE_OPENNI_INCLUDE_PATH ${OPENNI_INCLUDE_DIR} )
IF(OPENNI_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_OPENNI_LIB_PATH ${OPENNI_LIBRARY} PATH)
ENDIF(OPENNI_LIBRARY)

MARK_AS_ADVANCED(OPENNI_LIBRARY OPENNI_INCLUDE_DIR OPENNI_LINK_DIRECTORIES)