SET(TARGET_NAME "text-test")
IF(SUPPORT_TEXT AND USE_QT4)
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!

	ADD_EXECUTABLE(text-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(text-test mc_text)
	ADD_DEPENDENCIES(text-test mc_segmentation)
	ADD_DEPENDENCIES(text-test mc_visualisation)
	ADD_DEPENDENCIES(text-test mediacycle)
	ADD_DEPENDENCIES(text-test mediacycle-osg)
	ADD_DEPENDENCIES(text-test mediacycle-osg-qt)
	ADD_DEPENDENCIES(text-test SparseMatrix)


	#IF ( APPLE )
	#	set_target_properties(text-test PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(text-test mc_text)
	TARGET_LINK_LIBRARIES(text-test mediacycle)
	TARGET_LINK_LIBRARIES(text-test mediacycle-osg)
	TARGET_LINK_LIBRARIES(text-test mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(text-test SparseMatrix)
	TARGET_LINK_LIBRARIES(text-test ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(text-test ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(text-test ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(text-test ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
