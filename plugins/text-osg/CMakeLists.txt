SET(MEDIA_TYPE "text")
SET(PLUGIN_NAME "text-osg")
SET(MC_MEDIA_LIBS "${MEDIA_TYPE}-core;${MEDIA_TYPE}-osg")
IF(SUPPORT_TEXT AND BUILD_APPS AND USE_OSG AND OSG_FOUND)
	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
	ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
	MESSAGE("[X] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ELSE()
	MESSAGE("[ ] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ENDIF()
