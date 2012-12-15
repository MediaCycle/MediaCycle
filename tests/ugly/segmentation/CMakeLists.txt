IF(SUPPORT_AUDIO OR SUPPORT_VIDEO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/gnuplot_i)

	ADD_EXECUTABLE(segmentation-test ${SRC} ${HDR})

        ADD_DEPENDENCIES(segmentation-test armadillo-utils)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/segmentation)
        ADD_DEPENDENCIES(segmentation-test mediacycle mc_segmentation)
        TARGET_LINK_LIBRARIES(segmentation-test mediacycle mc_segmentation)

        IF(SUPPORT_AUDIO)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/audio-segmentation)
        ADD_DEPENDENCIES(segmentation-test mc_audio-segmentation)
        TARGET_LINK_LIBRARIES(segmentation-test mc_audio-segmentation)
        ENDIF()
        IF(SUPPORT_VIDEO)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/video-segmentation)
        ADD_DEPENDENCIES(segmentation-test mc_video-segmentation)
        TARGET_LINK_LIBRARIES(segmentation-test mc_video-segmentation)
        ENDIF()
	SET_TARGET_PROPERTIES("segmentation-test" PROPERTIES FOLDER "${FOLDERNAME}")

        MESSAGE("[X] segmentation")
ELSE()
        MESSAGE("[ ] segmentation")
ENDIF()
