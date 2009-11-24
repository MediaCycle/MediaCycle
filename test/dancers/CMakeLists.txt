FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

#FILE(GLOB MC_SRC ../../core/*.cpp ../../core/*.c)
#FILE(GLOB MC_HDR ../../core/*.hpp ../../core/*.h)

ADD_EXECUTABLE(mediacycle-dancers ${SRC} ${HDR})
#ADD_EXECUTABLE(mediacycle-dancers-2 ${SRC} ${HDR} ${MC_SRC} ${MC_HDR})

ADD_DEPENDENCIES(mediacycle-dancers mediacycle eyesweb)

TARGET_LINK_LIBRARIES(mediacycle-dancers mediacycle)
TARGET_LINK_LIBRARIES(mediacycle-dancers tinyxml armadillo-utils)

INCLUDE_DIRECTORIES(. ../../tinyxml)

#somehow OSX cannot link properly blas when using armadillo
IF ( APPLE )
    TARGET_LINK_LIBRARIES(mediacycle-dancers blas)
ENDIF ( APPLE )
#others
TARGET_LINK_LIBRARIES(mediacycle-dancers armadillo TiCommon pthread)

#OpenCV
IF ( USE_OPENCV )
    IF ( UNIX ) #AM : works on OSX and linux (don't know about win32)
        TARGET_LINK_LIBRARIES(mediacycle-dancers ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY})
    ELSE ( UNIX )
        TARGET_LINK_LIBRARIES(mediacycle-dancers cv cxcore highgui)
    ENDIF ( UNIX )
ENDIF ( USE_OPENCV )

#Boost
FIND_PACKAGE ( Boost COMPONENTS system )
IF ( Boost_SYSTEM_FOUND )
	MESSAGE( "Found boost system in ${Boost_SYSTEM_LIBRARY}" )
	TARGET_LINK_LIBRARIES(mediacycle-dancers ${Boost_SYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost system not found.")
ENDIF ( )
FIND_PACKAGE ( Boost COMPONENTS filesystem )
IF ( Boost_FILESYSTEM_FOUND )
	MESSAGE( "Found boost filesystem in ${Boost_FILESYSTEM_LIBRARY}" )
	TARGET_LINK_LIBRARIES(mediacycle-dancers ${Boost_FILESYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost filesystem not found.")
ENDIF ( )


INSTALL(TARGETS mediacycle-dancers RUNTIME DESTINATION bin)

