IF ( USE_OSG AND FFMPEG_FOUND )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(mediacycle STATIC ${SRC} ${HDR})

	#gsl - uncomment if needed
	#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

	#others
	IF ( MINGW )
		TARGET_LINK_LIBRARIES(mediacycle sndfile FLAC vorbisenc vorbis m ogg pthread wsock32 ws2_32) # CF: should be detected in main CMakeLists.txt
	ELSE ( MINGW )
		TARGET_LINK_LIBRARIES(mediacycle sndfile pthread)
	ENDIF ( MINGW )

	#OpenCV
	IF( APPLE_IOS_DEV OR APPLE_IOS_SIM OR APPLE_IOS_XCODE )
		TARGET_LINK_LIBRARIES(mediacycle cv cxcore)
	ELSE()
		TARGET_LINK_LIBRARIES(mediacycle cv cxcore highgui)
	ENDIF()

	TARGET_LINK_LIBRARIES(mediacycle ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_GRAPH_LIBRARY})
	TARGET_LINK_LIBRARIES(mediacycle ${FFMPEG_LIBRARIES} ${FFMPEG_LIBSWSCALE_LIBRARIES})

	# OSG for 3D Models
	TARGET_LINK_LIBRARIES(mediacycle osg osgDB OpenThreads)

	# this might require some path adaptation for Windows
	# (e.g. set cmake_install_prefix to the good value for Windows )
	# OSX is probably ok with installing lib in /usr/local/lib/
	##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
	##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)
ENDIF ( USE_OSG AND FFMPEG_FOUND )
