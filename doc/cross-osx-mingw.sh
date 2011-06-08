# Cross-compiling requires CMake 2.6 or newer. To cross-compile, first modify
# this file to set the proper settings and paths. Then use it from build/ like:
# cmake ../.. -DCMAKE_TOOLCHAIN_FILE=../../doc/doc/cross-mingw.sh /
#          -DCMAKE_INSTALL_PREFIX=/opt/mingw-cross-env/usr/i686-pc-mingw32
SET(MINGW_CROSS_ENV ${CMAKE_INSTALL_PREFIX}/../..)
SET(MINGW_PREFIX ${CMAKE_INSTALL_PREFIX})
set(MINGW_CROSS_BIN ${CMAKE_INSTALL_PREFIX}/../bin)

# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)
SET(BUILD_SHARED_LIBS OFF)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER ${MINGW_CROSS_BIN}/i686-pc-mingw32-gcc)
SET(CMAKE_RC_COMPILER ${MINGW_CROSS_BIN}/i686-pc-mingw32-rcc)
SET(CMAKE_CXX_COMPILER ${MINGW_CROSS_BIN}/i686-pc-mingw32-g++)
SET(CMAKE_Fortran_COMPILER ${MINGW_CROSS_BIN}/i686-pc-mingw32-gfortran)
SET(CMAKE_RANLIB ${MINGW_CROSS_BIN}/i686-pc-mingw32-ranlib)
SET(CMAKE_AR ${MINGW_CROSS_BIN}/i686-pc-mingw32-ar CACHE STRING "AR")

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH ${MINGW_PREFIX})
MESSAGE ("Find root path: ${CMAKE_FIND_ROOT_PATH}")

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# FindQt4.cmake querys qmake to get information:
set(QT_BINARY_DIR   ${MINGW_PREFIX}/bin)
set(QT_LIBRARY_DIR  ${MINGW_PREFIX}/lib)
set(QT_QTCORE_INCLUDE_DIR ${MINGW_PREFIX}/include/QtCore)
set(QT_QTGUI_INCLUDE_DIR ${MINGW_PREFIX}/include/QtGui)
set(QT_QTOPENGL_INCLUDE_DIR ${MINGW_PREFIX}/include/QtOpenGL)
set(QT_MKSPECS_DIR  ${MINGW_PREFIX}/mkspecs)
set(QT_MOC_EXECUTABLE  ${MINGW_CROSS_BIN}/i686-pc-mingw32-moc)
set(QT_QMAKE_EXECUTABLE ${MINGW_CROSS_BIN}/i686-pc-mingw32-qmake)
set(QT_UIC_EXECUTABLE  ${MINGW_CROSS_BIN}/i686-pc-mingw32-uic)
set(QT_RCC_EXECUTABLE  ${MINGW_CROSS_BIN}/i686-pc-mingw32-rcc)
set(QT_RC_EXECUTABLE  ${MINGW_CROSS_BIN}/i686-pc-mingw32-windres)

set(OpenCV_ROOT_DIR ${MINGW_PREFIX})

# Tell pkg-config not to look at the target environment's .pc files.
# Setting PKG_CONFIG_LIBDIR sets the default search directory, but we have to
# set PKG_CONFIG_PATH as well to prevent pkg-config falling back to the host's
# path.
SET(PKG_CONFIG_EXECUTABLE ${MINGW_CROSS_BIN}/i686-pc-mingw32-pkg-config)
set(ENV{PKG_CONFIG_LIBDIR} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
set(INSTALL_PREFIX ${CMAKE_FIND_ROOT_PATH})
set(ENV{MINGDIR} ${CMAKE_FIND_ROOT_PATH})