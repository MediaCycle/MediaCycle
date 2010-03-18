# Locate openCV
# This module defines
# OPENCV_LIBRARY
# OPENCV_FOUND, if false, do not try to link to gdal 
# OPENCV_INCLUDE_DIR, where to find the headers
#
# $OPENCV_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OPENCV_DIR
#
# Created by Robert Osfield. 

MESSAGE ( "Trying to find OpenCV" )

# required cv components with header and library if COMPONENTS unspecified
IF    (NOT OPENCV_FIND_COMPONENTS)
  # default
  SET(OPENCV_FIND_REQUIRED_COMPONENTS   CV CXCORE CVAUX )
  IF   (WIN32)
    LIST(APPEND OPENCV_FIND_REQUIRED_COMPONENTS  CVCAM ) # WIN32 only actually
  ENDIF(WIN32)  
ENDIF (NOT OPENCV_FIND_COMPONENTS)

FIND_PATH(OPENCV_INCLUDE_DIR
	NAMES "opencv/cv.h"
    $ENV{OPENCV_DIR}/include
    $ENV{OPENCV_DIR}
    $ENV{OPENCVDIR}/include
    $ENV{OPENCVDIR}
    $ENV{OPENCV_ROOT}/include
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    #[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

#MESSAGE ( "OpenCV include dir " ${OPENCV_INCLUDE_DIR} )

MACRO(FIND_OPENCV_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATHS
        $ENV{OPENCV_DIR}/lib
        $ENV{OPENCV_DIR}
        $ENV{OPENCVDIR}/lib
        $ENV{OPENCVDIR}
        $ENV{OPENCV_ROOT}/lib
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        #[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
        /usr/freeware/lib64
    )
 	
#MESSAGE ( "OpenCV lib: " ${MYLIBRARYNAME} )

ENDMACRO(FIND_OPENCV_LIBRARY LIBRARY LIBRARYNAME)

FIND_OPENCV_LIBRARY(OPENCV_CV_LIBRARY cv)
FIND_OPENCV_LIBRARY(OPENCV_CVAUX_LIBRARY cvaux)
FIND_OPENCV_LIBRARY(OPENCV_CXCORE_LIBRARY cxcore)
FIND_OPENCV_LIBRARY(OPENCV_HIGHGUI_LIBRARY highgui)

FOREACH(NAME ${OPENCV_FIND_REQUIRED_COMPONENTS} )

  # only good if header and library both found   
  IF    (OPENCV_${NAME}_LIBRARY)
    LIST(APPEND OPENCV_LIBRARIES    ${OPENCV_${NAME}_LIBRARY} )
  ELSE  (OPENCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
    #DBG_MSG("OPENCV component NAME=${NAME} not found! "
     # "\nOPENCV_${NAME}_LIBRARY=${OpenCV_${NAME}_LIBRARY} ")
  ENDIF (OPENCV_${NAME}_LIBRARY)
  
ENDFOREACH(NAME)

SET(OPENCV_FOUND "NO")
IF(OPENCV_LIBRARIES AND OPENCV_INCLUDE_DIR)
    SET(OPENCV_FOUND "YES")
ENDIF(OPENCV_LIBRARIES AND OPENCV_INCLUDE_DIR)
