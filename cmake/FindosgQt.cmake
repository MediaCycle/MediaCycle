# This is part of the Findosg* suite used to find OpenSceneGraph components.
# Each component is separate and you must opt in to each module. You must
# also opt into OpenGL and OpenThreads (and Producer if needed) as these
# modules won't do it for you. This is to allow you control over your own
# system piece by piece in case you need to opt out of certain components
# or change the Find behavior for a particular module (perhaps because the
# default FindOpenGL.cmake module doesn't work with your system as an
# example).
# If you want to use a more convenient module that includes everything,
# use the FindOpenSceneGraph.cmake instead of the Findosg*.cmake modules.
#
# Locate osgQt
# This module defines
#
# OSGQT_FOUND - Was osgQt found?
# OSGQT_INCLUDE_DIR - Where to find the headers
# OSGQT_LIBRARIES - The libraries to link for osgQt (use this)
#
# OSGQT_LIBRARY - The osgQt library
# OSGQT_LIBRARY_DEBUG - The osgQt debug library
#
# $OSGDIR is an environment variable that would
# correspond to the ./configure --prefix=$OSGDIR
# used in building osg.
#
# Created by Eric Wing.
# Modified to work with osgQt by Robert Osfield, January 2012.
# Modified to work with osgQOpenGL by Christian Frisson, 2021.

#=============================================================================
# Copyright 2007-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# Header files are presumed to be included like
# #include <osg/PositionAttitudeTransform>
# #include <osgQt/GraphicsWindowQt>

include(Findosg_functions)
OSG_FIND_PATH   (OSGQT osgQOpenGL/osgQOpenGLWidget)
OSG_FIND_LIBRARY(OSGQT osgQOpenGL)

#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(osgQt DEFAULT_MSG
    OSGQT_LIBRARY OSGQT_INCLUDE_DIR)
