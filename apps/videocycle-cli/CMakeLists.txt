SET(TARGET_NAME "VideoCycleCLI")
IF ( BUILD_CLIS AND SUPPORT_VIDEO )

	SET(WITH_SEGMENTATION ON)
	#SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_VIDEO")
        SET(MC_PLUGINS "video-reader-opencv;video-thumbnailer-slitscan-opencv") #video-thumbnailer-resize
        #video;video-thumbnailer-resize;video-thumbnailer-slitscan-ffmpeg;video-features-dancers;video-segmentation
        SET(DESCRIPTION "VideoCycleCLI allows to index video files by similarity.")
        SET(MAIN_CLASS "ACImportProcess")

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleCLI.cmake)
	ADD_MC_CLI_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} ...)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} ...)
	
	# Packaging
	#SET(PROGNAME "${TARGET_NAME}")
	#IF(NOT USE_DEBUG)
	#	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	#ENDIF()
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
