# stk 4.4.4 from https://ccrma.stanford.edu/software/stk/download.html
SET(TARGET_NAME "stk")
IF(SUPPORT_AUDIO AND NOT STK_FOUND AND (NOT APPLE OR (COREAUDIO_FOUND AND COREFOUNDATION_FOUND)))

        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

        FILE(GLOB ${TARGET_NAME}_SRCS src/*.cpp)
        FILE(GLOB ${TARGET_NAME}_HDRS include/*.h)

        ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})
        #TARGET_LINK_LIBRARIES(${TARGET_NAME})

	IF(APPLE)
		ADD_DEFINITIONS("-DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__")
		#-DRAWWAVE_PATH=\"../../rawwaves/\"
		#__UNIX_JACK__
                TARGET_LINK_LIBRARIES(${TARGET_NAME} ${COREFOUNDATION_LIBRARY} ${COREAUDIO_LIBRARY})
	ELSEIF(UNIX)
		ADD_DEFINITIONS("-DHAVE_GETTIMEOFDAY -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__")
		#__LINUX_ALSA__, __UNIX_JACK__, and/or __LINUX_OSS__
	ELSE()
		ADD_DEFINITIONS("-DHAVE_GETTIMEOFDAY -D__WINDOWS_ASIO__ -D__LITTLE_ENDIAN__")
		#__WINDOWS_DS__ to __WINDOWS_ASIO__, __WINDOWS_MM__
		#3. Under Link > General:  Add winmm.lib, dsound.lib, and Wsock32.lib to the end of the Object/library modules list.
	ENDIF()

	SET_TARGET_PROPERTIES("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()