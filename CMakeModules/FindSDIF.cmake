# - Find SDIF
# Find the native SDIF includes and library
# This module defines
#  SDIF_INCLUDE_DIR, where to find jpeglib.h, etc.
#  SDIF_LIBRARIES, the libraries needed to use SDIF.
#  SDIF_FOUND, If false, do not try to use SDIF.
# also defined, but not for general use are
#  SDIF_LIBRARY, where to find the SDIF library.

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

FIND_PATH(SDIF_INCLUDE_DIR sdif.h)

SET(SDIF_NAMES ${SDIF_NAMES} sdif)
FIND_LIBRARY(SDIF_LIBRARY NAMES ${SDIF_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set SDIF_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDIF DEFAULT_MSG SDIF_LIBRARY SDIF_INCLUDE_DIR)

IF(SDIF_FOUND)
  SET(SDIF_LIBRARIES ${SDIF_LIBRARY})
ENDIF(SDIF_FOUND)

# Deprecated declarations.
SET (NATIVE_SDIF_INCLUDE_PATH ${SDIF_INCLUDE_DIR} )
IF(SDIF_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_SDIF_LIB_PATH ${SDIF_LIBRARY} PATH)
ENDIF(SDIF_LIBRARY)

MARK_AS_ADVANCED(SDIF_LIBRARY SDIF_INCLUDE_DIR )
