SET(PLUGIN_NAME "io-apple-trackpad-autoplay")
SET(MC_LIBS "io-applemultitouch")
SET(MC_MEDIA_LIBS "")
SET(WITH_QT ON)
IF((USE_QT4 OR USE_QT5) AND APPLE AND USE_APPLE_MULTITOUCH)
	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
	ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${PLUGIN_PREFIX}${PLUGIN_NAME}")
ENDIF()
