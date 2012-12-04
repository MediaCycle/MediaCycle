IF(SUPPORT_IMAGE)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/image-core)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/image-features)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/cvBlobsLib)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/cvgabor)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/gnuplot_i)

	ADD_EXECUTABLE(image-test ${SRC} ${HDR})
	ADD_DEPENDENCIES(image-test mediacycle mc_image)
	TARGET_LINK_LIBRARIES(image-test mediacycle mc_image)

	MESSAGE("[X] image")
ELSE()
	MESSAGE("[ ] image")
ENDIF()
