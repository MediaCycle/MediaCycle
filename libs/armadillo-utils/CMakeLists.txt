IF ( ARMADILLO_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(armadillo-utils ${SRC} ${HDR})
	ADD_DEPENDENCIES(armadillo-utils fftooura)
	
	#gsl - uncomment if needed
	TARGET_LINK_LIBRARIES(armadillo-utils fftooura ${ARMADILLO_LIBRARIES})
	#IF ( SDIF_FOUND )
	#	TARGET_LINK_LIBRARIES(armadillo-utils sdif)
	#ENDIF ( )

	#INSTALL(TARGETS armadillo-utils LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/armadillo-utils)
	SET_TARGET_PROPERTIES("armadillo-utils" PROPERTIES FOLDER "libs")
    
	MESSAGE("${FOLDERNAME}")
	MESSAGE("[X] armadillo-utils")
ELSE()
	MESSAGE("[ ] armadillo-utils")
ENDIF()

