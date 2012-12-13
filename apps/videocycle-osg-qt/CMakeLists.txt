SET(TARGET_NAME "VideoCycle")
IF ( BUILD_VIDEOCYCLE_APP AND SUPPORT_VIDEO AND USE_QT4 AND USE_OSG AND USE_OSC )

	SET(WITH_SEGMENTATION ON)
	SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_VIDEO")
        SET(MC_PLUGINS "file;io-osc;pcapreprocess;segmentation;video;video-osg;video-segmentation;visualisation")
	SET(OSG_PLUGINS "osgdb_ffmpeg")
        SET(DESCRIPTION "VideoCycle allows to visually organize video files by similarity.")

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} ...)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} ...)
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
