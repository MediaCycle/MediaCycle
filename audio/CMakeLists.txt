IF(SUPPORT_AUDIO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(audiofeatures STATIC ${SRC} ${HDR})

	TARGET_LINK_LIBRARIES(audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} fftooura ${ARMADILLO_LIBRARY} armadillo-utils atlas lapack)
	INCLUDE_DIRECTORIES(. ../fftooura ../armadillo)

	#INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)
	MESSAGE("[X] audiofeatures")
ELSE()
	MESSAGE("[ ] audiofeatures")
ENDIF()
