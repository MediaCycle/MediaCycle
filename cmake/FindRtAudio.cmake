# - Find RTAUDIO
# Find the native RTAUDIO includes and library
# This module defines
#  RTAUDIO_INCLUDE_DIR, where to find RtAudio.h, etc.
#  RTAUDIO_LIBRARIES, the libraries needed to use RTAUDIO.
#  RTAUDIO_FOUND, If false, do not try to use RTAUDIO.
# also defined, but not for general use are
#  RTAUDIO_LIBRARY, where to find the RTAUDIO library.
#  RTAUDIO_VERSION, the version of the RTAUDIO library.

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

FIND_PATH(RTAUDIO_INCLUDE_DIR RtAudio.h PATH_SUFFIXES rtaudio)

FIND_LIBRARY(RTAUDIO_LIBRARY NAMES rtaudio)

# handle the QUIETLY and REQUIRED arguments and set RTAUDIO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTAUDIO DEFAULT_MSG RTAUDIO_LIBRARY RTAUDIO_INCLUDE_DIR)

IF(RTAUDIO_FOUND)
  SET(RTAUDIO_LIBRARIES ${RTAUDIO_LIBRARY})
  GET_FILENAME_COMPONENT(RTAUDIO_LINK_DIRECTORIES ${RTAUDIO_LIBRARY} PATH)
ENDIF(RTAUDIO_FOUND)

# Deprecated declarations.
SET (NATIVE_RTAUDIO_INCLUDE_PATH ${RTAUDIO_INCLUDE_DIR} )
IF(RTAUDIO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_RTAUDIO_LIB_PATH ${RTAUDIO_LIBRARY} PATH)
ENDIF(RTAUDIO_LIBRARY)

MARK_AS_ADVANCED(RTAUDIO_LIBRARY RTAUDIO_INCLUDE_DIR RTAUDIO_LINK_DIRECTORIES)