SET(TARGET_NAME "TestBuildXML")
IF ( BUILD_CLIS AND SUPPORT_AUDIO )

	SET(WITH_SEGMENTATION OFF)
        SET(WITH_FFTW3 ON)
	#SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-features-armadillo;audio-reader-sndfile;visualisation")
        SET(DESCRIPTION "TestBuildXML is a test code to creat XML library file from plugins.")
        SET(MAIN_CLASS "TestBuildXML")

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleCLI.cmake)
	ADD_MC_CLI_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} ...)
	# Link these dependences:
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${FFTW3_LIBRARY} ${SNDFILE_LIBRARIES})
	
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
