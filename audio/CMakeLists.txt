FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(audiofeatures SHARED ${SRC} ${HDR})

TARGET_LINK_LIBRARIES(audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} fftooura armadillo armadillo-utils atlas lapack)
INCLUDE_DIRECTORIES(. ../fftooura ../armadillo)

INSTALL(TARGETS audiofeatures LIBRARY DESTINATION lib)
INSTALL(FILES ${HDR} DESTINATION include/audiofeatures)

