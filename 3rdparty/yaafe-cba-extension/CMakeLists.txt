IF(SUPPORT_AUDIO AND USE_YAAFE)
#cmake_minimum_required (VERSION 2.6)
#project (yaafe CXX C)
SET(TARGET_NAME "yaafe-cba-extension")
add_custom_target(${TARGET_NAME})
include_directories("${CMAKE_SOURCE_DIR}/3rdparty/yaafe/src_cpp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src_cpp")#CF
IF(CMAKE_CAN_APPEND_PROPERTIES)
    set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/3rdparty/yaafe/src_cpp;${CMAKE_CURRENT_SOURCE_DIR}/src_cpp")
ENDIF()
#set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

# The version number.
set (YAAFE_VERSION_MAJOR 0)
set (YAAFE_VERSION_MINOR 1)
set (YAAFE_API_VERSION 0)

set (YAAFE_VERSION ${YAAFE_VERSION_MAJOR}.${YAAFE_VERSION_MINOR})
set (YAAFE_LIBVERSION ${YAAFE_VERSION}.${YAAFE_API_VERSION})

# Compiler FLAGS
if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -g2" CACHE STRING "C DEBUG FLAGS" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-90")
add_definitions(-Wno-write-strings)

#find_package(Argtable2 REQUIRED)
#find_package(DL REQUIRED)

#set(EIGEN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals/eigen)
#mark_as_advanced(FORCE EIGEN_INCLUDE_DIR) 
#set(SMARC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals/audio-smarc)
#mark_as_advanced(FORCE SMARC_INCLUDE_DIR)

# optional packages
option(WITH_SNDFILE "Use sndfile library" ON)
#find_package(Sndfile)
if (WITH_SNDFILE)
    if (NOT SNDFILE_FOUND)
        message(SEND_ERROR "Cannot find libsndfile library")
    endif (NOT SNDFILE_FOUND)
    add_definitions(-DWITH_SNDFILE=1)
endif (WITH_SNDFILE)



#FIND_PROGRAM(SPHINX_BUILD sphinx-build PATHS ENV PATH DOC "sphinx-build script used to build documentation")

# install paths
set(CMAKE_INSTALL_BINDIR bin CACHE PATH "Output directory for programs")
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
set(CMAKE_INSTALL_INCLUDE include CACHE PATH "Output directory for header files")
set(CMAKE_INSTALL_PYTHON_PACKAGES python_packages CACHE PATH "Output directory for yaafe Python packages")
set(CMAKE_INSTALL_YAAFE_EXTENSIONS yaafe_extensions CACHE PATH "Output directory for yaafe features libraries")

# add targets
add_definitions(-DVERSION=${YAAFE_VERSION})
add_subdirectory (src_cpp)
add_subdirectory (src_python)
#add_subdirectory (doc)

# export targets
#install(EXPORT yaafe-targets DESTINATION lib/yaafe)
#install(FILES yaafe-config.cmake DESTINATION lib/yaafe)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER")
set (CPACK_PACKAGE_VERSION_MAJOR "${YAAFE_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${YAAFE_VERSION_MINOR}")

SET_TARGET_PROPERTIES("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDERNAME}/${TARGET_NAME}")
SET_TARGET_PROPERTIES("cba-yaafe-extension" PROPERTIES FOLDER "${FOLDERNAME}/${TARGET_NAME}")

#include (CPack)
	MESSAGE("[X] yaafe-cba-extension")
ELSE()
	MESSAGE("[ ] yaafe-cba-extension")
ENDIF()
