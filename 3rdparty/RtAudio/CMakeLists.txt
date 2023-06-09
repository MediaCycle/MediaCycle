# RtAudio 4.0.12 from http://www.music.mcgill.ca/~gary/rtaudio/
SET(TARGET_NAME "RtAudio")
IF(SUPPORT_AUDIO AND NOT RTAUDIO_FOUND AND ( ALSA_FOUND OR (APPLE AND COREAUDIO_FOUND AND COREFOUNDATION_FOUND)))

        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/3rdparty/RtError)

	IF(APPLE OR UNIX)
        	FILE(GLOB ${TARGET_NAME}_SRCS *.cpp)
        	FILE(GLOB ${TARGET_NAME}_HDRS *.h)
	ELSE()
        	FILE(GLOB ${TARGET_NAME}_SRCS *.cpp include/*.cpp)
        	FILE(GLOB ${TARGET_NAME}_HDRS *.h include/*.h)
	ENDIF()

        ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})
        #TARGET_LINK_LIBRARIES(${TARGET_NAME})

	IF(APPLE)
		ADD_DEFINITIONS("-DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__")
		#-DRAWWAVE_PATH=\"../../rawwaves/\"
		#__UNIX_JACK__
                TARGET_LINK_LIBRARIES(${TARGET_NAME} ${COREFOUNDATION_LIBRARY} ${COREAUDIO_LIBRARY} ${COREMIDI_LIBRARY})
	ELSEIF(UNIX)
		ADD_DEFINITIONS("-DHAVE_GETTIMEOFDAY -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__")
		#__LINUX_ALSA__, __UNIX_JACK__, and/or __LINUX_OSS__
		TARGET_LINK_LIBRARIES(${TARGET_NAME} ${ALSA_LIBRARIES})

	ELSE()
		ADD_DEFINITIONS("-DHAVE_GETTIMEOFDAY -D__WINDOWS_DS__ -D__WINDOWS_MM__ -D__LITTLE_ENDIAN__")
		#__WINDOWS_DS__ to __WINDOWS_ASIO__, __WINDOWS_MM__
		#3. Under Link > General:  Add winmm.lib, dsound.lib, and Wsock32.lib to the end of the Object/library modules list.
		TARGET_LINK_LIBRARIES(${TARGET_NAME} winmm dsound.lib wsock32)
	ENDIF()

        IF(CMAKE_CAN_APPEND_PROPERTIES)
            set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src;${CMAKE_CURRENT_SOURCE_DIR}/src/include")
        ENDIF()
        SET_TARGET_PROPERTIES("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
