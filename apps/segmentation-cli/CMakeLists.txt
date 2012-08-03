SET(TARGET_NAME "segmentation-cli")
IF (SUPPORT_AUDIO AND USE_AUDIOFEAT AND BUILD_CLIS)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	FILE(GLOB PLUGIN_DIRS ${CMAKE_SOURCE_DIR}/plugins/*)
	FOREACH(PLUGIN_DIR ${PLUGIN_DIRS})
   		IF(IS_DIRECTORY ${PLUGIN_DIR})
        		INCLUDE_DIRECTORIES(${PLUGIN_DIR})
    		ENDIF()
	ENDFOREACH()

	ADD_EXECUTABLE(segmentation-cli ${SRC} ${HDR})

	ADD_DEPENDENCIES(segmentation-cli mediacycle armadillo-utils audiofeatures)
	ADD_DEPENDENCIES(segmentation-cli mc_audio mc_segmentation)

	TARGET_LINK_LIBRARIES(segmentation-cli mediacycle audiofeatures mc_segmentation ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
