SET(TARGET_NAME "navimedbiodatascript")
IF(SUPPORT_TEXT AND USE_QT4)
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!

	ADD_EXECUTABLE(navimedbiodatascript ${SRC} ${HDR})
	IF(TINYXML_FOUND)
		TARGET_LINK_LIBRARIES(navimedbiodatascript ${TINYXML_LIBRARY})
	ELSE()
		ADD_DEPENDENCIES(navimedbiodatascript tinyxml)
		TARGET_LINK_LIBRARIES(navimedbiodatascript tinyxml)
	ENDIF()


	#IF ( APPLE )
	#	set_target_properties(text-test PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
