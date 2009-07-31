FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_EXECUTABLE(mediacycle-cli ${SRC} ${HDR})
ADD_DEPENDENCIES(mediacycle-cli mediacycle)

TARGET_LINK_LIBRARIES(mediacycle-cli mediacycle)

INSTALL(TARGETS mediacycle-cli RUNTIME DESTINATION bin)
