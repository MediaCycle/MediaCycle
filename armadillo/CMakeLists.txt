IF ( ARMADILLO_FOUND AND SDIF_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(armadillo-utils SHARED ${SRC} ${HDR})

	#gsl - uncomment if needed
	TARGET_LINK_LIBRARIES(armadillo-utils mediacycle fftooura armadillo atlas lapack blas sdif)
	INCLUDE_DIRECTORIES(. ../fftooura ../core)

	INSTALL(TARGETS armadillo-utils LIBRARY DESTINATION lib)
	INSTALL(FILES ${HDR} DESTINATION include/armadillo-utils)
ELSE ( ARMADILLO_FOUND AND SDIF_FOUND )
	MESSAGE ("Not building armadillo-utils: armadillo and SDIF are required.")
ENDIF ( ARMADILLO_FOUND AND SDIF_FOUND )

