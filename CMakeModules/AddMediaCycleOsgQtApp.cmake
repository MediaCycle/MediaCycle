macro(ADD_MC_OSG_QT_EXECUTABLE TARGET_NAME)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/gui-osg-qt)

	FILE(GLOB ${TARGET_NAME}_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
	FILE(GLOB ${TARGET_NAME}_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
	FILE(GLOB ${TARGET_NAME}_UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

	# Generates ui_*.h files
	QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})

	# Generates moc_*.cxx files
	QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/libs/gui-osg-qt)

	IF ( WIN32 OR MINGW)
		EXECUTE_PROCESS(COMMAND echo "IDI_ICON1	ICON	DISCARDABLE \"${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.ico\"" OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
		EXECUTE_PROCESS(COMMAND ${QT_RC_EXECUTABLE} -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc 
                             -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.o)
		SET(OS_SPECIFIC "WIN32" ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.o)
	ELSE ()
		IF ( APPLE )
			SET(APP_TYPE MACOSX_BUNDLE)
			SET(MACOSX_BUNDLE_BUNDLE_NAME ${TARGET_NAME})
			# set how it shows up in the Info.plist file
  			SET(MACOSX_BUNDLE_ICON_FILE MultiMediaCycle.icns) 
  			# set where in the bundle to put the icns file
  			SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  			# include the icns file in the target
  			SET(${TARGET_NAME}_SRCS ${${TARGET_NAME}_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)
			SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
		ENDIF()
	ENDIF()
	ADD_EXECUTABLE(${TARGET_NAME} ${OS_SPECIFIC} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS})

	ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-osg-qt)
	ADD_DEPENDENCIES(${TARGET_NAME} mc_visualisation)
	ADD_DEPENDENCIES(${TARGET_NAME} mc_segmentation)
	ADD_DEPENDENCIES(${TARGET_NAME} mc_pca)

	TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle)
	TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-osg)
	TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-osg-qt)

	IF(USE_OSC)
		TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-io-osc)
	ENDIF()
	
	IF(SUPPORT_AUDIO AND USE_AUDIO)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_audio audioengine)
		TARGET_LINK_LIBRARIES(${TARGET_NAME} audioengine)
	ENDIF()
	IF((SUPPORT_IMAGE OR SUPPORT_VIDEO) AND (USE_IMAGE OR USE_VIDEO))
		ADD_DEPENDENCIES(${TARGET_NAME} mc_image)
	ENDIF()
	IF(SUPPORT_VIDEO AND USE_VIDEO)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_video)
	ENDIF()
	IF(SUPPORT_3DMODEL AND USE_3DMODEL)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_3Dmodel)
	ENDIF()
	IF(SUPPORT_TEXT AND USE_TEXT)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_text mc_text_sparse mc_SparseVisualisation)
	ENDIF()
	IF (SUPPORT_ARCHIPEL AND USE_ARCHIPEL)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_archipel)
	ENDIF()
	IF (SUPPORT_NAVIMEDL AND USE_NAVIMED)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_navimed)
	ENDIF()

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		#IF ( USE_APPLE_MULTITOUCH )
		#	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
		#	TARGET_LINK_LIBRARIES(audiogarden-osg-qt mediacycle-io-applemultitouch)
		#ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )

	# Copy Qt imageformat plugin to (Apple) Debug apps
	IF(APPLE AND USE_DEBUG)
		ADD_CUSTOM_COMMAND( TARGET ${TARGET_NAME} POST_BUILD
			COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.app/Contents/plugins
      			COMMAND cp -R ${QT_PLUGINS_DIR}/imageformats ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.app/Contents/plugins
		)
	ENDIF()
endmacro(ADD_MC_OSG_QT_EXECUTABLE TARGET_NAME)