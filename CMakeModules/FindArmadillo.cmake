# - Find ARMADILLO
# Find the native ARMADILLO includes and library
# This module defines
#  ARMADILLO_INCLUDE_DIR, where to find jpeglib.h, etc.
#  ARMADILLO_LIBRARIES, the libraries needed to use ARMADILLO.
#  ARMADILLO_FOUND, If false, do not try to use ARMADILLO.
# also defined, but not for general use are
#  ARMADILLO_LIBRARY, where to find the ARMADILLO library.

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

FIND_PATH(ARMADILLO_INCLUDE_DIR armadillo)

SET(ARMADILLO_NAMES ${ARMADILLO_NAMES} armadillo)
FIND_LIBRARY(ARMADILLO_LIBRARY NAMES ${ARMADILLO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set ARMADILLO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ARMADILLO DEFAULT_MSG ARMADILLO_LIBRARY ARMADILLO_INCLUDE_DIR)

IF(ARMADILLO_FOUND)
  SET(ARMADILLO_LIBRARIES ${ARMADILLO_LIBRARY})
  GET_FILENAME_COMPONENT(ARMADILLO_LINK_DIRECTORIES ${ARMADILLO_LIBRARY} PATH)
ENDIF(ARMADILLO_FOUND)

# Deprecated declarations.
SET (NATIVE_ARMADILLO_INCLUDE_PATH ${ARMADILLO_INCLUDE_DIR} )
IF(ARMADILLO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_ARMADILLO_LIB_PATH ${ARMADILLO_LIBRARY} PATH)
ENDIF(ARMADILLO_LIBRARY)

MARK_AS_ADVANCED(ARMADILLO_LIBRARY ARMADILLO_INCLUDE_DIR ARMADILLO_LINK_DIRECTORIES)
