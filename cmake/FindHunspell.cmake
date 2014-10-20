# - Find HUNSPELL
# Find the native HUNSPELL includes and library
# This module defines
#  HUNSPELL_INCLUDE_DIR, where to find falcon/core/FalconCore.h, etc.
#  HUNSPELL_LIBRARIES, the libraries needed to use HUNSPELL.
#  HUNSPELL_FOUND, If false, do not try to use HUNSPELL.
# also defined, but not for general use are
#  HUNSPELL_LIBRARY, where to find the HUNSPELL library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
# Author: Christian Frisson
# Copyright (c) 2013 – UMONS - Numediart
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

FIND_PATH(HUNSPELL_INCLUDE_DIR hunspell.hxx PATHS_SUFFIXES "hunspell")

SET(HUNSPELL_NAMES ${HUNSPELL_NAMES} hunspell)
FIND_LIBRARY(HUNSPELL_LIBRARY NAMES ${HUNSPELL_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set HUNSPELL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HUNSPELL DEFAULT_MSG HUNSPELL_LIBRARY HUNSPELL_INCLUDE_DIR)

IF(HUNSPELL_FOUND)
  SET(HUNSPELL_LIBRARIES ${HUNSPELL_LIBRARY})
  GET_FILENAME_COMPONENT(HUNSPELL_LINK_DIRECTORIES ${HUNSPELL_LIBRARY} PATH)
ENDIF(HUNSPELL_FOUND)

# Deprecated declarations.
SET (NATIVE_HUNSPELL_INCLUDE_PATH ${HUNSPELL_INCLUDE_DIR} )
IF(HUNSPELL_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_HUNSPELL_LIB_PATH ${HUNSPELL_LIBRARY} PATH)
ENDIF(HUNSPELL_LIBRARY)

MARK_AS_ADVANCED(HUNSPELL_LIBRARY HUNSPELL_INCLUDE_DIR HUNSPELL_LINK_DIRECTORIES)
