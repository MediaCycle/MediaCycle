# Locate Apple CoreData (next-generation QuickTime)
# This module defines
# COREDATA_LIBRARY
# COREDATA_FOUND, if false, do not try to link to gdal 
# COREDATA_INCLUDE_DIR, where to find the headers
#
# $COREDATA_DIR is an environment variable that would
# correspond to the ./configure --prefix=$COREDATA_DIR
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
  FIND_PATH(COREDATA_INCLUDE_DIR CoreData/CoreData.h)
  FIND_LIBRARY(COREDATA_LIBRARY CoreData)
ENDIF()


SET(COREDATA_FOUND "NO")
IF(COREDATA_LIBRARY AND COREDATA_INCLUDE_DIR)
  SET(COREDATA_FOUND "YES")
ENDIF()

