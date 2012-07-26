# - Try to find OpenCV library installation
# See http://sourceforge.net/projects/opencvlibrary/
#
# The following variables are optionally searched for defaults
#  OpenCV_ROOT_DIR:       Base directory of OpenCv tree to use.
#  OpenCV_FIND_COMPONENTS: FIND_PACKAGE(OpenCV COMPONENTS ..) 
#    compatible interface. typically  cv cxcore highgui .. etc.
#
# The following are set after configuration is done: 
#  OpenCV_FOUND
#  OpenCV_INCLUDE_DIR
#  OpenCV_LIBRARIES
#  OpenCV_LINK_DIRECTORIES
# 
# 2012/07 Christian Frisson: complete rework
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006/01 complete rewrite by Jan Woetzel
# 1006/09 2nd rewrite introducing ROOT_DIR and PATH_SUFFIXES 
#   to handle multiple installed versions gracefully by Jan Woetzel
#
# tested with:
# CF: OpenCV 2.4.2 mxe + OpenCV 2.3.1 OSX
#
# www.mip.informatik.uni-kiel.de/~jw
# --------------------------------

# Uncomment #MESSAGE... for debugging messages
MACRO(DBG_MSG _MSG)
	#MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}):\n${_MSG}")
ENDMACRO(DBG_MSG)

# required cv components with header and library if COMPONENTS unspecified
# possible: 1.x cv cxcore highgui + cvcam (windows)
# possible: 2.2.0): calib3d core contrib ffmpeg? features2D flann gpu haartraining imgproc legacy ml objdetect traincascade video
# possible: 2.3.0): androidcamera calib3d core contrib features2D flann gpu haartraining imgproc legacy ml objdetect stitching traincascade ts video
# possible: 2.4.2): androidcamera calib3d core contrib features2D flann gpu imgproc java? legacy ml objdetect nonfree photo stitching ts videostab video world 

DBG_MSG("OpenCV_FIND_COMPONENTS=${OpenCV_FIND_COMPONENTS}")
IF (NOT OpenCV_FIND_COMPONENTS)
  # default
  SET( OpenCV_FIND_COMPONENTS cv cxcore cvaux highgui )
  IF   (WIN32)
    LIST(APPEND  OpenCV_FIND_COMPONENTS cvcam ) # WIN32 only actually
  ENDIF(WIN32)  
ENDIF (NOT OpenCV_FIND_COMPONENTS)
DBG_MSG("OpenCV_FIND_COMPONENTS=${OpenCV_FIND_COMPONENTS}")

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
DBG_MSG("OpenCV_ROOT_DIR=${OpenCV_ROOT_DIR}")

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
DBG_MSG("OpenCV_LIBDIR_SUFFIXES=${OpenCV_LIBDIR_SUFFIXES}")

# Try to ascertain the version...
FIND_PATH(OpenCV_CV_VERSION_HPP_DIR
	NAMES core/version.hpp      
	PATHS ${OpenCV_ROOT_DIR} 
	PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
set(_OpenCV_Version_file "${OpenCV_CV_VERSION_HPP_DIR}/core/version.hpp")
DBG_MSG("OpenCV ${_OpenCV_Version_file}")
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

DBG_MSG("OpenCV version: ${_OpenCV_VERSION_MAJOR}.${_OpenCV_VERSION_MINOR}.${_OpenCV_VERSION_PATCH}")
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

IF(_OpenCV_VERSION_MAJOR GREATER 1)
	DBG_MSG("OpenCV removing potential 1.x components")
	DBG_MSG("OpenCV_FIND_COMPONENTS=${OpenCV_FIND_COMPONENTS}")
	list(REMOVE_ITEM OpenCV_FIND_COMPONENTS cv cxcore cvaux cvcam)
	DBG_MSG("OpenCV_FIND_COMPONENTS=${OpenCV_FIND_COMPONENTS}")
ENDIF()

FOREACH(NAME ${OpenCV_FIND_COMPONENTS} )
FIND_PATH(OpenCV_${NAME}_INCLUDE_DIR
  NAMES ${NAME}.h ${NAME}.hpp    
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} include/opencv2/${NAME})
DBG_MSG("component ${NAME} include dir ${OpenCV_${NAME}_INCLUDE_DIR}")
ENDFOREACH()

