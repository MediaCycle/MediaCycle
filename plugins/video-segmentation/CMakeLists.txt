SET(MEDIA_TYPE "video")
SET(PLUGIN_NAME "video-segmentation")
SET(MC_LIBS "segmentation")
SET(MC_MEDIA_LIBS "${MEDIA_TYPE}-core")
IF(SUPPORT_VIDEO AND OpenCV_FOUND)
	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
	ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
	MESSAGE("[X] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ELSE()
	MESSAGE("[ ] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ENDIF()