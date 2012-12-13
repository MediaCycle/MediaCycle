SET(TARGET_NAME "NavimedCycle")
IF ( SUPPORT_NAVIMED AND SUPPORT_MULTIMEDIA AND SUPPORT_IMAGE AND SUPPORT_TEXT AND USE_QT4 AND USE_OSG )

	SET(MEDIA_TYPE "MEDIA_TYPE_MIXED")
	SET(MC_PLUGINS "file;image;image-osg;segmentation;sensor;sensor-osg;text;text-osg;video;video-osg;visualisation;navimed") # load navimed after text plugin which also reads .xml
	SET(OSG_PLUGINS "osgdb_ffmpeg")
	SET(MEDIA_READER "Navimed Reader")
	SET(ACTIVE_MEDIA "text")
	SET(CLUSTERS_METHOD "Navimed KMeans")
        SET(CLUSTERS_POSITIONS "Navimed Position Plugin")
        SET(DESCRIPTION "NavimedCycle is an attempt to complement the Navimed database from Polymedis (Belgium) with the MediaCycle organization of media files by similarity.")

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
