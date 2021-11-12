IF ( ARMADILLO_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(armadillo-isomap ${SRC} ${HDR})
	#ADD_DEPENDENCIES(armadillo-isomap fftooura)
	
	TARGET_LINK_LIBRARIES(armadillo-isomap ${ARMADILLO_LIBRARIES})
	ADD_DEPENDENCIES(armadillo-isomap  armadillo-utils)
	TARGET_LINK_LIBRARIES(armadillo-isomap  armadillo-utils)

	MESSAGE("[X] armadillo-isomap")
ELSE()
	MESSAGE("[ ] armadillo-isomap")
ENDIF()

