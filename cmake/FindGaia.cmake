# - Find Gaia
# Find the native Gaia includes and library
# This module defines
#  GAIA_INCLUDE_DIR, where to find gaia2/gaia.h, etc.
#  GAIA_LIBRARIES, the libraries needed to use Gaia.
#  GAIA_FOUND, If false, do not try to use Gaia.
# also defined, but not for general use are
#  GAIA_LIBRARY, where to find the Gaia library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
# Author: Christian Frisson
# Copyright (c) 2015 – UMONS - Numediart
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

FIND_PATH(GAIA_INCLUDE_DIR gaia2/gaia.h)

SET(GAIA_NAMES ${GAIA_NAMES} gaia2)
FIND_LIBRARY(GAIA_LIBRARY NAMES ${GAIA_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set GAIA_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gaia DEFAULT_MSG GAIA_LIBRARY GAIA_INCLUDE_DIR)

IF(GAIA_FOUND)
  SET(GAIA_LIBRARIES ${GAIA_LIBRARY})
  GET_FILENAME_COMPONENT(GAIA_LINK_DIRECTORIES ${GAIA_LIBRARY} PATH)
ENDIF(GAIA_FOUND)

# Deprecated declarations.
SET (NATIVE_GAIA_INCLUDE_PATH ${GAIA_INCLUDE_DIR} )
IF(GAIA_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_GAIA_LIB_PATH ${GAIA_LIBRARY} PATH)
ENDIF(GAIA_LIBRARY)

MARK_AS_ADVANCED(GAIA_LIBRARY GAIA_INCLUDE_DIR GAIA_LINK_DIRECTORIES)
