IF(SUPPORT_AUDIO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiofeatures STATIC ${SRC} ${HDR})

	IF(WIN32 OR MINGW)
		TARGET_LINK_LIBRARIES(audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} fftooura ${ARMADILLO_LIBRARY} armadillo-utils ${ATLAS_LIBRARY} ${LAPACK_LIBRARY})
	ELSE()
		TARGET_LINK_LIBRARIES(audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} fftooura ${ARMADILLO_LIBRARY} armadillo-utils atlas lapack)
	ENDIF()
	ADD_DEPENDENCIES(audiofeatures mediacycle)

	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
	MESSAGE("[X] audiofeatures")
ELSE()
	MESSAGE("[ ] audiofeatures")
ENDIF()
