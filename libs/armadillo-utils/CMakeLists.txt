IF ( ARMADILLO_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(armadillo-utils STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(armadillo-utils fftooura)
	
	#gsl - uncomment if needed
	IF(WIN32 OR MINGW)
		TARGET_LINK_LIBRARIES(armadillo-utils fftooura ${ARMADILLO_LIBRARY} ${ATLAS_LIBRARY} ${LAPACK_LIBRARY} ${BLAS_LIBRARY})
	ELSE()
		TARGET_LINK_LIBRARIES(armadillo-utils fftooura ${ARMADILLO_LIBRARY} atlas lapack blas)
	ENDIF()
	#IF ( SDIF_FOUND )
	#	TARGET_LINK_LIBRARIES(armadillo-utils sdif)
	#ENDIF ( )

	TARGET_LINK_LIBRARIES(armadillo-utils fftooura ${ARMADILLO_LIBRARY})

	#INSTALL(TARGETS armadillo-utils LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/armadillo-utils)
	MESSAGE("[X] armadillo-utils")
ELSE()
	MESSAGE("[ ] armadillo-utils")
ENDIF()

