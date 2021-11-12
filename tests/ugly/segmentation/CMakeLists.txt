IF(USE_GNUPLOT_I)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/segmentation)

	ADD_EXECUTABLE(segmentation-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(segmentation-test armadillo-utils gnuplot_i)
	ADD_DEPENDENCIES(segmentation-test mediacycle mc_segmentation)

	TARGET_LINK_LIBRARIES(segmentation-test mediacycle mc_segmentation gnuplot_i)
        MESSAGE("[X] segmentation")
ELSE()
        MESSAGE("[ ] segmentation")
ENDIF()
