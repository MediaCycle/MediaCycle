# - Try to find OpenCV library installation
# See http://sourceforge.net/projects/opencvlibrary/
#
# The follwoing variables are optionally searched for defaults
#  OpenCV_ROOT_DIR:            Base directory of OpenCv tree to use.
#  OpenCV_FIND_REQUIRED_COMPONENTS : FIND_PACKAGE(OpenCV COMPONENTS ..) 
#    compatible interface. typically  CV CXCORE CVAUX HIGHGUI CVCAM .. etc.
#
# The following are set after configuration is done: 
#  OpenCV_FOUND
#  OpenCV_INCLUDE_DIR
#  OpenCV_LIBRARIES
#  OpenCV_LINK_DIRECTORIES
#
# deprecated:
#  OPENCV_* uppercase replaced by case sensitive OpenCV_*
#  OPENCV_EXE_LINKER_FLAGS
#  OPENCV_INCLUDE_DIR : replaced by plural *_DIRS
# 
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006/01 complete rewrite by Jan Woetzel
# 1006/09 2nd rewrite introducing ROOT_DIR and PATH_SUFFIXES 
#   to handle multiple installed versions gracefully by Jan Woetzel
#
# tested with:
# -OpenCV 0.97 (beta5a):  MSVS 7.1, gcc 3.3, gcc 4.1
# -OpenCV 0.99 (1.0rc1):  MSVS 7.1
#
# www.mip.informatik.uni-kiel.de/~jw
# --------------------------------


MACRO(DBG_MSG _MSG)
  #  MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}):\n${_MSG}")
ENDMACRO(DBG_MSG)



# required cv components with header and library if COMPONENTS unspecified
IF    (NOT OpenCV_FIND_COMPONENTS)
  # default
  SET(OpenCV_FIND_REQUIRED_COMPONENTS CV CXCORE CVAUX HIGHGUI )
  IF   (WIN32)
    LIST(APPEND OpenCV_FIND_REQUIRED_COMPONENTS  CVCAM ) # WIN32 only actually
  ENDIF(WIN32)  
ENDIF (NOT OpenCV_FIND_COMPONENTS)


# typical root dirs of installations, exactly one of them is used
SET (OpenCV_POSSIBLE_ROOT_DIRS
  "${OpenCV_ROOT_DIR}"
  "$ENV{OpenCV_ROOT_DIR}"  
  "$ENV{OPENCV_DIR}"  # only for backward compatibility deprecated by ROOT_DIR
  "$ENV{OPENCV_HOME}" # only for backward compatibility
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]"
  "$ENV{ProgramFiles}/OpenCV"
  /usr/local
  /opt/local
  /usr
  )


