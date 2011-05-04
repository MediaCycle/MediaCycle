IF(SUPPORT_VIDEO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/image/blobs ${CMAKE_SOURCE_DIR}/plugins/image/gabor)

	ADD_EXECUTABLE(video-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(video-test mediacycle ${PLUGIN_PREFIX}video)

	TARGET_LINK_LIBRARIES(video-test mediacycle ${PLUGIN_PREFIX}video)

	MESSAGE("[X] video")
ELSE()
	MESSAGE("[ ] video")
ENDIF()
