FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_EXECUTABLE(commonAC-test ${SRC} ${HDR})
ADD_DEPENDENCIES(commonAC-test mediacycle)

TARGET_LINK_LIBRARIES(commonAC-test mediacycle)

	SET_TARGET_PROPERTIES("commonAC-test" PROPERTIES FOLDER "${FOLDERNAME}")
MESSAGE("[X] common mediacycle")
