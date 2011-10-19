# - Find TINYXML
# Find the native TINYXML includes and library
# This module defines
#  TINYXML_INCLUDE_DIR, where to find tinyxml.h, etc.
#  TINYXML_LIBRARIES, the libraries needed to use TINYXML.
#  TINYXML_FOUND, If false, do not try to use TINYXML.
# also defined, but not for general use are
#  TINYXML_LIBRARY, where to find the TINYXML library.
#  TINYXML_VERSION, the version of the TINYXML library.

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

FIND_PATH(TINYXML_INCLUDE_DIR tinyxml.h)

SET(TINYXML_NAMES ${TINYXML_NAMES} tinyxml)
FIND_LIBRARY(TINYXML_LIBRARY NAMES ${TINYXML_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TINYXML_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TINYXML DEFAULT_MSG TINYXML_LIBRARY TINYXML_INCLUDE_DIR)

IF(TINYXML_FOUND)
  SET(TINYXML_LIBRARIES ${TINYXML_LIBRARY})
  GET_FILENAME_COMPONENT(TINYXML_LINK_DIRECTORIES ${TINYXML_LIBRARY} PATH)
ENDIF(TINYXML_FOUND)

# Deprecated declarations.
SET (NATIVE_TINYXML_INCLUDE_PATH ${TINYXML_INCLUDE_DIR} )
IF(TINYXML_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_TINYXML_LIB_PATH ${TINYXML_LIBRARY} PATH)
ENDIF(TINYXML_LIBRARY)

MARK_AS_ADVANCED(TINYXML_LIBRARY TINYXML_INCLUDE_DIR TINYXML_LINK_DIRECTORIES)