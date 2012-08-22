SET(TARGET_NAME "breakpad-qt")
IF(USE_BREAKPAD AND USE_QT4 )

	IF(APPLE)
		SET(PLATFORM "mac")
	ELSEIF(UNIX)
		SET(PLATFORM "linux")
	ELSE()
		SET(PLATFORM "winwows")
	ENDIF()
	MESSAGE("PLATFORM ${PLATFORM}")

	INCLUDE_DIRECTORIES(third-party/google-breakpad/src third-party/google-breakpad/src/client third-party/google-breakpad/src/common third-party/google-breakpad/src/google_breakpad third-party/google-breakpad/src/processor third-party/google-breakpad/src/tools)

	INCLUDE_DIRECTORIES(.)

	SET(${TARGET_NAME}_SRCS 
		handler/BreakPadHandler.cpp
		third-party/google-breakpad/src/client/minidump_file_writer.cc
		third-party/google-breakpad/src/common/string_conversion.cc
		third-party/google-breakpad/src/common/convert_UTF.c
		third-party/google-breakpad/src/common/md5.c
		third-party/google-breakpad/src/client/mac/handler/exception_handler.cc
		third-party/google-breakpad/src/client/mac/handler/minidump_generator.cc
		third-party/google-breakpad/src/client/mac/handler/dynamic_images.cc
		third-party/google-breakpad/src/common/mac/string_utilities.cc
		third-party/google-breakpad/src/common/mac/file_id.cc
		third-party/google-breakpad/src/common/mac/macho_id.cc
		third-party/google-breakpad/src/common/mac/macho_utilities.cc
		third-party/google-breakpad/src/common/mac/macho_walker.cc
		sender/BreakpadHttpSender.cpp
	)

	SET(${TARGET_NAME}_HDRS 
		handler/BreakPadHandler.h
		sender/BreakpadHttpSender.h
	)

	# Generates ui_*.h files 
	#QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
	# Generates moc_*.cxx files
	QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed	

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(${TARGET_NAME} STATIC ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS} ${${TARGET_NAME}_RCC_SRCS})
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} ${OPENSSL_LIBRARIES})
	
	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
	ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()