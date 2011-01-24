IF ( ARMADILLO_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(armadillo-utils STATIC ${SRC} ${HDR})

	INCLUDE_DIRECTORIES(../fftooura)
	ADD_DEPENDENCIES(mediacycle fftooura)
	
	#gsl - uncomment if needed
	##TARGET_LINK_LIBRARIES(armadillo-utils mediacycle fftooura armadillo atlas lapack blas)
	TARGET_LINK_LIBRARIES(armadillo-utils fftooura armadillo atlas lapack blas)
	#IF ( SDIF_FOUND )
	#	TARGET_LINK_LIBRARIES(armadillo-utils sdif)
	#ENDIF ( )

	TARGET_LINK_LIBRARIES(armadillo-utils ${ARMADILLO_LIBRARY})
	INCLUDE_DIRECTORIES(. ../fftooura )#../core)

	#INSTALL(TARGETS armadillo-utils LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/armadillo-utils)

ELSE ( ARMADILLO_FOUND )
	MESSAGE ("Not building armadillo-utils: armadillo is required.")
ENDIF ( ARMADILLO_FOUND )

