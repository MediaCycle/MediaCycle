# - Find LIBLO
# Find the native LIBLO includes and library
# This module defines
#  LIBLO_INCLUDE_DIR, where to find jpeglib.h, etc.
#  LIBLO_LIBRARIES, the libraries needed to use LIBLO.
#  LIBLO_FOUND, If false, do not try to use LIBLO.
# also defined, but not for general use are
#  LIBLO_LIBRARY, where to find the LIBLO library.

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

FIND_PATH(LIBLO_INCLUDE_DIR lo/lo.h)

SET(LIBLO_NAMES ${LIBLO_NAMES} lo)
FIND_LIBRARY(LIBLO_LIBRARY NAMES ${LIBLO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set LIBLO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBLO DEFAULT_MSG LIBLO_LIBRARY LIBLO_INCLUDE_DIR)

IF(LIBLO_FOUND)
  SET(LIBLO_LIBRARIES ${LIBLO_LIBRARY})
  GET_FILENAME_COMPONENT(LIBLO_LINK_DIRECTORIES ${LIBLO_LIBRARY} PATH)
ENDIF(LIBLO_FOUND)

# Deprecated declarations.
SET (NATIVE_LIBLO_INCLUDE_PATH ${LIBLO_INCLUDE_DIR} )
IF(LIBLO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_LIBLO_LIB_PATH ${LIBLO_LIBRARY} PATH)
ENDIF(LIBLO_LIBRARY)

MARK_AS_ADVANCED(LIBLO_LIBRARY LIBLO_INCLUDE_DIR LIBLO_LINK_DIRECTORIES)