FOREACH(NAME ${OpenCV_FIND_COMPONENTS} )
	FIND_LIBRARY(OpenCV_${NAME}_LIBRARY
		NAMES ${NAME} opencv_${NAME} opencv_${NAME}${_OpenCV_VERSION_MAJOR}${_OpenCV_VERSION_MINOR}${_OpenCV_VERSION_PATCH} /
			 opencv_${NAME}.${_OpenCV_VERSION_MAJOR}.${_OpenCV_VERSION_MINOR}.${_OpenCV_VERSION_PATCH}
		PATHS ${OpenCV_ROOT_DIR}
	)
	DBG_MSG("component ${NAME} library ${OpenCV_${NAME}_LIBRARY}")

	# get the link directory for rpath to be used with LINK_DIRECTORIES: 
	GET_FILENAME_COMPONENT(OpenCV_${NAME}_LINK_DIRECTORY ${OpenCV_${NAME}_LIBRARY} PATH)
	SET(OpenCV_LINK_DIRECTORIES ${OpenCV_LINK_DIRECTORIES} ${OpenCV_${NAME}_LINK_DIRECTORY})
	DBG_MSG("component ${NAME} link directory ${OpenCV_${NAME}_LINK_DIRECTORY}")
ENDFOREACH()

#
# Logic selecting required libs and headers
#
SET(OpenCV_FOUND ON)
DBG_MSG(" OpenCV_FIND_COMPONENTS ${OpenCV_FIND_COMPONENTS}")
FOREACH(NAME ${OpenCV_FIND_COMPONENTS})
	# only good if header and library both found   
	IF (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
		LIST(APPEND OpenCV_INCLUDE_DIRS ${OpenCV_${NAME}_INCLUDE_DIR} )
		LIST(APPEND OpenCV_LIBRARIES    ${OpenCV_${NAME}_LIBRARY} )
    		#DBG_MSG("appending for NAME=${NAME} ${OpenCV_${NAME}_INCLUDE_DIR} and ${OpenCV_${NAME}_LIBRARY}" )
	ELSE (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
    		DBG_MSG("OpenCV component NAME=${NAME} not found! "
      			"\nOpenCV_${NAME}_INCLUDE_DIR=${OpenCV_${NAME}_INCLUDE_DIR} "
      			"\nOpenCV_${NAME}_LIBRARY=${OpenCV_${NAME}_LIBRARY} ")
		LIST(APPEND FAILED_COMPONENTS ${NAME} )
    		SET(OpenCV_FOUND OFF)
  	ENDIF(OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
ENDFOREACH(NAME)

DBG_MSG("OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS}")
DBG_MSG("OpenCV_LIBRARIES=${OpenCV_LIBRARIES}")

MARK_AS_ADVANCED(
  OpenCV_ROOT_DIR
  OpenCV_INCLUDE_DIRS
  OpenCV_LIBRARIES
)

FOREACH(NAME ${OpenCV_FIND_COMPONENTS})
	MARK_AS_ADVANCED(
		OpenCV_${NAME}_INCLUDE_DIR
		OpenCV_${NAME}_LIBRARY
	)
ENDFOREACH(NAME)


# be backward compatible:
SET(OPENCV_LIBRARIES   ${OpenCV_LIBRARIES} )
SET(OPENCV_INCLUDE_DIR ${OpenCV_INCLUDE_DIRS} )
SET(OPENCV_FOUND       ${OpenCV_FOUND})

# display help message
IF(NOT OpenCV_FOUND)
	# make FIND_PACKAGE friendly
	IF(NOT OpenCV_FIND_QUIETLY)
		IF(OpenCV_FIND_REQUIRED)
			MESSAGE(FATAL_ERROR "OpenCV required but some headers or libs not found (${FAILED_COMPONENTS}).")
		ELSE(OpenCV_FIND_REQUIRED)
			MESSAGE(STATUS 
			"ERROR: OpenCV was not found.")
		ENDIF(OpenCV_FIND_REQUIRED)
	ENDIF(NOT OpenCV_FIND_QUIETLY)
ENDIF(NOT OpenCV_FOUND)