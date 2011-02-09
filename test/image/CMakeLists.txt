FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(image-test ${SRC} ${HDR})

ADD_DEPENDENCIES(image-test mediacycle mc_image)
INCLUDE_DIRECTORIES(. ../../plugins/image ../../plugins/image/blobs ../../plugins/image/gabor ../../tinyxml)

TARGET_LINK_LIBRARIES(image-test mediacycle mc_image)

INSTALL(TARGETS image-test RUNTIME DESTINATION bin)

