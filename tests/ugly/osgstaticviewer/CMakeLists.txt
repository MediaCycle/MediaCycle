SET(TARGET_NAME "osgstaticviewer") # from the OSG examples
IF(OSG_FOUND AND CMAKE_TOOLCHAIN_FILE AND MINGW)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(${TARGET_NAME} ${SRC} ${HDR})

	TARGET_LINK_LIBRARIES(${TARGET_NAME} -L${OSG_LIBRARY_DIR}/osgPlugins-${OPENSCENEGRAPH_VERSION}
	)#-losgdb_obj -losgdb_osg -losgdb_deprecated_osg -losgdb_deprecated_osgtext -losgdb_deprecated_osgviewer -losgdb_serializers_osg -losgdb_serializers_osgtext)

	#TARGET_LINK_LIBRARIES(${TARGET_NAME} osgdb_svg)
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} osgdb_svg)
	IF(FREETYPE_FOUND)
    		ADD_DEFINITIONS(-DUSE_FREETYPE)
    		TARGET_LINK_LIBRARIES(${TARGET_NAME} osgdb_freetype)
	ENDIF(FREETYPE_FOUND)

	IF(FFMPEG_FOUND)
		#TARGET_LINK_LIBRARIES(${TARGET_NAME} -losgdb_ffmpeg ${FFMPEG_LIBRARIES})
		TARGET_LINK_LIBRARIES(${TARGET_NAME} osgdb_ffmpeg)
	ENDIF()

        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${OPENSCENEGRAPH_LIBRARIES} ${OPENGL_LIBRARIES})

	SET_TARGET_PROPERTIES("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()


