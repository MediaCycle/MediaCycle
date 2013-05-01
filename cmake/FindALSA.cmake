# - Find ALSA
# Find the native ALSA includes and library
# This module defines
#  ALSA_INCLUDE_DIR, where to find asoundlib.h, etc.
#  ALSA_LIBRARIES, the libraries needed to use ALSA.
#  ALSA_FOUND, If false, do not try to use ALSA.
# also defined, but not for general use are
#  ALSA_LIBRARY, where to find the ALSA library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2013 – UMONS - Numediart
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more inforALSAn.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(ALSA_INCLUDE_DIR alsa/asoundlib.h)

SET(ALSA_NAMES ${ALSA_NAMES} ALSA asound)
FIND_LIBRARY(ALSA_LIBRARY NAMES ${ALSA_NAMES} PATH_SUFFIXES i386-linux-gnu x86_64-linux-gnu)

# handle the QUIETLY and REQUIRED arguments and set ALSA_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALSA DEFAULT_MSG ALSA_LIBRARY ALSA_INCLUDE_DIR)

IF(ALSA_FOUND)
  SET(ALSA_LIBRARIES ${ALSA_LIBRARY})
  GET_FILENAME_COMPONENT(ALSA_LINK_DIRECTORIES ${ALSA_LIBRARY} PATH)
ENDIF(ALSA_FOUND)

# Deprecated declarations.
SET (NATIVE_ALSA_INCLUDE_PATH ${ALSA_INCLUDE_DIR} )
IF(ALSA_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_ALSA_LIB_PATH ${ALSA_LIBRARY} PATH)
ENDIF(ALSA_LIBRARY)

MARK_AS_ADVANCED(ALSA_LIBRARY ALSA_INCLUDE_DIR ALSA_LINK_DIRECTORIES)
