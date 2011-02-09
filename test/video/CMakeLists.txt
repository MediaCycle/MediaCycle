FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(video-test ${SRC} ${HDR})

ADD_DEPENDENCIES(video-test mediacycle ${PLUGIN_PREFIX}video)
INCLUDE_DIRECTORIES(. ../../plugins/video ../../plugins/image ../../plugins/image/blobs ../../plugins/image/gabor ../../armadillo ../../tinyxml)

TARGET_LINK_LIBRARIES(video-test mediacycle ${PLUGIN_PREFIX}video)



INSTALL(TARGETS video-test RUNTIME DESTINATION bin)

