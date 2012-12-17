# Locate Apple CoreAudio
# This module defines
# COREAUDIO_LIBRARY
# COREAUDIO_FOUND, if false, do not try to link to gdal 
# COREAUDIO_INCLUDE_DIR, where to find the headers
#
# $COREAUDIO_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COREAUDIO_DIR
#
#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2011 – UMONS - Numediart
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


IF(APPLE)
  FIND_PATH(COREAUDIO_INCLUDE_DIR CoreAudio/CoreAudio.h)
  FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
ENDIF()


SET(COREAUDIO_FOUND "NO")
IF(COREAUDIO_LIBRARY AND COREAUDIO_INCLUDE_DIR)
  SET(COREAUDIO_FOUND "YES")
ENDIF()

