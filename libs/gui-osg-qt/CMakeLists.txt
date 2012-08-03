IF ( USE_QT4 )

	FILE(GLOB mediacycle-osg-qt_SRCS *.cpp)
	FILE(GLOB mediacycle-osg-qt_HDRS *.h)
	FILE(GLOB mediacycle-osg-qt_UIS *.ui)

	# Generates ui_*.h files 
	QT4_WRAP_UI(mediacycle-osg-qt_UIS_H ${mediacycle-osg-qt_UIS})
	# Generates moc_*.cxx files
	QT4_WRAP_CPP(mediacycle-osg-qt_MOC_SRCS ${mediacycle-osg-qt_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed	

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(mediacycle-osg-qt STATIC ${mediacycle-osg-qt_SRCS} ${mediacycle-osg-qt_HDRS} ${mediacycle-osg-qt_UIS_H} ${mediacycle-osg-qt_MOC_SRCS} ${mediacycle-osg-qt_RCC_SRCS})
	#ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-osg)
	IF(SUPPORT_AUDIO)
		ADD_DEPENDENCIES(mediacycle-osg-qt audioengine)
	ENDIF()
	#ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-io-osc)
	IF ( USE_AUDIOGARDEN AND ( USE_OPENAL OR USE_PORTAUDIO) )
		ADD_DEPENDENCIES(mediacycle-osg-qt armadillo-utils audiofeatures audiogarden)
	ENDIF()

	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${QT_LIBRARIES} ${QWT_LIBRARY})
	IF ( MINGW )#OR APPLE_IOS_DEV OR APPLE_IOS_SIM )
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${Qt4_PKG_STATIC_LDFLAGS})#${Qt4_PKG_LDFLAGS})
	ENDIF ( MINGW )#OR APPLE_IOS_DEV OR APPLE_IOS_SIM )
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${OPENGL_LIBRARIES})
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt audioengine)
	ENDIF()
    IF(USE_OSC)
	    TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-io-osc)
    ENDIF()
	IF ( USE_AUDIOGARDEN AND ( USE_OPENAL OR USE_PORTAUDIO ) )
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} audiofeatures fftooura armadillo-utils)
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt audiogarden-synthesis)
	ENDIF()

	#IF ( APPLE )
	#	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	#ENDIF ( APPLE )
	MESSAGE("[X] mediacycle-osg-qt")
ELSE()
	MESSAGE("[ ] mediacycle-osg-qt")
ENDIF()
