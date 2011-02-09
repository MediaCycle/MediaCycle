	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(segmentation-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(segmentation-test armadillo-utils gnuplot_i)
	ADD_DEPENDENCIES(segmentation-test mediacycle mc_segmentation)
	INCLUDE_DIRECTORIES(../../plugins/segmentation ../../armadillo ../../fftooura ../../gnuplot_i ../../tinyxml)

	TARGET_LINK_LIBRARIES(segmentation-test mediacycle mc_segmentation gnuplot_i)
	INSTALL(TARGETS segmentation-test RUNTIME DESTINATION bin)
