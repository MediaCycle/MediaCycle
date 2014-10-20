SET(TARGET_NAME "ArchipelCycle")
IF ( SUPPORT_ARCHIPEL AND SUPPORT_MULTIMEDIA AND SUPPORT_AUDIO AND SUPPORT_IMAGE AND SUPPORT_TEXT AND (USE_QT4 OR USE_QT5) AND USE_OSG AND USE_OSC)

	SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_MIXED")
    SET(MC_PLUGINS "audio-engine;audio-features-yaafe;audio-osg;audio-reader-sndfile;file;image-features-color;image-features-contours;image-features-faces;image-features-shape;image-features-texture;image-osg;segmentation;text;text-osg;visualisation;visualisation-advanced;archipel") # load archipel after text plugin which also reads .xml
        SET(CLUSTERS_METHOD "MediaCycle KMeans")
        SET(CLUSTERS_POSITIONS "Archipel Atoll")
	SET(OSG_PLUGINS "")
	SET(MEDIA_READER "Archipel Reader")
	SET(ACTIVE_MEDIA "text")
        SET(DESCRIPTION "ArchipelCycle is an attempt to complement the Archipel database from La Mediatheque (Belgium) with the MediaCycle organization of media files by similarity.")

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
