IF(SUPPORT_AUDIO AND USE_AUDIOGARDEN)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiogarden-synthesis STATIC ${SRC} ${HDR})

    ADD_DEPENDENCIES(audiogarden-synthesis mediacycle audiofeatures)

	IF(WIN32 OR MINGW)
		TARGET_LINK_LIBRARIES(audiogarden-synthesis ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} mediacycle audiofeatures fftooura ${ARMADILLO_LIBRARY} armadillo-utils ${ATLAS_LIBRARY} ${LAPACK_LIBRARY})
	ELSE()
		TARGET_LINK_LIBRARIES(audiogarden-synthesis ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} mediacycle audiofeatures fftooura ${ARMADILLO_LIBRARY} armadillo-utils atlas lapack)
	ENDIF()

	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
	MESSAGE("[X] audiogarden-synthesis")
ELSE()
	MESSAGE("[ ] audiogarden-synthesis")
ENDIF()
