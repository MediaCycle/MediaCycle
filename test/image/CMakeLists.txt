FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(image-test ${SRC} ${HDR})

ADD_DEPENDENCIES(image-test mediacycle libimage)
INCLUDE_DIRECTORIES(. ../../plugins/image ../../plugins/image/blobs ../../plugins/image/gabor)

TARGET_LINK_LIBRARIES(image-test mediacycle image)

INSTALL(TARGETS image-test RUNTIME DESTINATION bin)

