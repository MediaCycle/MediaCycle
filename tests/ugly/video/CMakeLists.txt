IF(SUPPORT_VIDEO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	FILE(GLOB PLUGIN_DIRS ${CMAKE_SOURCE_DIR}/plugins/*)
	FOREACH(PLUGIN_DIR ${PLUGIN_DIRS})
   		IF(IS_DIRECTORY ${PLUGIN_DIR})
        		INCLUDE_DIRECTORIES(${PLUGIN_DIR})
    		ENDIF()
	ENDFOREACH()

    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/image ${CMAKE_SOURCE_DIR}/plugins/image/blobs ${CMAKE_SOURCE_DIR}/plugins/image/gabor ${CMAKE_SOURCE_DIR}/plugins/video)

	ADD_EXECUTABLE(video-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(video-test mediacycle ${PLUGIN_PREFIX}video)

	TARGET_LINK_LIBRARIES(video-test mediacycle ${PLUGIN_PREFIX}video)

	MESSAGE("[X] video")
ELSE()
	MESSAGE("[ ] video")
ENDIF()
