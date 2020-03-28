SET(TARGET_NAME "GridNeighborsCLI")
SET(WITH_QT OFF)
IF ( BUILD_CLIS AND SUPPORT_AUDIO AND (USE_QT4 OR USE_QT5) ) # Qt for visualisation-s-hull

        SET(MC_PLUGINS_STATIC ON)

	SET(WITH_SEGMENTATION OFF)
	#SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-features-yaafe;audio-reader-sndfile;visualisation")
	LIST(APPEND MC_PLUGINS "visualisation-s-hull")
        IF(MLPACK_FOUND)
            IF(USE_DEBUG)
                LIST(APPEND MC_PLUGINS "visualisation-advanced-mlpack")
            ENDIF()
            LIST(APPEND MC_PLUGINS "visualisation-mlpack")
        ENDIF()
        IF(LIBDISPATCH_FOUND)
            LIST(APPEND MC_PLUGINS "visualisation-divvy")
        ENDIF()
        IF(NEWMAT_FOUND)
            LIST(APPEND MC_PLUGINS "visualisation-lineartimenerv;visualisation-dredviznerv")
        ENDIF()
        SET(DESCRIPTION "GridNeighborsCLI assesses how neighborhoods are preserved by proximity grids.")
        SET(MAIN_CLASS "ACGridNeighborsProcess")

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
