FILE(GLOB SRC *.cpp *.c ./blobs/*.cpp ./gabor/*.cpp)
FILE(GLOB HDR *.hpp *.h ./blobs/*.h ./gabor/*.h)

SET(PLUGIN_NAME "image")
SET(TARGET_NAME "${PLUGIN_PREFIX}${PLUGIN_NAME}")

ADD_LIBRARY(${TARGET_NAME} SHARED ${SRC} ${HDR})
ADD_DEPENDENCIES(${TARGET_NAME} mediacycle)

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES PREFIX ${PLUGIN_PREFIX})
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${PLUGIN_NAME})

INCLUDE_DIRECTORIES(. ../../core ./blobs ./gabor)
TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle)

#OpenCV
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${OpenCV_LIBRARIES} )

#FFTW

# XS : this is the path on y machine. I tried to use FIND_PACKAGE FFTW but it did not work : 
# --
# CMake Warning at CMakeLists.txt:114 (FIND_PACKAGE):
#  Could not find module FindFFTW.cmake or a configuration file for package
#  FFTW.

# TODO : get this FindFFTW.cmake and put it in CmakeModules

#  Adjust CMAKE_MODULE_PATH to find FindFFTW.cmake or set FFTW_DIR to the
#  directory containing a CMake configuration file for FFTW.  The file will
#  have one of the following names:

#    FFTWConfig.cmake
#    fftw-config.cmake

SET (FFTW_INCLUDE_DIR "/opt/local/include")
SET (FFTW_LIBRARY_DIR "/opt/local/lib")

INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${FFTW_LIBRARY_DIR} )

ADD_DEFINITIONS(-DUSE_FFTW)
TARGET_LINK_LIBRARIES(${TARGET_NAME} fftw3)

INSTALL(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})
