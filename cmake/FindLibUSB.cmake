# - Find LibUSB
# Find the native LibUSB includes and library
# This module defines
#  LIBUSB_INCLUDE_DIR, where to find jpeglib.h, etc.
#  LIBUSB_LIBRARIES, the libraries needed to use LibUSB.
#  LIBUSB_FOUND, If false, do not try to use LibUSB.
# also defined, but not for general use are
#  LIBUSB_LIBRARY, where to find the LibUSB library.

#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2013 – UMONS - Numediart
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more inforLibUSBn.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(LIBUSB_INCLUDE_DIR libusb.h PATH_SUFFIXES libusb-1.0)

SET(LIBUSB_NAMES ${LIBUSB_NAMES} libusb-1.0.0 usb-1.0.0 libusb usb usb-1.0)
FIND_LIBRARY(LIBUSB_LIBRARY NAMES ${LIBUSB_NAMES} PATH_SUFFIXES i386-linux-gnu x86_64-linux-gnu)

# handle the QUIETLY and REQUIRED arguments and set LIBUSB_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibUSB DEFAULT_MSG LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)

IF(LIBUSB_FOUND)
  SET(LIBUSB_LIBRARIES ${LIBUSB_LIBRARY})
  GET_FILENAME_COMPONENT(LIBUSB_LINK_DIRECTORIES ${LIBUSB_LIBRARY} PATH)
ENDIF(LIBUSB_FOUND)

# Deprecated declarations.
SET (NATIVE_LIBUSB_INCLUDE_PATH ${LIBUSB_INCLUDE_DIR} )
IF(LIBUSB_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_LIBUSB_LIB_PATH ${LIBUSB_LIBRARY} PATH)
ENDIF(LIBUSB_LIBRARY)

MARK_AS_ADVANCED(LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR LIBUSB_LINK_DIRECTORIES)
