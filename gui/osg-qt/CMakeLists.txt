IF ( USE_QT4 )

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../ ../../armadillo ../../fftooura ../../tinyxml ../osg)

	IF(SUPPORT_AUDIO)
		INCLUDE_DIRECTORIES(../../audio)
		IF(USE_AUDIOGARDEN)
			INCLUDE_DIRECTORIES(../../audiogarden)
		ENDIF()
	ENDIF()

	IF(USE_OSC)
		INCLUDE_DIRECTORIES(../../io/osc)
	ENDIF()

	FILE(GLOB mediacycle-osg-qt_UIS *.ui)
	STRING(REPLACE ".ui" ".cpp" mediacycle-osg-qt_SRCS "${mediacycle-osg-qt_UIS}")
	STRING(REPLACE ".ui" ".h" mediacycle-osg-qt_MOC_HDRS "${mediacycle-osg-qt_UIS}")

	# Generates ui_ACAudioControlsDockWidgetQt.h and ui_ACVideooControlsDockWidgetQt.h 
	QT4_WRAP_UI(mediacycle-osg-qt_UIS_H ${mediacycle-osg-qt_UIS})

	# Generates moc_ACAudioControlsDockWidgetQt.cxx and moc_ACVideoControlsDockWidgetQt.cxx
	QT4_WRAP_CPP(mediacycle-osg-qt_MOC_SRCS ${mediacycle-osg-qt_MOC_HDRS})

	SET(mediacycle-osg-qt_RCC ACOsgQtResources.qrc)
	QT4_ADD_RESOURCES(mediacycle-osg-qt_RCC_SRCS ${mediacycle-osg-qt_RCC})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(mediacycle-osg-qt STATIC ${SRC} ${HDR} ${mediacycle-osg-qt_UIS_H} ${mediacycle-osg-qt_MOC_SRCS} ${mediacycle-osg-qt_RCC_SRCS})
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-osg)
	IF(SUPPORT_AUDIO)
		ADD_DEPENDENCIES(mediacycle-osg-qt audioengine)
	ENDIF()
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(mediacycle-osg-qt oscpack)
	IF ( USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND ( USE_OPENAL OR USE_PORTAUDIO) )
		ADD_DEPENDENCIES(mediacycle-osg-qt armadillo-utils audiofeatures audiogarden)
	ENDIF()

	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${OPENGL_LIBRARIES})
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt audioengine)
	ENDIF()
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-io-osc)
	IF ( USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND USE_OPENAL )
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} audiofeatures fftooura ${ARMADILLO_LIBRARY} armadillo-utils atlas lapack)
		#TARGET_LINK_LIBRARIES(mediacycle-osg-qt audiogarden)
	ENDIF()

	#IF ( APPLE )
	#	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	#ENDIF ( APPLE )
	MESSAGE("[X] mediacycle-osg-qt")
ELSE()
	MESSAGE("[ ] mediacycle-osg-qt")
ENDIF()
