# Locate Apple Foundation
# This module defines
# FOUNDATION_LIBRARY
# FOUNDATION_FOUND, if false, do not try to link to gdal 
# FOUNDATION_INCLUDE_DIR, where to find the headers
#
# $FOUNDATION_DIR is an environment variable that would
# correspond to the ./configure --prefix=$FOUNDATION_DIR
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
  FIND_PATH(FOUNDATION_INCLUDE_DIR Foundation/Foundation.h)
  FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
ENDIF()


SET(FOUNDATION_FOUND "NO")
IF(FOUNDATION_LIBRARY AND FOUNDATION_INCLUDE_DIR)
  SET(FOUNDATION_FOUND "YES")
ENDIF()

