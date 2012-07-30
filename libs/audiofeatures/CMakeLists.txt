IF(SUPPORT_AUDIO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiofeatures STATIC ${SRC} ${HDR})

	TARGET_LINK_LIBRARIES(audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} fftooura armadillo-utils ${ARMADILLO_LIBRARIES})
	ADD_DEPENDENCIES(audiofeatures mediacycle)

	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
	MESSAGE("[X] audiofeatures")
ELSE()
	MESSAGE("[ ] audiofeatures")
ENDIF()
