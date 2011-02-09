IF (USE_AUDIOGARDEN)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiogarden STATIC ${SRC} ${HDR})

	TARGET_LINK_LIBRARIES(audiogarden ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} audiofeatures fftooura armadillo armadillo-utils atlas lapack)
	INCLUDE_DIRECTORIES(. ../fftooura ../armadillo ../audio ../tinyxml)

	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
ENDIF ()

