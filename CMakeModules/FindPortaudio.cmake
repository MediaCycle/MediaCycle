# - Find PORTAUDIO
# Find the native PORTAUDIO includes and library
# This module defines
#  PORTAUDIO_INCLUDE_DIR, where to find jpeglib.h, etc.
#  PORTAUDIO_LIBRARIES, the libraries needed to use PORTAUDIO.
#  PORTAUDIO_FOUND, If false, do not try to use PORTAUDIO.
# also defined, but not for general use are
#  PORTAUDIO_LIBRARY, where to find the PORTAUDIO library.

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

FIND_PATH(PORTAUDIO_INCLUDE_DIR portaudio.h)

SET(PORTAUDIO_NAMES ${PORTAUDIO_NAMES} portaudio)
FIND_LIBRARY(PORTAUDIO_LIBRARY NAMES ${PORTAUDIO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set PORTAUDIO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PORTAUDIO DEFAULT_MSG PORTAUDIO_LIBRARY PORTAUDIO_INCLUDE_DIR)

IF(PORTAUDIO_FOUND)
  SET(PORTAUDIO_LIBRARIES ${PORTAUDIO_LIBRARY})
  GET_FILENAME_COMPONENT(PORTAUDIO_LINK_DIRECTORIES ${PORTAUDIO_LIBRARY} PATH)
ENDIF(PORTAUDIO_FOUND)

# Deprecated declarations.
SET (NATIVE_PORTAUDIO_INCLUDE_PATH ${PORTAUDIO_INCLUDE_DIR} )
IF(PORTAUDIO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_PORTAUDIO_LIB_PATH ${PORTAUDIO_LIBRARY} PATH)
ENDIF(PORTAUDIO_LIBRARY)

MARK_AS_ADVANCED(PORTAUDIO_LIBRARY PORTAUDIO_INCLUDE_DIR PORTAUDIO_LINK_DIRECTORIES)
