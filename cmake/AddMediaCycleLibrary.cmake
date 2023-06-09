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

# Optional variables to be set:
#  MC_3RDPARTIES (list): example SET(MC_3RDPARTIES "cvBlobsLib;cvgabor")
#  MC_MEDIA_LIBS (list): example SET(MC_MEDIA_LIBS "image-core;image-features"), without their prefix mediacycle-

macro(ADD_MC_LIBRARY LIBRARY_NAME)

SET(TARGET_NAME "${LIBRARY_PREFIX}${LIBRARY_NAME}")
SET(MEDIA_TYPE "${LIBRARY_NAME}")

FILE(GLOB ${TARGET_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)
FILE(GLOB ${TARGET_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
IF(WITH_QT)
    #MESSAGE("Qt4-powered MediaCycle library")
    FILE(GLOB ${TARGET_NAME}_UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui ${CMAKE_CURRENT_BINARY_DIR}/*.ui)
    # Generates ui_*.h files
    IF(USE_QT4)
        QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
    ELSEIF(USE_QT5)
        QT5_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
    ENDIF()

    # Generates moc_*.cxx files
    IF(USE_QT4)
        QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDR} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed
    ELSEIF(USE_QT5)
        QT5_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDR} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed
    ENDIF()

    # Don't forget to include output directory, otherwise
    # the UI file won't be wrapped!
    include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/libs/gui-osg-qt)

    ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRC} ${${TARGET_NAME}_HDR} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS})
ELSE()
    ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRC} ${${TARGET_NAME}_HDR})
ENDIF()

foreach(MC_3RDPARTY ${MC_3RDPARTIES})
    IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY}")
        MESSAGE(FATAL_ERROR "${MC_3RDPARTY} is not part of the MediaCycle 3rd parties in ${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY}")
    ENDIF()
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY})
    ADD_DEPENDENCIES(${TARGET_NAME} ${MC_3RDPARTY})
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${MC_3RDPARTY})
endforeach(MC_3RDPARTY)
foreach(MC_MEDIA_LIB ${MC_MEDIA_LIBS})
    STRING(REGEX REPLACE "mediacycle-" "" MC_MEDIA_LIB "${MC_MEDIA_LIB}")
    IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB}")
        MESSAGE(FATAL_ERROR "mediacycle-${MC_MEDIA_LIB} is not part of the MediaCycle media libs in ${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB}")
    ENDIF()
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB})
    ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-${MC_MEDIA_LIB})
    TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-${MC_MEDIA_LIB})
endforeach(MC_MEDIA_LIB)

IF(NOT MC_3RDPARTY AND NOT MC_MEDIA_LIBS)
    ADD_DEPENDENCIES(${TARGET_NAME} mediacycle)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle)
ENDIF()

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})
#SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES SUFFIX ${LIBRARY_SUFFIX})
#INSTALL(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})

endmacro(ADD_MC_LIBRARY LIBRARY_NAME)
