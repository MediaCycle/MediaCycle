# - Find LibPoDoFo
# Find the native PoDoFo includes and library
# This module defines
#  PODOFO_INCLUDE_DIR, where to find podofo.h, etc.
#  PODOFO_LIBRARIES, the libraries needed to use PODOFO.
#  PODOFO_FOUND, If false, do not try to use PODOFO.
# also defined, but not for general use are
#  PODOFO_LIBRARY, where to find the PODOFO library.

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

FIND_PATH(PODOFO_INCLUDE_DIR podofo/podofo.h)

SET(PODOFO_NAMES ${PODOFO_NAMES} podofo)
FIND_LIBRARY(PODOFO_LIBRARY NAMES ${PODOFO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set PODOFO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PODOFO DEFAULT_MSG PODOFO_LIBRARY PODOFO_INCLUDE_DIR)

IF(PODOFO_FOUND)
  SET(PODOFO_LIBRARIES ${PODOFO_LIBRARY})
  GET_FILENAME_COMPONENT(PODOFO_LINK_DIRECTORIES ${PODOFO_LIBRARY} PATH)
ENDIF(PODOFO_FOUND)

# Deprecated declarations.
SET (NATIVE_PODOFO_INCLUDE_PATH ${PODOFO_INCLUDE_DIR} )
IF(PODOFO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_PODOFO_LIB_PATH ${PODOFO_LIBRARY} PATH)
ENDIF(PODOFO_LIBRARY)

MARK_AS_ADVANCED(PODOFO_LIBRARY PODOFO_LIBRARIES PODOFO_INCLUDE_DIR PODOFO_LINK_DIRECTORIES)
