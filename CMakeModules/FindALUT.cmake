# Locate ALUT
# This module defines
# ALUT_LIBRARY
# ALUT_FOUND, if false, do not try to link to gdal 
# ALUT_INCLUDE_DIR, where to find the headers
#
#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2011 – UMONS - Numediart
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


MESSAGE ( "Trying to find ALUT" )

FIND_PATH(ALUT_INCLUDE_DIR alut.h
  HINTS
  $ENV{ALUTDIR}
  PATH_SUFFIXES include/AL include/OpenAL include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
)

FIND_LIBRARY(ALUT_LIBRARY 
  NAMES alut
  HINTS
  $ENV{ALUTDIR}
  PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
)


SET(ALUT_FOUND "NO")
IF(ALUT_LIBRARY AND ALUT_INCLUDE_DIR)
  SET(ALUT_FOUND "YES")
ENDIF(ALUT_LIBRARY AND ALUT_INCLUDE_DIR)

