# - Find RTMIDI
# Find the native RTMIDI includes and library
# This module defines
#  RTMIDI_INCLUDE_DIR, where to find RtMidi.h, etc.
#  RTMIDI_LIBRARIES, the libraries needed to use RTMIDI.
#  RTMIDI_FOUND, If false, do not try to use RTMIDI.
# also defined, but not for general use are
#  RTMIDI_LIBRARY, where to find the RTMIDI library.
#  RTMIDI_VERSION, the version of the RTMIDI library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2021 – UMONS - Numediart
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

FIND_PATH(RTMIDI_INCLUDE_DIR RtMidi.h PATH_SUFFIXES rtmidi)

FIND_LIBRARY(RTMIDI_LIBRARY NAMES rtmidi)

# handle the QUIETLY and REQUIRED arguments and set RTMIDI_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTMIDI DEFAULT_MSG RTMIDI_LIBRARY RTMIDI_INCLUDE_DIR)

IF(RTMIDI_FOUND)
  SET(RTMIDI_LIBRARIES ${RTMIDI_LIBRARY})
  GET_FILENAME_COMPONENT(RTMIDI_LINK_DIRECTORIES ${RTMIDI_LIBRARY} PATH)
ENDIF(RTMIDI_FOUND)

# Deprecated declarations.
SET (NATIVE_RTMIDI_INCLUDE_PATH ${RTMIDI_INCLUDE_DIR} )
IF(RTMIDI_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_RTMIDI_LIB_PATH ${RTMIDI_LIBRARY} PATH)
ENDIF(RTMIDI_LIBRARY)

MARK_AS_ADVANCED(RTMIDI_LIBRARY RTMIDI_INCLUDE_DIR RTMIDI_LINK_DIRECTORIES)