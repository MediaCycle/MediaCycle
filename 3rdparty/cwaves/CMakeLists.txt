FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(cwaves ${SRC} ${HDR})

MESSAGE("[X] cwaves")
