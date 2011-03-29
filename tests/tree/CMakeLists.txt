FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_EXECUTABLE(tree-test ${SRC} ${HDR})

ADD_DEPENDENCIES(tree-test mediacycle)

TARGET_LINK_LIBRARIES(tree-test mediacycle)

MESSAGE("[X] tree")
