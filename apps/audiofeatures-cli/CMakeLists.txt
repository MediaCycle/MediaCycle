FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(audiofeatures-cli ${SRC} ${HDR})

ADD_DEPENDENCIES(audiofeatures-cli armadillo-utils audiofeatures)
INCLUDE_DIRECTORIES(. ../../armadillo ../../audio)

TARGET_LINK_LIBRARIES(audiofeatures-cli audiofeatures sndfile)

INSTALL(TARGETS audiofeatures-cli RUNTIME DESTINATION bin)

