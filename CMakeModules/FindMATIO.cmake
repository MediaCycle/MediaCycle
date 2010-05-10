# - Find MATIO
# Find the native MATIO includes and library
# This module defines
#  MATIO_INCLUDE_DIR, where to find jpeglib.h, etc.
#  MATIO_LIBRARIES, the libraries needed to use MATIO.
#  MATIO_FOUND, If false, do not try to use MATIO.
# also defined, but not for general use are
#  MATIO_LIBRARY, where to find the MATIO library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
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

FIND_PATH(MATIO_INCLUDE_DIR matio.h)

SET(MATIO_NAMES ${MATIO_NAMES} matio)
FIND_LIBRARY(MATIO_LIBRARY NAMES ${MATIO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set MATIO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MATIO DEFAULT_MSG MATIO_LIBRARY MATIO_INCLUDE_DIR)

IF(MATIO_FOUND)
  SET(MATIO_LIBRARIES ${MATIO_LIBRARY})
ENDIF(MATIO_FOUND)

# Deprecated declarations.
SET (NATIVE_MATIO_INCLUDE_PATH ${MATIO_INCLUDE_DIR} )
IF(MATIO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_MATIO_LIB_PATH ${MATIO_LIBRARY} PATH)
ENDIF(MATIO_LIBRARY)

MARK_AS_ADVANCED(MATIO_LIBRARY MATIO_INCLUDE_DIR )
