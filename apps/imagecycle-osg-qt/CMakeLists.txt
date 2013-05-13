SET(TARGET_NAME "ImageCycle")
IF ( BUILD_IMAGECYCLE_APP AND SUPPORT_IMAGE AND USE_QT4 AND USE_OSG AND USE_OSC )

	SET(MEDIA_TYPE "MEDIA_TYPE_IMAGE")
	SET(MC_PLUGINS "file;image;image-osg;pcapreprocess;visualisation;visualisation-advanced;visualisation-tagging")
        IF(APPLE)
            SET(OSG_PLUGINS "osgdb_imageio")
        ELSE()
            SET(OSG_PLUGINS "osgdb_bmp;osgdb_jpeg;osgdb_png")#;osgdb_tiff
        ENDIF()
        SET(DESCRIPTION "ImageCycle allows to organize image files by similarity.")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} )
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} )
	
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
