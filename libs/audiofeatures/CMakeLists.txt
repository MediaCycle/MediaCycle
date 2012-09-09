IF(SUPPORT_AUDIO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiofeatures ${SRC} ${HDR})

	TARGET_LINK_LIBRARIES(audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} fftooura mediacycle armadillo-utils ${ARMADILLO_LIBRARIES})
	ADD_DEPENDENCIES(audiofeatures mediacycle)

	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
	MESSAGE("[X] audiofeatures")
ELSE()
	MESSAGE("[ ] audiofeatures")
ENDIF()
