FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_EXECUTABLE(mediacycle-dancers ${SRC} ${HDR})
ADD_DEPENDENCIES(mediacycle-dancers mediacycle)

TARGET_LINK_LIBRARIES(mediacycle-dancers mediacycle)

INSTALL(TARGETS mediacycle-dancers RUNTIME DESTINATION bin)
