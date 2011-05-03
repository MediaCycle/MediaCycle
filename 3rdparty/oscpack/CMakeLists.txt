IF ( USE_OSC )
IF(WIN32)
 set(SystemTypePath ip/win32)
ELSE(WIN32)
 set(SystemTypePath ip/posix)
ENDIF(WIN32)

ADD_LIBRARY(oscpack STATIC 

ip/IpEndpointName.cpp 
${SystemTypePath}/NetworkingUtils.cpp
${SystemTypePath}/UdpSocket.cpp

osc/OscPrintReceivedElements.cpp
osc/OscReceivedElements.cpp 
osc/OscTypes.cpp 
osc/OscOutboundPacketStream.cpp

)

IF ( MINGW )
	TARGET_LINK_LIBRARIES(oscpack winmm ws2_32) # CF: should be detected in main CMakeLists.txt wsock32
ENDIF ( MINGW )
#INSTALL(TARGETS oscpack ARCHIVE DESTINATION lib)
ENDIF ( USE_OSC )
