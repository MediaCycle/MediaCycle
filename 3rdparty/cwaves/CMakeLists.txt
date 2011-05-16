FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(cwaves STATIC ${SRC} ${HDR})

MESSAGE("[X] cwaves")
