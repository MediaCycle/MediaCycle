FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.hh)

ADD_LIBRARY(tree ${SRC} ${HDR})
SET_TARGET_PROPERTIES("tree" PROPERTIES FOLDER "${FOLDERNAME}")
MESSAGE("[X] Kasper Peeters' STL-like templated tree class")
