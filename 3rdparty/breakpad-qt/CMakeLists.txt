SET(TARGET_NAME "breakpad-qt")
IF(USE_BREAKPAD AND (USE_QT4 OR USE_QT5) )

	IF(APPLE)
		SET(PLATFORM "mac")
	ELSEIF(UNIX)
		SET(PLATFORM "linux")
	ELSE()
		SET(PLATFORM "windows")
		IF(MINGW)
			INCLUDE_DIRECTORIES(third-party/mingw)
		ENDIF()
	ENDIF()
        #MESSAGE("PLATFORM ${PLATFORM}")

	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} third-party/google-breakpad/src third-party/google-breakpad/src/client third-party/google-breakpad/src/common third-party/google-breakpad/src/google_breakpad third-party/google-breakpad/src/processor third-party/google-breakpad/src/tools)

	SET(${TARGET_NAME}_SRCS 
		${CMAKE_CURRENT_SOURCE_DIR}/handler/BreakpadHandler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/minidump_file_writer.cc
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/string_conversion.cc
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/convert_UTF.c
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/md5.cc
		${CMAKE_CURRENT_SOURCE_DIR}/sender/BreakpadHttpSender.cpp
	)

	IF(APPLE)
		SET(${TARGET_NAME}_SRCS ${${TARGET_NAME}_SRCS}
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/mac/crash_generation/crash_generation_client.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/mac/handler/exception_handler.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/mac/handler/minidump_generator.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/mac/handler/dynamic_images.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/bootstrap_compat.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/file_id.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/MachIPC.mm
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/macho_id.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/macho_utilities.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/macho_walker.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/mac/string_utilities.cc
		)

		IF(NOT ${MACPORTS_BUILD_ARCH} LESS "x86_64")
			SET(${TARGET_NAME}_SRCS ${${TARGET_NAME}_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/mac/handler/breakpad_nlist_64.cc)
		ENDIF()
	ELSEIF(UNIX)
		SET(${TARGET_NAME}_SRCS ${${TARGET_NAME}_SRCS}
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/crash_generation/crash_generation_client.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/handler/exception_handler.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/handler/minidump_descriptor.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/log/log.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/minidump_writer/linux_core_dumper.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/minidump_writer/linux_dumper.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/minidump_writer/linux_ptrace_dumper.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/linux/minidump_writer/minidump_writer.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/linux/elfutils.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/linux/file_id.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/linux/guid_creator.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/linux/linux_libc_support.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/linux/memory_mapped_file.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/linux/safe_readlink.cc
		)	
	ELSEIF(MINGW)
		SET(${TARGET_NAME}_SRCS ${${TARGET_NAME}_SRCS}
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/windows/crash_generation/crash_generation_client.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/windows/handler/exception_handler.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/client/windows/crash_generation/minidump_generator.cc
			${CMAKE_CURRENT_SOURCE_DIR}/third-party/google-breakpad/src/common/windows/guid_string.cc
		)
	ENDIF()

	SET(${TARGET_NAME}_HDRS 
		${CMAKE_CURRENT_SOURCE_DIR}/handler/BreakpadHandler.h
		${CMAKE_CURRENT_SOURCE_DIR}/sender/BreakpadHttpSender.h
	)

	# Generates ui_*.h files 
        #IF(USE_QT4)
        #	QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
        #ELSEIF(USE_QT5)
        #	QT5_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
        #ENDIF()
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
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	
        IF(USE_QT5)
                qt5_use_modules(${TARGET_NAME} Core Gui Widgets)
                #set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC ON)
                #target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        ENDIF()

	IF ( APPLE )
		TARGET_LINK_LIBRARIES(${TARGET_NAME} ${COREFOUNDATION_LIBRARY})
	ENDIF ( APPLE )
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
