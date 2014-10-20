SET(TARGET_NAME "Videodrome")
IF ( BUILD_VIDEOCYCLE_APP AND SUPPORT_VIDEO AND (USE_QT4 OR USE_QT5) AND USE_OSG AND USE_OSC AND USE_FFMPEG )

	SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC ON)
	SET(ICON_NAME "Videodrome")

	SET(MEDIA_TYPE "MEDIA_TYPE_VIDEO")
        SET(MC_PLUGINS "file;video-reader-opencv;video-osg;video-thumbnailer-resize;video-thumbnailer-slitscan-opencv;visualisation")
        SET(MC_PLUGINS_STATIC ON)
        SET(OSG_PLUGINS "osgdb_ffmpeg")
        SET(CLUSTERS_POSITIONS "MediaCycle ScatterPlot")
        SET(DESCRIPTION "Videodrome allows to browse video files quickly and compute their slit-scans.")

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
	IF(NOT USE_DEBUG)
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
