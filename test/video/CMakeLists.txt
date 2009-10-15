FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)


ADD_EXECUTABLE(video-test ${SRC} ${HDR})

ADD_DEPENDENCIES(video-test mediacycle mc_video)
INCLUDE_DIRECTORIES(. ../../plugins/video ../../plugins/image ../../plugins/image/blobs)

TARGET_LINK_LIBRARIES(video-test mediacycle video)



INSTALL(TARGETS video-test RUNTIME DESTINATION bin)

