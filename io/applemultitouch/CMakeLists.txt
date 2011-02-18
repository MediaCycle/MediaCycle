IF ( USE_APPLE_MULTITOUCH )

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../ ../../tinyxml)

	ADD_LIBRARY(mediacycle-io-applemultitouch STATIC ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-io-applemultitouch mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-io-applemultitouch mediacycle)

	# add "/System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport" to your application's CMAKE_EXE_LINKER_FLAGS
	MESSAGE("[X] mediacycle-io-applemultitouch")
ELSE()
	MESSAGE("[ ] mediacycle-io-applemultitouch")
ENDIF()
