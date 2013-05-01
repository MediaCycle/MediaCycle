IF ( ARMADILLO_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(armadillo-visualisation ${SRC} ${HDR})
	#ADD_DEPENDENCIES(armadillo-visualisation fftooura)
	
	TARGET_LINK_LIBRARIES(armadillo-visualisation ${ARMADILLO_LIBRARIES})
	ADD_DEPENDENCIES(armadillo-visualisation  armadillo-utils)
	TARGET_LINK_LIBRARIES(armadillo-visualisation  armadillo-utils)
	SET_TARGET_PROPERTIES("armadillo-visualisation" PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] armadillo-visualisation")
ELSE()
	MESSAGE("[ ] armadillo-visualisation")
ENDIF()

