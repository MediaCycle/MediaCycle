# Locate Apple CoreFoundation
# This module defines
# COREFOUNDATION_LIBRARY
# COREFOUNDATION_FOUND, if false, do not try to link to gdal 
# COREFOUNDATION_INCLUDE_DIR, where to find the headers
#
# $COREFOUNDATION_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COREFOUNDATION_DIR
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
  FIND_PATH(COREFOUNDATION_INCLUDE_DIR CoreFoundation/CoreFoundation.h)
  FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
ENDIF()


SET(COREFOUNDATION_FOUND "NO")
IF(COREFOUNDATION_LIBRARY AND COREFOUNDATION_INCLUDE_DIR)
  SET(COREFOUNDATION_FOUND "YES")
ENDIF()

