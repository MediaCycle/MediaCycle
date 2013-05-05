# Locate Apple CoreMIDI
# This module defines
# COREMIDI_LIBRARY
# COREMIDI_FOUND, if false, do not try to link to gdal 
# COREMIDI_INCLUDE_DIR, where to find the headers
#
# $COREMIDI_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COREMIDI_DIR
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
  FIND_PATH(COREMIDI_INCLUDE_DIR CoreMIDI/CoreMIDI.h)
  FIND_LIBRARY(COREMIDI_LIBRARY CoreMIDI)
ENDIF()


SET(COREMIDI_FOUND "NO")
IF(COREMIDI_LIBRARY AND COREMIDI_INCLUDE_DIR)
  SET(COREMIDI_FOUND "YES")
ENDIF()

