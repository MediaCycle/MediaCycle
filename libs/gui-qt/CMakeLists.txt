SET(TARGET_NAME "mediacycle-qt")
IF((USE_QT4 OR USE_QT5) AND QWT_FOUND) 

	FILE(GLOB ${TARGET_NAME}_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/AC*.cpp)
	FILE(GLOB ${TARGET_NAME}_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/AC*.h)
	FILE(GLOB ${TARGET_NAME}_UIS ${CMAKE_CURRENT_SOURCE_DIR}/AC*.ui)

	# Generates ui_*.h files 
	IF(USE_QT4)
		QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
        ELSEIF(USE_QT5)
                QT5_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
        ENDIF()

	# Generates moc_*.cxx files
	IF(USE_QT4)
		QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed
        ELSEIF(USE_QT5)
                QT5_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed
        ENDIF()

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS} ${${TARGET_NAME}_RCC_SRCS})
        ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-gui-core mediacycle)

        TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-gui-core mediacycle)
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${Boost_SYSTEM_LIBRARY})
        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} ${QWT_LIBRARIES})

	IF(USE_QT5)
                qt5_use_modules(${TARGET_NAME} Core Gui Widgets WebKitWidgets)
                #set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC ON)
                #target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
	ENDIF()

	IF ( MINGW )#OR APPLE_IOS_DEV OR APPLE_IOS_SIM )
		TARGET_LINK_LIBRARIES(${TARGET_NAME} ${Qt4_PKG_STATIC_LDFLAGS})#${Qt4_PKG_LDFLAGS})
	ENDIF ( MINGW )#OR APPLE_IOS_DEV OR APPLE_IOS_SIM )
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
