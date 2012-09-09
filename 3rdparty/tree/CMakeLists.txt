FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.hh)

ADD_LIBRARY(tree ${SRC} ${HDR})
MESSAGE("[X] Kasper Peeters' STL-like templated tree class")
