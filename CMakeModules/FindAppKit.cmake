# Locate Apple AppKit
# This module defines
# APPKIT_LIBRARY
# APPKIT_FOUND, if false, do not try to link to gdal 
# APPKIT_INCLUDE_DIR, where to find the headers
#
# $APPKIT_DIR is an environment variable that would
# correspond to the ./configure --prefix=$APPKIT_DIR
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
  FIND_PATH(APPKIT_INCLUDE_DIR AppKit/AppKit.h)
  FIND_LIBRARY(APPKIT_LIBRARY AppKit)
ENDIF()


SET(APPKIT_FOUND "NO")
IF(APPKIT_LIBRARY AND APPKIT_INCLUDE_DIR)
  SET(APPKIT_FOUND "YES")
ENDIF()

