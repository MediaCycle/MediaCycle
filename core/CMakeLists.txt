FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
	execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/ACMediaFactoryDebug.in ${CMAKE_CURRENT_BINARY_DIR}/ACMediaFactoryDebug.cpp)

	# add the executable that will do the generation
	ADD_EXECUTABLE(mediafactorygenerator AC3DModel.cpp AC3DModel.h ACAudio.cpp ACAudio.h ACFeaturesTypes.h ACImage.cpp ACImage.h ACMedia.cpp ACMedia.h ACMediaData.cpp ACMediaData.h ACMediaFactory.cpp ACMediaFactory.h ACMediaFactoryGenerator.cpp ACMediaFeatures.cpp ACMediaFeatures.h ACMediaSegment.cpp ACMediaSegment.h ACMediaTimedFeature.cpp ACMediaTimedFeature.h ACMediaTypes.h ACOpenCVInclude.h ACPlugin.cpp ACPlugin.h ACPluginManager.cpp ACPluginManager.h ACVideo.cpp ACVideo.h DynamicLibrary.cpp DynamicLibrary.h)
	GET_TARGET_PROPERTY(MEDIA_FACTORY_GENERATOR mediafactorygenerator LOCATION)
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ACMediaFactoryDebug.cpp
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/ACMediaFactoryDebug.in ${CMAKE_CURRENT_BINARY_DIR}/ACMediaFactoryDebug.cpp
		DEPENDS mediafactorygenerator
	)

	ADD_CUSTOM_COMMAND(TARGET mediafactorygenerator
		POST_BUILD
		COMMAND ${MEDIA_FACTORY_GENERATOR} "${CMAKE_CURRENT_BINARY_DIR}/ACMediaFactoryDebug.cpp"
	)

	ADD_LIBRARY(mediacycle STATIC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/ACMediaFactoryDebug.cpp ${HDR})
ELSE()
	ADD_LIBRARY(mediacycle STATIC ${SRC} ${HDR})
ENDIF()

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

ADD_DEPENDENCIES(mediacycle armadillo-utils tinyxml)
IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
ADD_DEPENDENCIES(mediacycle mediafactorygenerator)
ENDIF(USE_DEBUG)#(APPLE AND USE_DEBUG)

TARGET_LINK_LIBRARIES(mediacycle armadillo-utils tinyxml armadillo-utils)
IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
	TARGET_LINK_LIBRARIES(mediafactorygenerator armadillo-utils tinyxml armadillo-utils)
ENDIF()
IF (APPLE)
	TARGET_LINK_LIBRARIES(mediacycle stdc++)# CF otherwise "make package" fails
ENDIF(APPLE)
	
IF ( SDIF_FOUND )
	TARGET_LINK_LIBRARIES(mediacycle sdif)
	IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
		TARGET_LINK_LIBRARIES(mediafactorygenerator sdif)
	ENDIF()
ENDIF ( )

#others
IF(SUPPORT_AUDIO)
	TARGET_LINK_LIBRARIES(mediacycle sndfile)
	IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
		TARGET_LINK_LIBRARIES(mediafactorygenerator sndfile)
	ENDIF()
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
	IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
		TARGET_LINK_LIBRARIES(mediafactorygenerator ${OpenCV_LIBRARIES})
	ENDIF()
ENDIF()
TARGET_LINK_LIBRARIES(mediacycle ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_GRAPH_LIBRARY})
IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
	TARGET_LINK_LIBRARIES(mediafactorygenerator ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_GRAPH_LIBRARY})
ENDIF()
IF(SUPPORT_VIDEO)
	TARGET_LINK_LIBRARIES(mediacycle ${FFMPEG_LIBRARIES} ${FFMPEG_LIBSWSCALE_LIBRARIES})
	IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
		TARGET_LINK_LIBRARIES(mediafactorygenerator ${FFMPEG_LIBRARIES} ${FFMPEG_LIBSWSCALE_LIBRARIES})
	ENDIF()
ENDIF()

# OSG for 3D Models and/or ffmpeg plugin checkup
IF(SUPPORT_3DMODEL OR SUPPORT_IMAGE OR SUPPORT_VIDEO)
	TARGET_LINK_LIBRARIES(mediacycle ${OPENSCENEGRAPH_CORE_LIBRARIES})
	IF(USE_DEBUG)#(APPLE AND USE_DEBUG)
		TARGET_LINK_LIBRARIES(mediafactorygenerator ${OPENSCENEGRAPH_CORE_LIBRARIES})
	ENDIF()
ENDIF()

# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)
