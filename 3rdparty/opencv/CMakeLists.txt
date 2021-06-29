#CF this is a utility header for cross-platform including opencv
SET(TARGET_NAME "opencv-headers")
IF(OPENCV_FOUND)
    add_library(${TARGET_NAME} INTERFACE) 
    target_include_directories(${TARGET_NAME} INTERFACE .)
    MESSAGE("[x] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()