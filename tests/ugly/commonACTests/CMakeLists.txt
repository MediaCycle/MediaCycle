FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_EXECUTABLE(commonAC-test ${SRC} ${HDR})
ADD_DEPENDENCIES(commonAC-test mediacycle)

TARGET_LINK_LIBRARIES(commonAC-test mediacycle)

MESSAGE("[X] common mediacycle")
