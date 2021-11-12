# - Find Torch3
# Find the native Torch3 includes and library
# This module defines
#  TORCH3_INCLUDE_DIR, where to find jpeglib.h, etc.
#  TORCH3_LIBRARIES, the libraries needed to use TORCH3.
#  TORCH3_FOUND, If false, do not try to use TORCH3.
# also defined, but not for general use are
#  TORCH3_LIBRARY, where to find the TORCH3 library.

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

FIND_PATH(TORCH3_INCLUDE_DIR ParzenMachine.h NAMES torch)

SET(TORCH3_NAMES ${TORCH3_NAMES} torch)
FIND_LIBRARY(TORCH3_LIBRARY NAMES ${TORCH3_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TORCH3_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TORCH3 DEFAULT_MSG TORCH3_LIBRARY TORCH3_INCLUDE_DIR)

IF(TORCH3_FOUND)
  SET(TORCH3_LIBRARIES ${TORCH3_LIBRARY})
  GET_FILENAME_COMPONENT(TORCH3_LINK_DIRECTORIES ${TORCH3_LIBRARY} PATH)
ENDIF(TORCH3_FOUND)

# Deprecated declarations.
SET (NATIVE_TORCH3_INCLUDE_PATH ${TORCH3_INCLUDE_DIR} )
IF(TORCH3_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_TORCH3_LIB_PATH ${TORCH3_LIBRARY} PATH)
ENDIF(TORCH3_LIBRARY)

MARK_AS_ADVANCED(TORCH3_LIBRARY TORCH3_INCLUDE_DIR TORCH3_LINK_DIRECTORIES)
