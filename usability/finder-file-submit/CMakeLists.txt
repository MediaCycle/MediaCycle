# Inspired from cdto by Jay Tuley
# http://code.google.com/p/cdto
SET(TARGET_NAME "finder-file-submit")
IF(APPLE AND COCOA_FOUND AND CURL_FOUND)
	FILE(GLOB SRC *.cpp *.m)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES("/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/")
	INCLUDE_DIRECTORIES(${COCOA_INCLUDE_DIR})

        SET(ICON_NAME "MediaCycleFinderFileSubmit")
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME ${TARGET_NAME})
	# set how it shows up in the Info.plist file
        SET(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}.icns)
	# set the bundle identifier (REQUIRED, or some strange GUI bugs may appear)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "org.mediacycle.${TARGET_NAME}")
	SET(MACOSX_BUNDLE_INFO_STRING "${TARGET_NAME}")
	SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${TARGET_NAME}")
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${TARGET_NAME}")
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "${MediaCycle_VERSION}")
	SET(MACOSX_BUNDLE_COPYRIGHT "Copyright UMONS 2008-present")
	# set where in the bundle to put the icns file
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/${ICON_NAME}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	# include the icns file in the target
	SET(SRC ${SRC} ${CMAKE_CURRENT_SOURCE_DIR}/${ICON_NAME}.icns)
	SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})

	ADD_EXECUTABLE(${TARGET_NAME} ${OS_SPECIFIC} ${SRC} ${HDR})

        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${CURL_LIBRARIES})
	SET(CMAKE_EXE_LINKER_FLAGS "")
	#AppKit Cocoa CoreData Foundation ScriptingBridge
	set(CMAKE_EXE_LINKER_FLAGS "-framework AppKit -framework Cocoa -framework CoreData -framework Foundation -framework ScriptingBridge")

	IF(NOT USE_DEBUG)
		SET(PROGNAME ${TARGET_NAME})
		include(${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
        MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
