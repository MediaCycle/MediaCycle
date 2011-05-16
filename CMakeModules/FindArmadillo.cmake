# - Find ARMADILLO
# Find the native ARMADILLO includes and library
# This module defines
#  ARMADILLO_INCLUDE_DIR, where to find jpeglib.h, etc.
#  ARMADILLO_LIBRARIES, the libraries needed to use ARMADILLO.
#  ARMADILLO_FOUND, If false, do not try to use ARMADILLO.
# also defined, but not for general use are
#  ARMADILLO_LIBRARY, where to find the ARMADILLO library.
#  ARMADILLO_VERSION, the version of the ARMADILLO library.

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

FIND_PATH(ARMADILLO_INCLUDE_DIR armadillo)

SET(ARMADILLO_NAMES ${ARMADILLO_NAMES} armadillo)
FIND_LIBRARY(ARMADILLO_LIBRARY NAMES ${ARMADILLO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set ARMADILLO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ARMADILLO DEFAULT_MSG ARMADILLO_LIBRARY ARMADILLO_INCLUDE_DIR)

IF(ARMADILLO_FOUND)
  SET(ARMADILLO_LIBRARIES ${ARMADILLO_LIBRARY})
  GET_FILENAME_COMPONENT(ARMADILLO_LINK_DIRECTORIES ${ARMADILLO_LIBRARY} PATH)
ENDIF(ARMADILLO_FOUND)

# Deprecated declarations.
SET (NATIVE_ARMADILLO_INCLUDE_PATH ${ARMADILLO_INCLUDE_DIR} )
IF(ARMADILLO_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_ARMADILLO_LIB_PATH ${ARMADILLO_LIBRARY} PATH)
ENDIF(ARMADILLO_LIBRARY)

MARK_AS_ADVANCED(ARMADILLO_LIBRARY ARMADILLO_INCLUDE_DIR ARMADILLO_LINK_DIRECTORIES)

# Adapted from FindArmadillo.cmake from the standard CMake distribution

# Try to ascertain the version...
if(ARMADILLO_INCLUDE_DIR)
    set(_arma_Version_file "${ARMADILLO_INCLUDE_DIR}/armadillo_bits/arma_version.hpp")
    if(EXISTS "${_arma_Version_file}")
        file(READ "${_arma_Version_file}" _arma_Version_contents)
    else()
        set(_arma_Version_file "${ARMADILLO_INCLUDE_DIR}/armadillo_bits/version.hpp")
        if(EXISTS "${_arma_Version_file}")
            file(READ "${_arma_Version_file}" _arma_Version_contents)
        else()
            set(_arma_Version_contents "unknown")
        endif()
    endif()

    string(REGEX MATCH ".*#define ARMA_VERSION_MAJOR[ \t]+[0-9]+.*"
        _arma_new_version_defines "${_arma_Version_contents}")
    if(_arma_new_version_defines)
        string(REGEX REPLACE ".*#define ARMA_VERSION_MAJOR[ \t]+([0-9]+).*"
            "\\1" _arma_VERSION_MAJOR ${_arma_Version_contents})
        string(REGEX REPLACE ".*#define ARMA_VERSION_MINOR[ \t]+([0-9]+).*"
            "\\1" _arma_VERSION_MINOR ${_arma_Version_contents})
        string(REGEX REPLACE ".*#define ARMA_VERSION_PATCH[ \t]+([0-9]+).*"
            "\\1" _arma_VERSION_PATCH ${_arma_Version_contents})
    else()
        string(REGEX MATCH ".*static const unsigned int major = [0-9]+.*"
        _arma_old_version_defines "${_arma_Version_contents}")
        if(_arma_old_version_defines)
            string(REGEX REPLACE ".*static const unsigned int major = ([0-9]+).*"
            "\\1" _arma_VERSION_MAJOR ${_arma_Version_contents})
            string(REGEX REPLACE ".*static const unsigned int minor = ([0-9]+).*"
            "\\1" _arma_VERSION_MINOR ${_arma_Version_contents})
            string(REGEX REPLACE ".*static const unsigned int patch = ([0-9]+).*"
            "\\1" _arma_VERSION_PATCH ${_arma_Version_contents})
        else()
            message(FATAL_ERROR "[ FindArmadillo.cmake:${CMAKE_CURRENT_LIST_LINE} ] "
            "Failed to parse version number, please report this as a bug")
        endif()
    endif()

    set(ARMADILLO_VERSION "${_arma_VERSION_MAJOR}.${_arma_VERSION_MINOR}.${_arma_VERSION_PATCH}"
                                CACHE INTERNAL "The version of Armadillo which was detected")
endif()

if(Armadillo_FIND_VERSION AND ARMADILLO_VERSION)
    if(Armadillo_FIND_VERSION_EXACT)
        if(NOT ARMADILLO_VERSION VERSION_EQUAL ${Armadillo_FIND_VERSION})
            MESSAGE(FATAL_ERROR "Armadillo version found is ${ARMADILLO_VERSION}, while version needed is exactly ${Armadillo_FIND_VERSION}.")
        endif()
    else()
        # version is too low
        if(NOT ARMADILLO_VERSION VERSION_EQUAL ${Armadillo_FIND_VERSION} AND 
                NOT ARMADILLO_VERSION VERSION_GREATER ${Armadillo_FIND_VERSION})
            MESSAGE(FATAL_ERROR "Armadillo version found is ${ARMADILLO_VERSION}, while version needed is ${Armadillo_FIND_VERSION}.")
        endif()
    endif()
endif()


