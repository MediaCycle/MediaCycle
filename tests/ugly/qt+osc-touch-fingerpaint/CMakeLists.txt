IF(USE_QT4 AND USE_OSC)
	IF(QT_VERSION_MAJOR LESS 4 AND QT_VERSION_MINOR LESS 7)
		MESSAGE("[ ] qt+osc-touch-fingerpaint (Qt>=4.7 is required for multitouch support)")
	ELSE()
		FILE(GLOB SRC *.cpp *.c)
		FILE(GLOB HDR *.hpp *.h)

		# Required by all Qt classes containing Q_OBJECT in the declaration
		QT4_WRAP_CPP(MOC_SRCS ${HDR})

		# Required by all Qt (sub)classes using *.ui files, in this case the ACOSCDockWidget from libs/gui-osg-qt
		include_directories(${CMAKE_BINARY_DIR}/libs/gui-osg-qt)

		ADD_EXECUTABLE(qt+osc-touch-fingerpaint ${SRC} ${HDR} ${MOC_SRCS})

		ADD_DEPENDENCIES(qt+osc-touch-fingerpaint mediacycle-io-osc mediacycle-osg-qt)

		TARGET_LINK_LIBRARIES(qt+osc-touch-fingerpaint mediacycle-io-osc mediacycle-osg-qt ${QT_LIBRARIES})

		MESSAGE("[X] qt+osc-touch-fingerpaint")
	ENDIF()
ELSE()
	MESSAGE("[ ] qt+osc-touch-fingerpaint")
ENDIF()
