# XS TODO: check that gnuplot is intalled on the machine...
IF(USE_GNUPLOT_I)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(gnuplot_i STATIC ${SRC} ${HDR})

	#INSTALL(TARGETS gnuplot_i LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/gnuplot_i)
	MESSAGE("[X] gnuplot_i")
ELSE()
	MESSAGE("[ ] gnuplot_i")
ENDIF()