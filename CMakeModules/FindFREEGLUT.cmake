# - Find FREEGLUT
# Find the native FREEGLUT includes and library
# This module defines
#  FREEGLUT_INCLUDE_DIR, where to find jpeglib.h, etc.
#  FREEGLUT_LIBRARIES, the libraries needed to use FREEGLUT.
#  FREEGLUT_FOUND, If false, do not try to use FREEGLUT.
# also defined, but not for general use are
#  FREEGLUT_LIBRARY, where to find the FREEGLUT library.

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

FIND_PATH(FREEGLUT_INCLUDE_DIR GL/glut.h OpenGL/glut.h glut.h)

SET(FREEGLUT_NAMES ${FREEGLUT_NAMES} freeglut)
FIND_LIBRARY(FREEGLUT_LIBRARY NAMES ${FREEGLUT_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set FREEGLUT_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FREEGLUT DEFAULT_MSG FREEGLUT_LIBRARY FREEGLUT_INCLUDE_DIR)

IF(FREEGLUT_FOUND)
  SET(FREEGLUT_LIBRARIES ${FREEGLUT_LIBRARY})
  GET_FILENAME_COMPONENT(FREEGLUT_LINK_DIRECTORIES ${FREEGLUT_LIBRARY} PATH)
ENDIF(FREEGLUT_FOUND)

# Deprecated declarations.
SET (NATIVE_FREEGLUT_INCLUDE_PATH ${FREEGLUT_INCLUDE_DIR} )
IF(FREEGLUT_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_FREEGLUT_LIB_PATH ${FREEGLUT_LIBRARY} PATH)
ENDIF(FREEGLUT_LIBRARY)

MARK_AS_ADVANCED(FREEGLUT_LIBRARY FREEGLUT_INCLUDE_DIR FREEGLUT_LINK_DIRECTORIES)
