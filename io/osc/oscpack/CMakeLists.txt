FILE(GLOB oscpack_SRC *.cpp)
FILE(GLOB oscpack_HDR *.h)

IF ( USE_OSC )
ADD_LIBRARY(oscpack STATIC ${oscpack_SRC} ${oscpack_HDR})
INSTALL(TARGETS oscpack ARCHIVE DESTINATION lib)
#INSTALL(FILES ${oscpack_HDR} DESTINATION include/TiCore/Io/Osc)
ENDIF ( USE_OSC )
