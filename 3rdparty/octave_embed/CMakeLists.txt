FILE(GLOB SRC *.cc *.c)
FILE(GLOB HDR *.h)

ADD_LIBRARY(octave_embed ${SRC} ${HDR})

TARGET_LINK_LIBRARIES(octave_embed ${OCTAVE_LIBRARY})

#INSTALL(TARGETS octave_embed LIBRARY DESTINATION lib)
#INSTALL(FILES ${HDR} DESTINATION include/octave_embed)
MESSAGE("[X] octave_embed")
