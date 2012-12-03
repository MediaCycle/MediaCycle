# - Find GSL
# Find the native GSL includes and library
# This module defines
#  GSL_INCLUDE_DIR, where to find gsl/gsl-statistics.h, etc.
#  GSL_LIBRARIES, the libraries needed to use GSL.
#  GSL_FOUND, If false, do not try to use GSL.
# also defined, but not for general use are
#  GSL_LIBRARY, where to find the GSL library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more inforGSLn.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(GSL_INCLUDE_DIR gsl_statistics.h PATH_SUFFIXES "gsl")

SET(GSL_NAMES ${GSL_NAMES} GSL)
FIND_LIBRARY(GSL_LIBRARY NAMES ${GSL_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set GSL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL DEFAULT_MSG GSL_LIBRARY GSL_INCLUDE_DIR)

IF(GSL_FOUND)
  SET(GSL_LIBRARIES ${GSL_LIBRARY})
  GET_FILENAME_COMPONENT(GSL_LINK_DIRECTORIES ${GSL_LIBRARY} PATH)
ENDIF(GSL_FOUND)

# Deprecated declarations.
SET (NATIVE_GSL_INCLUDE_PATH ${GSL_INCLUDE_DIR} )
IF(GSL_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_GSL_LIB_PATH ${GSL_LIBRARY} PATH)
ENDIF(GSL_LIBRARY)

MARK_AS_ADVANCED(GSL_LIBRARY GSL_INCLUDE_DIR GSL_LINK_DIRECTORIES)
