SET(TARGET_NAME "DancersCycleCLI")
SET(ICON_NAME "DancersCycle")
IF(BUILD_CLIS AND SUPPORT_VIDEO)
	#SET(WITH_SEGMENTATION ON)
	SET(MEDIA_TYPE "MEDIA_TYPE_VIDEO")

	SET(MC_PLUGINS "video-reader-opencv;video-features-dancers;visualisation-dancers")
	SET(DESCRIPTION "DancersCycleCLI is a server for retrieving video files by similarity for Bud Hybrid's Dancers! project.")
        #SET(MAIN_CLASS "ACImportProcess")
        SET(CLUSTERS_POSITIONS "MediaCycle DANCERS!")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleCLI.cmake)
	ADD_MC_CLI_EXECUTABLE(${TARGET_NAME})

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