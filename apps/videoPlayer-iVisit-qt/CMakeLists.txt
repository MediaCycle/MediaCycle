SET(TARGET_NAME "videoPlayer-iVisit-qt")
IF(BUILD_IVISIT_APPS AND SUPPORT_VIDEO AND (USE_QT4 OR USE_QT5) AND QT_PHONON_FOUND)

	SET(MEDIA_TYPE "MEDIA_TYPE_VIDEO")
	SET(MC_PLUGINS "file;video-reader-opencv;")#video;
	SET(DESCRIPTION "iVisit.")



	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	SET(MAIN_CLASS "iVisit")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleQtApp.cmake)
	ADD_MC_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} )
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} )

#	set(QT_PHONON_LIBRARY phonon)
#	set(PHONON_LIBS
#		${QT_PHONON_LIBRARY}
#		${QT_QTCORE_LIBRARY}
#		${QT_QTGUI_LIBRARY}
#	)

	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
