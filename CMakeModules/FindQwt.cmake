# - Find Qwt
# Find the native Qwt includes and library
# This module defines
#  QWT_INCLUDE_DIR, where to find qwt.h, etc.
#  QWT_LIBRARIES, the libraries needed to use QWT.
#  QWT_FOUND, If false, do not try to use QWT.
# also defined, but not for general use are
#  QWT_LIBRARY, where to find the QWT library.

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

FIND_PATH(QWT_INCLUDE_DIR qwt.h PATH_SUFFIXES "qwt")

SET(QWT_NAMES ${QWT_NAMES} qwt qwt-qt4)
FIND_LIBRARY(QWT_LIBRARY NAMES ${QWT_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QWT DEFAULT_MSG QWT_LIBRARY QWT_INCLUDE_DIR)

IF(QWT_FOUND)
  SET(QWT_LIBRARIES ${QWT_LIBRARY})
  GET_FILENAME_COMPONENT(QWT_LINK_DIRECTORIES ${QWT_LIBRARY} PATH)
ENDIF(QWT_FOUND)

# Deprecated declarations.
SET (NATIVE_QWT_INCLUDE_PATH ${QWT_INCLUDE_DIR} )
IF(QWT_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_QWT_LIB_PATH ${QWT_LIBRARY} PATH)
ENDIF(QWT_LIBRARY)

MARK_AS_ADVANCED(QWT_LIBRARY QWT_INCLUDE_DIR QWT_LINK_DIRECTORIES)
