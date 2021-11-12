SET(TARGET_NAME "octave_embed_test")
IF(OCTAVE_FOUND)
	#FILE(GLOB SRC *.cpp)
	#FILE(GLOB HDR *.h)
	SET(SRC ${CMAKE_SOURCE_DIR}/3rdparty/octave_embed/test.cpp)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!

	ADD_EXECUTABLE(${TARGET_NAME} ${SRC} ${HDR})

	ADD_DEPENDENCIES(${TARGET_NAME} octave_embed)


	#IF ( APPLE )
	#	set_target_properties(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(${TARGET_NAME} octave_embed ${OCTAVE_LIBRARY})

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
