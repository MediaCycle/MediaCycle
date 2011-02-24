IF (BUILD_CLIS)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(mediacycle-cli ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-cli mediacycle)
	#ADD_DEPENDENCIES(mediacycle-cli audiogarden)
	#INCLUDE_DIRECTORIES(. ../audiogarden)
	INCLUDE_DIRECTORIES(. ../fftooura ../armadillo ../tinyxml)

	TARGET_LINK_LIBRARIES(mediacycle-cli mediacycle tinyxml)
	#TARGET_LINK_LIBRARIES(mediacycle-cli audiogarden)

	INSTALL(TARGETS mediacycle-cli RUNTIME DESTINATION bin)

ENDIF (BUILD_CLIS)
