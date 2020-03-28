SET(TARGET_NAME "ImageCycleHTML5")
IF ( BUILD_IMAGECYCLE_APP AND SUPPORT_IMAGE AND (USE_QT4 OR USE_QT5))

        SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC OFF)

        SET(MEDIA_TYPE "MEDIA_TYPE_IMAGE")
        SET(MEDIA_TYPE "MEDIA_TYPE_IMAGE")
        SET(MC_PLUGINS "file;image-reader-opencv;image-features-color;pcapreprocess;visualisation;visualisation-advanced;visualisation-tagging;library-json")#image-features-contours;image-features-shape;image-features-texture;image-features-faces;
        IF(MLPACK_FOUND)
            IF(USE_DEBUG)
                LIST(APPEND MC_PLUGINS "visualisation-advanced-mlpack")
            ENDIF()
            LIST(APPEND MC_PLUGINS "visualisation-mlpack")
        ENDIF()

        SET(LOCAL_LIBRARY_FILENAME "library.json")
        SET(LOCAL_LIBRARY_WRITER_PLUGIN_NAME "JSON export")
        SET(WEBPAGE_RELATIVE_SOURCEPATH "3rdparty/imagecycle-html5/index.html")

        SET(DESCRIPTION "ImageCycle allows to organize image files by similarity.")
        #SET(FILTERING "Proximity Grid")
        SET(CLUSTERS_POSITIONS "MediaCycle Import Grid")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleWebkitQtApp.cmake)
        ADD_MC_WEBKIT_QT_EXECUTABLE(${TARGET_NAME})

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