# MIP Uni Kiel /opt/net network installation 
# get correct prefix for current gcc compiler version for gcc 3.x  4.x
IF    (${CMAKE_COMPILER_IS_GNUCXX})
  IF    (NOT OpenCV_FIND_QUIETLY)
    MESSAGE(STATUS "Checking GNUCXX version 3/4 to determine  OpenCV /opt/net/ path")
  ENDIF (NOT OpenCV_FIND_QUIETLY)
  EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE CXX_COMPILER_VERSION)  
  IF   (CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")
    SET(IS_GNUCXX3 TRUE)
    LIST(APPEND OpenCV_POSSIBLE_ROOT_DIRS /opt/net/gcc33/OpenCV )
  ENDIF(CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")  
  IF   (CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")
    SET(IS_GNUCXX4 TRUE)
    LIST(APPEND OpenCV_POSSIBLE_ROOT_DIRS /opt/net/gcc41/OpenCV )
  ENDIF(CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")
ENDIF (${CMAKE_COMPILER_IS_GNUCXX})

#DBG_MSG("DBG (OpenCV_POSSIBLE_ROOT_DIRS=${OpenCV_POSSIBLE_ROOT_DIRS}")

#
# select exactly ONE OpenCV base directory/tree 
# to avoid mixing different version headers and libs
#
FIND_PATH(OpenCV_ROOT_DIR 
  NAMES 
  cv/include/cv.h     # windows
  include/opencv/cv.h # linux /opt/net
  include/cv/cv.h 
  include/cv.h
  include/opencv/cv.h 
  PATHS ${OpenCV_POSSIBLE_ROOT_DIRS})
#DBG_MSG("OpenCV_ROOT_DIR=${OpenCV_ROOT_DIR}")


# header include dir suffixes appended to OpenCV_ROOT_DIR
SET(OpenCV_INCDIR_SUFFIXES
  include
  include/cv
  include/opencv
  include/opencv2
  cv/include
  cxcore/include
  cvaux/include
  otherlibs/cvcam/include
  otherlibs/highgui
  otherlibs/highgui/include
  otherlibs/_graphics/include
  )

# library linkdir suffixes appended to OpenCV_ROOT_DIR 
SET(OpenCV_LIBDIR_SUFFIXES
  lib
  OpenCV/lib
  otherlibs/_graphics/lib
  )
#DBG_MSG("OpenCV_LIBDIR_SUFFIXES=${OpenCV_LIBDIR_SUFFIXES}")


#
# find incdir for each lib
#
FIND_PATH(OpenCV_CV_INCLUDE_DIR
  NAMES cv.h      
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CXCORE_INCLUDE_DIR   
  NAMES cxcore.h
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CVAUX_INCLUDE_DIR    
  NAMES cvaux.h
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_HIGHGUI_INCLUDE_DIR  
  NAMES highgui.h 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CVCAM_INCLUDE_DIR    
  NAMES cvcam.h 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_IMGPROC_INCLUDE_DIR 
  NAMES imgproc/imgproc.hpp
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CALIB3D_INCLUDE_DIR 
  NAMES calib3d/calib3d.hpp
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_VIDEO_INCLUDE_DIR 
  NAMES video/tracking.hpp
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_FEATURES2D_INCLUDE_DIR 
  NAMES features2d/features2d.hpp 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_OBJDETECT_INCLUDE_DIR 
  NAMES objdetect/objdetect.hpp 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CONTRIB_INCLUDE_DIR 
  NAMES contrib/contrib.hpp
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_LEGACY_INCLUDE_DIR 
  NAMES legacy/legacy.hpp
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )

# Try to ascertain the version...
FIND_PATH(OpenCV_CV_VERSION_HPP_DIR
	NAMES core/version.hpp      
	PATHS ${OpenCV_ROOT_DIR} 
	PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
set(_OpenCV_Version_file "${OpenCV_CV_VERSION_HPP_DIR}/core/version.hpp")
MESSAGE("OpenCV ${_OpenCV_Version_file}")
if(EXISTS "${_OpenCV_Version_file}")
       	file(READ "${_OpenCV_Version_file}" _OpenCV_Version_contents)
	string(REGEX MATCH ".*#define CV_MAJOR_VERSION+    +[0-9]+.*" _OpenCV_new_version_defines "${_OpenCV_Version_contents}")
	if(_OpenCV_new_version_defines)
		string(REGEX REPLACE ".*#define CV_MAJOR_VERSION    +([0-9]+).*" "\\1" _OpenCV_VERSION_MAJOR ${_OpenCV_Version_contents})
      		string(REGEX REPLACE ".*#define CV_MINOR_VERSION    +([0-9]+).*" "\\1" _OpenCV_VERSION_MINOR ${_OpenCV_Version_contents})
       		string(REGEX REPLACE ".*#define CV_SUBMINOR_VERSION +([0-9]+).*" "\\1" _OpenCV_VERSION_PATCH ${_OpenCV_Version_contents})
	else()
       		message(FATAL_ERROR "[ FindOpenCV.cmake:${CMAKE_CURRENT_LIST_LINE} ] " "Failed to parse version number, please report this as a bug")
       	endif()
else()
      	set(_OpenCV_Version_contents "unknown")
	set(_OpenCV_VERSION_MAJOR "1")
	set(_OpenCV_VERSION_MINOR "0")
	set(_OpenCV_VERSION_PATCH "0")
endif()
set(OpenCV_VERSION "${_OpenCV_VERSION_MAJOR}.${_OpenCV_VERSION_MINOR}.${_OpenCV_VERSION_PATCH}" CACHE INTERNAL "The version of OpenCV which was detected")

if(OpenCV_FIND_VERSION AND OpenCV_VERSION)
    if(OpenCV_FIND_VERSION_EXACT)
        if(NOT OpenCV_VERSION VERSION_EQUAL ${OpenCV_FIND_VERSION})
            MESSAGE(FATAL_ERROR "OpenCV version found is ${OpenCV_VERSION}, while version needed is exactly ${OpenCV_FIND_VERSION}.")
        endif()
    else()
        # version is too low
        if(NOT OpenCV_VERSION VERSION_EQUAL ${OpenCV_FIND_VERSION} AND 
                NOT OpenCV_VERSION VERSION_GREATER ${OpenCV_FIND_VERSION})
            MESSAGE(FATAL_ERROR "OpenCV version found is ${OpenCV_VERSION}, while version needed is ${OpenCV_FIND_VERSION}.")
        endif()
    endif()
endif()



#
# find sbsolute path to all libraries 
# some are optionally, some may not exist on Linux
#
FIND_LIBRARY(OpenCV_CV_LIBRARY   
  NAMES cv opencv opencv_core opencv_legacy
  PATHS ${OpenCV_ROOT_DIR}  
  PATH_SUFFIXES  ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CVAUX_LIBRARY
  NAMES cvaux
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CVCAM_LIBRARY   
  NAMES cvcam
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} ) 
FIND_LIBRARY(OpenCV_CVHAARTRAINING_LIBRARY
  NAMES cvhaartraining
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} ) 
FIND_LIBRARY(OpenCV_CXCORE_LIBRARY  
  NAMES cxcore opencv_core
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CXTS_LIBRARY   
  NAMES cxts
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_HIGHGUI_LIBRARY  
  NAMES highgui opencv_highgui
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_ML_LIBRARY  
  NAMES ml opencv_ml
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_TRS_LIBRARY  
  NAMES trs
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
# missing flann gpu
FIND_LIBRARY(OpenCV_IMGPROC_LIBRARY  
  NAMES opencv_imgproc
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CALIB3D_LIBRARY  
  NAMES opencv_calib3d
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_VIDEO_LIBRARY  
  NAMES opencv_video
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_FEATURES2D_LIBRARY  
  NAMES opencv_features2d
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_OBJDETECT_LIBRARY  
  NAMES opencv_objdetect
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CONTRIB_LIBRARY  
  NAMES opencv_contrib
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_LEGACY_LIBRARY  
  NAMES opencv_legacy
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )

#
# Logic selecting required libs and headers
#
SET(OpenCV_FOUND ON)
DBG_MSG("OpenCV_FIND_REQUIRED_COMPONENTS=${OpenCV_FIND_REQUIRED_COMPONENTS}")
##FOREACH(NAME ${OpenCV_FIND_REQUIRED_COMPONENTS} )
FOREACH(NAME ${OpenCV_FIND_COMPONENTS} )

  # only good if header and library both found   
  IF    (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
    LIST(APPEND OpenCV_INCLUDE_DIRS ${OpenCV_${NAME}_INCLUDE_DIR} )
    LIST(APPEND OpenCV_LIBRARIES    ${OpenCV_${NAME}_LIBRARY} )
    #DBG_MSG("appending for NAME=${NAME} ${OpenCV_${NAME}_INCLUDE_DIR} and ${OpenCV_${NAME}_LIBRARY}" )
  ELSE  (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
    DBG_MSG("OpenCV component NAME=${NAME} not found! "
      "\nOpenCV_${NAME}_INCLUDE_DIR=${OpenCV_${NAME}_INCLUDE_DIR} "
      "\nOpenCV_${NAME}_LIBRARY=${OpenCV_${NAME}_LIBRARY} ")
    ##SET(OpenCV_FOUND OFF)
  ENDIF (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
  
ENDFOREACH(NAME)

DBG_MSG("OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS}")
DBG_MSG("OpenCV_LIBRARIES=${OpenCV_LIBRARIES}")

# get the link directory for rpath to be used with LINK_DIRECTORIES: 
IF    (OpenCV_CV_LIBRARY)
  GET_FILENAME_COMPONENT(OpenCV_LINK_DIRECTORIES ${OpenCV_CV_LIBRARY} PATH)
ENDIF (OpenCV_CV_LIBRARY)

MARK_AS_ADVANCED(
  OpenCV_ROOT_DIR
  OpenCV_INCLUDE_DIRS
  OpenCV_CV_INCLUDE_DIR
  OpenCV_CXCORE_INCLUDE_DIR
  OpenCV_CVAUX_INCLUDE_DIR
  OpenCV_CVCAM_INCLUDE_DIR
  OpenCV_HIGHGUI_INCLUDE_DIR
  OpenCV_LIBRARIES
  OpenCV_CV_LIBRARY
  OpenCV_CXCORE_LIBRARY
  OpenCV_CVAUX_LIBRARY
  OpenCV_CVCAM_LIBRARY
  OpenCV_CVHAARTRAINING_LIBRARY
  OpenCV_CXTS_LIBRARY
  OpenCV_HIGHGUI_LIBRARY
  OpenCV_ML_LIBRARY
  OpenCV_TRS_LIBRARY
  OpenCV_IMGPROC_LIBRARY
  OpenCV_CALIB3D_LIBRARY
  OpenCV_VIDEO_LIBRARY
  OpenCV_FEATURES2D_LIBRARY
  OpenCV_OBJDETECT_LIBRARY 
  OpenCV_CONTRIB_LIBRARY  
  OpenCV_LEGACY_LIBRARY
  )


# be backward compatible:
SET(OPENCV_LIBRARIES   ${OpenCV_LIBRARIES} )
SET(OPENCV_INCLUDE_DIR ${OpenCV_INCLUDE_DIRS} )
SET(OPENCV_FOUND       ${OpenCV_FOUND})



# display help message
IF(NOT OpenCV_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT OpenCV_FIND_QUIETLY)
    IF(OpenCV_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
        "OpenCV required but some headers or libs not found. Please specify it's location with OpenCV_ROOT_DIR env. variable.")
    ELSE(OpenCV_FIND_REQUIRED)
      MESSAGE(STATUS 
        "ERROR: OpenCV was not found.")
    ENDIF(OpenCV_FIND_REQUIRED)
  ENDIF(NOT OpenCV_FIND_QUIETLY)
ENDIF(NOT OpenCV_FOUND)
