SET(MEDIA_TYPE "video")
SET(PLUGIN_NAME "video-osg")
IF(FFMPEG_FOUND)
	SET(OSG_PLUGINS "osgdb_ffmpeg")
ENDIF()
SET(MC_MEDIA_LIBS "${MEDIA_TYPE}-core;${MEDIA_TYPE}-osg")
list(APPEND 3RDPARTY_NAMES "OpenCV")
list(APPEND 3RDPARTY_LICENSES "BSD")
list(APPEND 3RDPARTY_URLS "http://opencv.willowgarage.com")
IF(SUPPORT_VIDEO AND OpenCV_FOUND AND BUILD_APPS AND USE_OSG AND OSG_FOUND)
	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
	ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
	MESSAGE("[X] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ELSE()
	MESSAGE("[ ] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ENDIF()
