IF ( USE_QT4 )
	MESSAGE("gui/osg-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../osg ../../tinyxml)
	IF ( USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND USE_OPENAL )
		INCLUDE_DIRECTORIES(. ../../armadillo ../../audio ../../ ../../fftooura ../../audiogarden)
	ENDIF()

	FILE(GLOB mediacycle-osg-qt_UIS *.ui)
	SET(mediacycle-osg-qt_SRCS "${mediacycle-osg-qt_UIS}")
	STRING(REPLACE ".ui" ".cpp" mediacycle-osg-qt_SRCS "${mediacycle-osg-qt_SRCS}")
	SET(mediacycle-osg-qt_MOC_HDRS "${mediacycle-osg-qt_UIS}")
	STRING(REPLACE ".ui" ".h" mediacycle-osg-qt_MOC_HDRS "${mediacycle-osg-qt_MOC_HDRS}")

	# Generates ui_ACAudioControlsDockWidgetQt.h and ui_ACVideooControlsDockWidgetQt.h 
	QT4_WRAP_UI(mediacycle-osg-qt_UIS_H ${mediacycle-osg-qt_UIS})

	# Generates moc_ACAudioControlsDockWidgetQt.cxx and moc_ACVideoControlsDockWidgetQt.cxx
	QT4_WRAP_CPP(mediacycle-osg-qt_MOC_SRCS ${mediacycle-osg-qt_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(mediacycle-osg-qt STATIC ${SRC} ${HDR} ${mediacycle-osg-qt_UIS_H} ${mediacycle-osg-qt_MOC_SRCS})
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(mediacycle-osg-qt audioengine)
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(mediacycle-osg-qt oscpack)
	IF ( USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND USE_OPENAL )
		ADD_DEPENDENCIES(mediacycle-osg-qt armadillo-utils audiofeatures audiogarden)
		ADD_DEPENDENCIES(mediacycle-osg-qt audioengine)
	ENDIF()

	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt OpenThreads)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${OPENGL_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt audioengine)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-io-osc)
	IF ( USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND USE_OPENAL )
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} audiofeatures fftooura armadillo armadillo-utils atlas lapack)
		#TARGET_LINK_LIBRARIES(mediacycle-osg-qt audiogarden)
		#TARGET_LINK_LIBRARIES(mediacycle-osg-qt audioengine)
	ENDIF()

	#IF ( APPLE )
	#	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	#ENDIF ( APPLE )
ELSE ( USE_QT4 )
	MESSAGE("Qt4 disabled - not compiling Qt GUI.")
ENDIF ( USE_QT4 )
