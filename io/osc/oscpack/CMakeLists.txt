FILE(GLOB oscpack_SRC *.cpp)
FILE(GLOB oscpack_HDR *.h)

IF ( USE_OSC )
ADD_LIBRARY(oscpack STATIC ${oscpack_SRC} ${oscpack_HDR})
IF ( MINGW )
	TARGET_LINK_LIBRARIES(oscpack winmm ws2_32) # CF: should be detected in main CMakeLists.txt wsock32
ENDIF ( MINGW )
INSTALL(TARGETS oscpack ARCHIVE DESTINATION lib)
#INSTALL(FILES ${oscpack_HDR} DESTINATION include/TiCore/Io/Osc)
ENDIF ( USE_OSC )
