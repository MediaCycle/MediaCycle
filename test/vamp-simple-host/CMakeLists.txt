FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(vamp-simple-host ${SRC} ${HDR})

ADD_DEPENDENCIES(vamp-simple-host vamp-simple-hostsdk)
INCLUDE_DIRECTORIES(. ../../lib/vamp-plugin-sdk)

TARGET_LINK_LIBRARIES(vamp-simple-host vamp-hostsdk vamp-sdk ${SNDFILE_LIBRARY} dl)


