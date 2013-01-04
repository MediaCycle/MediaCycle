# - Find Curl
# Find the native Curl includes and library
# This module defines
#  CURL_INCLUDE_DIR, where to find curl.h, etc.
#  CURL_LIBRARIES, the libraries needed to use Curl.
#  CURL_FOUND, If false, do not try to use Curl.
# also defined, but not for general use are
#  CURL_LIBRARY, where to find the Curl library.

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

FIND_PATH(CURL_INCLUDE_DIR curl.h PATH_SUFFIXES "curl")

SET(CURL_NAMES ${CURL_NAMES} curl)
FIND_LIBRARY(CURL_LIBRARY NAMES ${CURL_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Curl DEFAULT_MSG CURL_LIBRARY CURL_INCLUDE_DIR)

IF(CURL_FOUND)
  SET(CURL_LIBRARIES ${CURL_LIBRARY})
  GET_FILENAME_COMPONENT(CURL_LINK_DIRECTORIES ${CURL_LIBRARY} PATH)
ENDIF(CURL_FOUND)

# Deprecated declarations.
SET (NATIVE_CURL_INCLUDE_PATH ${CURL_INCLUDE_DIR} )
IF(CURL_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_CURL_LIB_PATH ${CURL_LIBRARY} PATH)
ENDIF(CURL_LIBRARY)

MARK_AS_ADVANCED(CURL_LIBRARY CURL_INCLUDE_DIR CURL_LINK_DIRECTORIES)
