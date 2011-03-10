FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h)

ADD_LIBRARY(tree STATIC ${SRC} ${HDR})
MESSAGE("[X] Kasper Peeters' STL-like templated tree class")
