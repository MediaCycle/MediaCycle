# - Find STK
# Find the native STK includes and library
# This module defines
#  STK_INCLUDE_DIR, where to find XnSTK.h, etc.
#  STK_LIBRARIES, the libraries needed to use STK.
#  STK_FOUND, If false, do not try to use STK.
# also defined, but not for general use are
#  STK_LIBRARY, where to find the STK library.
#  STK_VERSION, the version of the STK library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2012 – UMONS - Numediart
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

FIND_PATH(STK_INCLUDE_DIR Stk.h PATH_SUFFIXES stk)

FIND_LIBRARY(STK_LIBRARY NAMES stk)

# handle the QUIETLY and REQUIRED arguments and set STK_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(STK DEFAULT_MSG STK_LIBRARY STK_INCLUDE_DIR)

IF(STK_FOUND)
  SET(STK_LIBRARIES ${STK_LIBRARY})
  GET_FILENAME_COMPONENT(STK_LINK_DIRECTORIES ${STK_LIBRARY} PATH)
ENDIF(STK_FOUND)

# Deprecated declarations.
SET (NATIVE_STK_INCLUDE_PATH ${STK_INCLUDE_DIR} )
IF(STK_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_STK_LIB_PATH ${STK_LIBRARY} PATH)
ENDIF(STK_LIBRARY)

MARK_AS_ADVANCED(STK_LIBRARY STK_INCLUDE_DIR STK_LINK_DIRECTORIES)