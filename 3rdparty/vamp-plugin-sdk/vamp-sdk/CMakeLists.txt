FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(vamp-sdk ${SRC} ${HDR})
INCLUDE_DIRECTORIES(. ..)

#gsl - uncomment if needed
TARGET_LINK_LIBRARIES(vamp-sdk ${SNDFILE_LIBRARIES})

#INSTALL(TARGETS vamp-sdk LIBRARY DESTINATION lib)
#INSTALL(FILES ${HDR} DESTINATION include/vamp-sdk)

