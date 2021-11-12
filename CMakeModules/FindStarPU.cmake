# - Find STARPU
# Find the native STARPU includes and library
# This module defines
#  STARPU_INCLUDE_DIR, where to find starpu.h, etc.
#  STARPU_LIBRARIES, the libraries needed to use STARPU.
#  STARPU_FOUND, If false, do not try to use STARPU.
#  STARPU_USE_CPU
#  STARPU_USE_CUDA
#  STARPU_USE_OPENCL
# also defined, but not for general use are
#  STARPU_LIBRARY, where to find the STARPU library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2010 – UMONS - Numediart
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

FIND_PATH(STARPU_INCLUDE_DIR starpu_config.h)

SET(STARPU_NAMES ${STARPU_NAMES} starpu)
FIND_LIBRARY(STARPU_LIBRARY NAMES ${STARPU_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set STARPU_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(STARPU DEFAULT_MSG STARPU_LIBRARY STARPU_INCLUDE_DIR)

IF(STARPU_FOUND)
  SET(STARPU_LIBRARIES ${STARPU_LIBRARY})
  GET_FILENAME_COMPONENT(STARPU_LINK_DIRECTORIES ${STARPU_LIBRARY} PATH)

  #INCLUDE (CheckIncludeFiles) #CF all of these worked at first, but not anymore lately
  #CHECK_SYMBOL_EXISTS (STARPU_USE_CPU "starpu_config.h" STARPU_USE_CPU)
  #CHECK_SYMBOL_EXISTS (STARPU_USE_CUDA "starpu_config.h" STARPU_USE_CUDA)
  #CHECK_SYMBOL_EXISTS (STARPU_USE_OPENCL "starpu_config.h" STARPU_USE_OPENCL)

  set(starpu_config "${STARPU_INCLUDE_DIR}/starpu_config.h")
  if(EXISTS "${starpu_config}")
    file(READ "${starpu_config}" starpu_config_contents)
    string(REGEX MATCH ".*#define STARPU_USE_CPU 1.*" STARPU_USE_CPU "${starpu_config_contents}")
    string(REGEX MATCH ".*#define STARPU_USE_CUDA 1.*" STARPU_USE_CUDA "${starpu_config_contents}")
    string(REGEX MATCH ".*#define STARPU_USE_OPENCL 1.*" STARPU_USE_OPENCL "${starpu_config_contents}")
    IF(STARPU_USE_CPU)
      SET(STARPU_USE_CPU ON)
    ENDIF()
        IF(STARPU_USE_OPENCL)
      SET(STARPU_USE_OPENCL ON)
    ENDIF()
  else()
    MESSAGE("Couldn't read StarPU config file")
  endif()
ENDIF(STARPU_FOUND)

# Deprecated declarations.
SET (NATIVE_STARPU_INCLUDE_PATH ${STARPU_INCLUDE_DIR} )
IF(STARPU_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_STARPU_LIB_PATH ${STARPU_LIBRARY} PATH)
ENDIF(STARPU_LIBRARY)

MARK_AS_ADVANCED(STARPU_LIBRARY STARPU_INCLUDE_DIR STARPU_LINK_DIRECTORIES)
