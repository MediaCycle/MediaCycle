SET(TARGET_NAME "videoPlayer-iVisit-qt")
IF(BUILD_IVISIT_APPS AND SUPPORT_VIDEO AND USE_QT4 AND QT_PHONON_FOUND)

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

#	set(QT_PHONON_LIBRARY phonon)
#	set(PHONON_LIBS
#		${QT_PHONON_LIBRARY}
#		${QT_QTCORE_LIBRARY}
#		${QT_QTGUI_LIBRARY}
#	)
    SET(QT_USE_PHONON TRUE)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(settings_SRCS mediaplayer.cpp main.cpp iVisit.cpp)
	SET(settings_MOC_HDRS mediaplayer.h iVisit.h)
	SET(settings_UIS settings.ui iVisit.ui)
	
	SET(settings_RCCS mediaplayer.qrc)
	# generate rules for building source files from the resources
	QT4_ADD_RESOURCES(settings_SRCS ${settings_RCCS})


	# Generates ui_settings.h 
	QT4_WRAP_UI(settings_UIS_H ${settings_UIS})

	# Generates moc_settings.cxx
	QT4_WRAP_CPP(settings_MOC_SRCS ${settings_MOC_HDRS} ${QtApp_MOCS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(videoPlayer-iVisit-qt WIN32 ${settings_UIS_H} ${settings_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(videoPlayer-iVisit-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${videoPlayer-iVisit-qt_SRCS} ${videoPlayer-iVisit-qt_UIS_H} ${videoPlayer-iVisit-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(videoPlayer-iVisit-qt ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS})
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(videoPlayer-iVisit-qt mc_image)
	ADD_DEPENDENCIES(videoPlayer-iVisit-qt mediacycle)
#	ADD_DEPENDENCIES(videoPlayer-iVisit-qt mediacycle-osg)
#	ADD_DEPENDENCIES(videoPlayer-iVisit-qt mediacycle-osg-qt)

	#IF ( APPLE )
	#	set_target_properties(videoPlayer-iVisit-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	#TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt mc_image)
	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt mediacycle)
#	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt mediacycle-osg)
#	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt mediacycle-osg-qt)
#	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_PHONON_LIBRARY})

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )

	IF ( WIN32 )
		TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt ${OPENGL_LIBRARIES} ${FREEGLUT_LIBRARIES})
	ELSE ( WIN32 )
		TARGET_LINK_LIBRARIES(videoPlayer-iVisit-qt ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ENDIF ( WIN32 ) 
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
