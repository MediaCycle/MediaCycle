include(${CMAKE_SOURCE_DIR}/CMakeModules/AddQtApp.cmake)
macro(ADD_MC_OSG_QT_EXECUTABLE TARGET_NAME)

	SET(WITH_MC ON)
	SET(WITH_OSG ON)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/gui-osg-qt)

	ADD_QT_EXECUTABLE(${TARGET_NAME})	

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
		IF(USE_VAMP)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_vamp)
		ENDIF()
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
	IF (SUPPORT_NAVIMED AND USE_NAVIMED)
		ADD_DEPENDENCIES(${TARGET_NAME} mc_navimed)
	ENDIF()

	ADD_DEPENDENCIES(${TARGET_NAME} mc_file)

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		IF ( USE_APPLE_MULTITOUCH )
			#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )

endmacro(ADD_MC_OSG_QT_EXECUTABLE TARGET_NAME)
