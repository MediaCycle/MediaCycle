IF(SUPPORT_IMAGE)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/image ${CMAKE_SOURCE_DIR}/plugins/image/blobs ${CMAKE_SOURCE_DIR}/plugins/image/gabor)

	ADD_EXECUTABLE(image-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(image-test mediacycle mc_image)

	TARGET_LINK_LIBRARIES(image-test mediacycle mc_image)

	MESSAGE("[X] image")
ELSE()
	MESSAGE("[ ] image")
ENDIF()
