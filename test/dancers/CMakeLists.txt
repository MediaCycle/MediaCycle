FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

FILE(GLOB MC_SRC ../../core/*.cpp ../../core/*.c)
FILE(GLOB MC_HDR ../../core/*.hpp ../../core/*.h)

ADD_EXECUTABLE(mediacycle-dancers ${SRC} ${HDR})
ADD_EXECUTABLE(mediacycle-dancers-2 ${SRC} ${HDR} ${MC_SRC} ${MC_HDR})

ADD_DEPENDENCIES(mediacycle-dancers mediacycle)

TARGET_LINK_LIBRARIES(mediacycle-dancers mediacycle)

#somehow OSX cannot link properly blas when using armadillo
IF ( APPLE )
    TARGET_LINK_LIBRARIES(mediacycle-dancers-2 blas)
ENDIF ( APPLE )
#others
TARGET_LINK_LIBRARIES(mediacycle-dancers-2 armadillo TiCommon pthread)

#OpenCV
IF ( USE_OPENCV )
    IF ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle-dancers-2 ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY})
    ELSE ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle-dancers-2 cv cxcore highgui)
    ENDIF ( APPLE )
ENDIF ( USE_OPENCV )

#Boost
IF ( USE_BOOST_SERIALIZATION )
    IF ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle-dancers-2 boost_system-mt boost_filesystem-mt)
    ELSE ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle-dancers-2 boost_system boost_filesystem)
    ENDIF ( APPLE )
ENDIF ( USE_BOOST_SERIALIZATION )


INSTALL(TARGETS mediacycle-dancers RUNTIME DESTINATION bin)

