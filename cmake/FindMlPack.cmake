# - Find MLPACK
# Find the native MLPACK includes and library
# This module defines
#  MLPACK_INCLUDE_DIR, where to find jpeglib.h, etc.
#  MLPACK_LIBRARIES, the libraries needed to use MLPACK.
#  MLPACK_FOUND, If false, do not try to use MLPACK.
# also defined, but not for general use are
#  MLPACK_LIBRARY, where to find the MLPACK library.

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
MESSAGE("Find MlPack")
FIND_PATH(MLPACK_INCLUDE_DIR mlpack)


SET(MLPACK_NAMES ${MLPACK_NAMES} mlpack)
FIND_LIBRARY(MLPACK_LIBRARY NAMES ${MLPACK_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set MLPACK_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MLPACK DEFAULT_MSG MLPACK_LIBRARY MLPACK_INCLUDE_DIR)

IF(MLPACK_FOUND)
  SET(MLPACK_LIBRARIES ${MLPACK_LIBRARY})
  GET_FILENAME_COMPONENT(MLPACK_LINK_DIRECTORIES ${MLPACK_LIBRARY} PATH)
ENDIF(MLPACK_FOUND)


# Deprecated declarations.
SET (NATIVE_MLPACK_INCLUDE_PATH ${MLPACK_INCLUDE_DIR} )
IF(MLPACK_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_MLPACK_LIB_PATH ${MLPACK_LIBRARY} PATH)
ENDIF(MLPACK_LIBRARY)

MARK_AS_ADVANCED(MLPACK_LIBRARY MLPACK_INCLUDE_DIR MLPACK_LINK_DIRECTORIES)


if(MlPack_FIND_VERSION AND MLPACK_VERSION)
    if(MlPack_FIND_VERSION_EXACT)
        if(NOT MLPACK_VERSION VERSION_EQUAL ${MlPack_FIND_VERSION})
            MESSAGE(FATAL_ERROR "MlPack version found is ${MLPACK_VERSION}, while version needed is exactly ${MlPack_FIND_VERSION}.")
        endif()
    else()
        # version is too low
        if(NOT MLPACK_VERSION VERSION_EQUAL ${MlPack_FIND_VERSION} AND 
                NOT MLPACK_VERSION VERSION_GREATER ${MlPack_FIND_VERSION})
            MESSAGE(FATAL_ERROR "MlPack version found is ${MLPACK_VERSION}, while version needed is ${MlPack_FIND_VERSION}.")
        endif()
    endif()
endif()


