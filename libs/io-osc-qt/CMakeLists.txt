SET(TARGET_NAME "mediacycle-io-osc-qt")
IF(USE_OSC AND USE_QT4)

        FILE(GLOB ${TARGET_NAME}_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
        FILE(GLOB ${TARGET_NAME}_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
        FILE(GLOB ${TARGET_NAME}_UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

	# Generates ui_*.h files 
	QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
	# Generates moc_*.cxx files
	QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed	

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS} ${${TARGET_NAME}_RCC_SRCS})
	ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-osg)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/io-osc ${CMAKE_SOURCE_DIR}/libs/gui-osg-qt)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/io-osc ${CMAKE_SOURCE_DIR}/libs/gui-osg-qt)
        TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-osg-qt mediacycle-io-osc)

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
