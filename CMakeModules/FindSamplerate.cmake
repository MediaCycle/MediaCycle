# - Find SAMPLERATE
# Find the native SAMPLERATE includes and library
# This module defines
#  SAMPLERATE_INCLUDE_DIR, where to find jpeglib.h, etc.
#  SAMPLERATE_LIBRARIES, the libraries needed to use SAMPLERATE.
#  SAMPLERATE_FOUND, If false, do not try to use SAMPLERATE.
# also defined, but not for general use are
#  SAMPLERATE_LIBRARY, where to find the SAMPLERATE library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
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

FIND_PATH(SAMPLERATE_INCLUDE_DIR samplerate.h)

SET(SAMPLERATE_NAMES ${SAMPLERATE_NAMES} samplerate)
FIND_LIBRARY(SAMPLERATE_LIBRARY NAMES ${SAMPLERATE_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set SAMPLERATE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SAMPLERATE DEFAULT_MSG SAMPLERATE_LIBRARY SAMPLERATE_INCLUDE_DIR)

IF(SAMPLERATE_FOUND)
  SET(SAMPLERATE_LIBRARIES ${SAMPLERATE_LIBRARY})
  GET_FILENAME_COMPONENT(SAMPLERATE_LINK_DIRECTORIES ${SAMPLERATE_LIBRARY} PATH)
ENDIF(SAMPLERATE_FOUND)

# Deprecated declarations.
SET (NATIVE_SAMPLERATE_INCLUDE_PATH ${SAMPLERATE_INCLUDE_DIR} )
IF(SAMPLERATE_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_SAMPLERATE_LIB_PATH ${SAMPLERATE_LIBRARY} PATH)
ENDIF(SAMPLERATE_LIBRARY)

MARK_AS_ADVANCED(SAMPLERATE_LIBRARY SAMPLERATE_INCLUDE_DIR SAMPLERATE_LINK_DIRECTORIES)
