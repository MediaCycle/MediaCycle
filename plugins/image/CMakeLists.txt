FILE(GLOB SRC *.cpp *.c ./blobs/*.cpp ./gabor/*.cpp)
FILE(GLOB HDR *.hpp *.h ./blobs/*.h ./gabor/*.h)

SET(PLUGIN_NAME "image")

ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SRC} ${HDR})
ADD_DEPENDENCIES(${PLUGIN_NAME} mediacycle)

INCLUDE_DIRECTORIES(. ./blobs ./gabor)
TARGET_LINK_LIBRARIES(${PLUGIN_NAME} mediacycle)

#OpenCV
IF ( USE_OPENCV )
    IF ( UNIX )
        TARGET_LINK_LIBRARIES(${PLUGIN_NAME} ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY})
    ELSE ( UNIX )
        TARGET_LINK_LIBRARIES(${PLUGIN_NAME} cv cxcore highgui)
    ENDIF ( UNIX )
ENDIF ( USE_OPENCV )

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
TARGET_LINK_LIBRARIES(${PLUGIN_NAME} fftw3)

INSTALL(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})
