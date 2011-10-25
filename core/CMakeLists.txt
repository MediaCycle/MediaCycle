FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(mediacycle STATIC ${SRC} ${HDR})

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

ADD_DEPENDENCIES(mediacycle armadillo-utils)
IF(TINYXML_FOUND)
	TARGET_LINK_LIBRARIES(mediacycle ${TINYXML_LIBRARY})
ELSE()
	ADD_DEPENDENCIES(mediacycle tinyxml)
	TARGET_LINK_LIBRARIES(mediacycle tinyxml)
ENDIF()

TARGET_LINK_LIBRARIES(mediacycle armadillo-utils)
IF (APPLE)
	TARGET_LINK_LIBRARIES(mediacycle stdc++)# CF otherwise "make package" fails
ENDIF(APPLE)
	
IF ( SDIF_FOUND )
	TARGET_LINK_LIBRARIES(mediacycle sdif)
ENDIF ( )

#others
IF(SUPPORT_AUDIO)
	TARGET_LINK_LIBRARIES(mediacycle sndfile)
ENDIF()
TARGET_LINK_LIBRARIES(mediacycle pthread)
IF ( MINGW )
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(mediacycle FLAC vorbisenc vorbis m ogg)
	ENDIF()
	TARGET_LINK_LIBRARIES(mediacycle wsock32 ws2_32) # CF: should be detected in main CMakeLists.txt
ENDIF ( MINGW )

#OpenCV
IF(SUPPORT_VIDEO OR SUPPORT_IMAGE)
	TARGET_LINK_LIBRARIES(mediacycle ${OpenCV_LIBRARIES})
ENDIF()

TARGET_LINK_LIBRARIES(mediacycle ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_GRAPH_LIBRARY} ${Boost_THREAD_LIBRARY})

IF(SUPPORT_VIDE0 AND FFMPEG_FOUND AND WITH_FFMPEG)
	TARGET_LINK_LIBRARIES(mediacycle ${FFMPEG_LIBRARIES} ${FFMPEG_LIBSWSCALE_LIBRARIES})
ENDIF()

IF(SUPPORT_PDF AND PODOFO_FOUND)
	TARGET_LINK_LIBRARIES(mediacycle ${PODOFO_LIBRARY})
ENDIF()

IF(SUPPORT_TEXT)
	ADD_DEPENDENCIES(mediacycle textTool)
	TARGET_LINK_LIBRARIES(mediacycle textTool)
ENDIF()
# OSG for 3D Models and/or ffmpeg plugin checkup
IF(SUPPORT_3DMODEL OR SUPPORT_IMAGE OR SUPPORT_VIDEO)
	TARGET_LINK_LIBRARIES(mediacycle ${OPENSCENEGRAPH_CORE_LIBRARIES})
ENDIF()

# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)
