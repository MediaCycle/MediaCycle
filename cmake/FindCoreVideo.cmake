# Locate Apple CoreVideo (next-generation QuickTime)
# This module defines
# COREVIDEO_LIBRARY
# COREVIDEO_FOUND, if false, do not try to link to gdal 
# COREVIDEO_INCLUDE_DIR, where to find the headers
#
# $COREVIDEO_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COREVIDEO_DIR
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
  FIND_PATH(COREVIDEO_INCLUDE_DIR CoreVideo/CoreVideo.h)
  FIND_LIBRARY(COREVIDEO_LIBRARY CoreVideo)
ENDIF()


SET(COREVIDEO_FOUND "NO")
IF(COREVIDEO_LIBRARY AND COREVIDEO_INCLUDE_DIR)
  SET(COREVIDEO_FOUND "YES")
ENDIF()

