SET(TARGET_NAME "segmentation-cli")
IF (SUPPORT_AUDIO AND BUILD_CLIS AND (USE_AUDIOFEAT OR USE_YAAFE))

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	FILE(GLOB PLUGIN_DIRS ${CMAKE_SOURCE_DIR}/plugins/*)
	FOREACH(PLUGIN_DIR ${PLUGIN_DIRS})
   		IF(IS_DIRECTORY ${PLUGIN_DIR})
        		INCLUDE_DIRECTORIES(${PLUGIN_DIR})
    		ENDIF()
	ENDFOREACH()

	ADD_EXECUTABLE(segmentation-cli ${SRC} ${HDR})

	ADD_DEPENDENCIES(segmentation-cli mediacycle armadillo-utils  mc_audio mc_segmentation)
	IF(USE_AUDIOFEAT)
		ADD_DEPENDENCIES(segmentation-cli audiofeatures)
		TARGET_LINK_LIBRARIES(segmentation-cli audiofeatures ${SAMPLERATE_LIBRARY})

	ENDIF()
	TARGET_LINK_LIBRARIES(segmentation-cli mediacycle mc_segmentation ${SNDFILE_LIBRARY})


	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
