# - Find NEWMAT
# Find the native NEWMAT includes and library
# This module defines
#  NEWMAT_INCLUDE_DIR, where to find XnNEWMAT.h, etc.
#  NEWMAT_LIBRARIES, the libraries needed to use NEWMAT.
#  NEWMAT_FOUND, If false, do not try to use NEWMAT.
# also defined, but not for general use are
#  NEWMAT_LIBRARY, where to find the NEWMAT library.
#  NEWMAT_VERSION, the version of the NEWMAT library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2014 – UMONS - Numediart
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

FIND_PATH(NEWMAT_INCLUDE_DIR newmat.h PATH_SUFFIXES newmat)

FIND_LIBRARY(NEWMAT_LIBRARY NAMES newmat)

# handle the QUIETLY and REQUIRED arguments and set NEWMAT_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NEWMAT DEFAULT_MSG NEWMAT_LIBRARY NEWMAT_INCLUDE_DIR)

IF(NEWMAT_FOUND)
  SET(NEWMAT_LIBRARIES ${NEWMAT_LIBRARY})
  GET_FILENAME_COMPONENT(NEWMAT_LINK_DIRECTORIES ${NEWMAT_LIBRARY} PATH)
ENDIF(NEWMAT_FOUND)

# Deprecated declarations.
SET (NATIVE_NEWMAT_INCLUDE_PATH ${NEWMAT_INCLUDE_DIR} )
IF(NEWMAT_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_NEWMAT_LIB_PATH ${NEWMAT_LIBRARY} PATH)
ENDIF(NEWMAT_LIBRARY)

MARK_AS_ADVANCED(NEWMAT_LIBRARY NEWMAT_INCLUDE_DIR NEWMAT_LINK_DIRECTORIES)