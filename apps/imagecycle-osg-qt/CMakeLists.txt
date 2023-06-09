SET(TARGET_NAME "ImageCycle")
IF ( BUILD_IMAGECYCLE_APP AND SUPPORT_IMAGE AND (USE_QT4 OR USE_QT5) AND USE_OSG AND USE_OSC )

	SET(MEDIA_TYPE "MEDIA_TYPE_IMAGE")
        SET(MC_PLUGINS "file;image-features-color;image-features-contours;image-features-shape;image-features-texture;image-osg;pcapreprocess;visualisation;visualisation-advanced;visualisation-tagging")#image-features-faces;
        IF(MLPACK_FOUND)
            IF(USE_DEBUG)
                LIST(APPEND MC_PLUGINS "visualisation-advanced-mlpack")
            ENDIF()
            LIST(APPEND MC_PLUGINS "visualisation-mlpack")
        ENDIF()

        SET(MC_PLUGINS_STATIC ON)
        #IF(APPLE)
        #    SET(OSG_PLUGINS "osgdb_imageio")
        #ELSE()
            SET(OSG_PLUGINS "osgdb_bmp;osgdb_jpeg;osgdb_png")#;osgdb_tiff
        #ENDIF()
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
