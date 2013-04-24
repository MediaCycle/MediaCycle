SET(TARGET_NAME "VideoCycleBrowserShowdown")
IF ( BUILD_VIDEOCYCLE_APP AND SUPPORT_VIDEO AND USE_QT4 AND USE_OSG AND USE_FFMPEG) # AND USE_OSC )

	SET(WITH_SEGMENTATION ON)
        SET(WITHOUT_NEIGHBORS ON)
	SET(WITH_OSC OFF)
	SET(FULLSCREEN OFF)

	SET(MEDIA_TYPE "MEDIA_TYPE_VIDEO")
        SET(MC_PLUGINS "file;segmentation;video-reader-opencv;video-osg;video-segmentation;video-thumbnailer-resize;video-thumbnailer-slitscan-ffmpeg;visualisation;video-browser-showdown")
        #video;io-osc;pcapreprocess;video-thumbnailer-slitscan-ffmpeg;video-thumbnailer-slitscan-opencv
	SET(CLUSTERS_METHOD "MediaCycle Child KMeans")
        SET(OSG_PLUGINS "osgdb_ffmpeg")
	SET(MAIN_CLASS "ACVideoBrowserShowdownOsgQt")
        SET(DESCRIPTION "${TARGET_NAME} is the VideoCycle variant application for the Video Browser Showdown challenge, allowing to perform Known-Item Search tasks in video segments on a timeline and organized by similarity.")

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
