# Locate Apple CoreGraphics
# This module defines
# COREGRAPHICS_LIBRARY
# COREGRAPHICS_FOUND, if false, do not try to link to gdal 
# COREGRAPHICS_INCLUDE_DIR, where to find the headers
#
# $COREGRAPHICS_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COREGRAPHICS_DIR
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
  FIND_PATH(COREGRAPHICS_INCLUDE_DIR CoreGraphics/CoreGraphics.h)
  FIND_LIBRARY(COREGRAPHICS_LIBRARY CoreGraphics)
ENDIF()


SET(COREGRAPHICS_FOUND "NO")
IF(COREGRAPHICS_LIBRARY AND COREGRAPHICS_INCLUDE_DIR)
  SET(COREGRAPHICS_FOUND "YES")
ENDIF()

