SET(TARGET_NAME "audioengine-tests")
IF(SUPPORT_AUDIO AND USE_OPENAL)
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-engine)

	#IF ( APPLE )
	#	SET(APP_TYPE MACOSX_BUNDLE)
	#ENDIF ( APPLE )

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!

	ADD_EXECUTABLE(${TARGET_NAME} ${SRC} ${HDR})

	ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-audio-engine)


	#IF ( APPLE )
	#	set_target_properties(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

        TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-audio-engine)

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreAudio -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
