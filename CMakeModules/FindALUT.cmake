# - Find ALUT
# Find the native ALUT includes and library
# This module defines
#  ALUT_INCLUDE_DIR, where to find jpeglib.h, etc.
#  ALUT_LIBRARIES, the libraries needed to use ALUT.
#  ALUT_FOUND, If false, do not try to use ALUT.
# also defined, but not for general use are
#  ALUT_LIBRARY, where to find the ALUT library.

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

FIND_PATH(ALUT_INCLUDE_DIR AL/alut.h OpenAL/alut.h alut.h)

SET(ALUT_NAMES ${ALUT_NAMES} alut)
FIND_LIBRARY(ALUT_LIBRARY NAMES ${ALUT_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set ALUT_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALUT DEFAULT_MSG ALUT_LIBRARY ALUT_INCLUDE_DIR)

IF(ALUT_FOUND)
  SET(ALUT_LIBRARIES ${ALUT_LIBRARY})
ENDIF(ALUT_FOUND)

# Deprecated declarations.
SET (NATIVE_ALUT_INCLUDE_PATH ${ALUT_INCLUDE_DIR} )
IF(ALUT_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_ALUT_LIB_PATH ${ALUT_LIBRARY} PATH)
ENDIF(ALUT_LIBRARY)

MARK_AS_ADVANCED(ALUT_LIBRARY ALUT_INCLUDE_DIR )
