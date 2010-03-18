IF ( ARMADILLO_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	FILE(GLOB MC_SRC ../../core/*.cpp ../../core/*.c)
	FILE(GLOB MC_HDR ../../core/*.hpp ../../core/*.h)

	ADD_EXECUTABLE(mediacycle-dancers ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-dancers mediacycle)

	TARGET_LINK_LIBRARIES(mediacycle-dancers mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-dancers tinyxml armadillo-utils)

	INCLUDE_DIRECTORIES(. ../../tinyxml ../../armadillo ../../fftooura)

	INSTALL(TARGETS mediacycle-dancers RUNTIME DESTINATION bin)
ELSE ( ARMADILLO_FOUND )
	MESSAGE ("Not building DANCERS! app: armadillo with BLAS/LAPACK support required.")
ENDIF ( ARMADILLO_FOUND )

