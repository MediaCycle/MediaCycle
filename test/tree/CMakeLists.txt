FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(tree-test ${SRC} ${HDR})

ADD_DEPENDENCIES(tree-test mediacycle)
INCLUDE_DIRECTORIES(.)

TARGET_LINK_LIBRARIES(tree-test mediacycle)

INSTALL(TARGETS tree-test RUNTIME DESTINATION bin)

