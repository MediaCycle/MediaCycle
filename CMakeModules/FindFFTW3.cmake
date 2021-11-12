# - Find FFTW3
# Find the native FFTW3 includes and library
# This module defines
#  FFTW3_INCLUDE_DIR, where to find jpeglib.h, etc.
#  FFTW3_LIBRARIES, the libraries needed to use FFTW3.
#  FFTW3_FOUND, If false, do not try to use FFTW3.
# also defined, but not for general use are
#  FFTW3_LIBRARY, where to find the FFTW3 library.

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

FIND_PATH(FFTW3_INCLUDE_DIR fftw3.h)

SET(FFTW3_NAMES ${FFTW3_NAMES} fftw3)
FIND_LIBRARY(FFTW3_LIBRARY NAMES ${FFTW3_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set FFTW3_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3 DEFAULT_MSG FFTW3_LIBRARY FFTW3_INCLUDE_DIR)

IF(FFTW3_FOUND)
  SET(FFTW3_LIBRARIES ${FFTW3_LIBRARY})
  GET_FILENAME_COMPONENT(FFTW3_LINK_DIRECTORIES ${FFTW3_LIBRARY} PATH)
ENDIF(FFTW3_FOUND)

# Deprecated declarations.
SET (NATIVE_FFTW3_INCLUDE_PATH ${FFTW3_INCLUDE_DIR} )
IF(FFTW3_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_FFTW3_LIB_PATH ${FFTW3_LIBRARY} PATH)
ENDIF(FFTW3_LIBRARY)

MARK_AS_ADVANCED(FFTW3_LIBRARY FFTW3_INCLUDE_DIR FFTW3_LINK_DIRECTORIES)
