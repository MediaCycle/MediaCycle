# Locate Apple Accelerate
# This module defines
# ACCELERATE_LIBRARY
# ACCELERATE_FOUND, if false, do not try to link to Accelerate 
# ACCELERATE_INCLUDE_DIR, where to find the headers
# ACCELERATE_INCLUDE_DIRS, where to find the headers
#
# $ACCELERATE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$ACCELERATE_DIR
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
  FIND_PATH(ACCELERATE_INCLUDE_DIR Accelerate/Accelerate.h)
  FIND_LIBRARY(ACCELERATE_LIBRARY Accelerate)
ENDIF()

SET(ACCELERATE_FOUND "NO")
IF(ACCELERATE_LIBRARY AND ACCELERATE_INCLUDE_DIR)
  SET(ACCELERATE_FOUND "YES")
  SET(ACCELERATE_INCLUDE_DIRS "${ACCELERATE_INCLUDE_DIR};/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers")
ENDIF()

