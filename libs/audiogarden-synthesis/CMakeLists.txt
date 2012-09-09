IF(SUPPORT_AUDIO AND USE_AUDIOGARDEN AND USE_AUDIOFEAT)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiogarden-synthesis ${SRC} ${HDR})

    ADD_DEPENDENCIES(audiogarden-synthesis mediacycle audiofeatures)

	TARGET_LINK_LIBRARIES(audiogarden-synthesis ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} mediacycle audiofeatures fftooura armadillo-utils)
	
	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
	MESSAGE("[X] audiogarden-synthesis")
ELSE()
	MESSAGE("[ ] audiogarden-synthesis")
ENDIF()
