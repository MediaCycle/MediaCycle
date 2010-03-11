FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(mediacycle STATIC ${SRC} ${HDR})

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

#others
TARGET_LINK_LIBRARIES(mediacycle TiCommon pthread sndfile)

#OpenCV
IF ( USE_OPENCV )
    IF ( UNIX )
        TARGET_LINK_LIBRARIES(mediacycle ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY})
    ELSE ( UNIX )
        TARGET_LINK_LIBRARIES(mediacycle cv cxcore highgui)
    ENDIF ( UNIX )
ENDIF ( USE_OPENCV )

#linking w/ Boost (searched for in main CMakeLists.txt)
IF ( Boost_SYSTEM_FOUND )
	TARGET_LINK_LIBRARIES(mediacycle ${Boost_SYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost system not found. needed in core mediacycle.")
ENDIF ( )
IF ( Boost_FILESYSTEM_FOUND )
	TARGET_LINK_LIBRARIES(mediacycle ${Boost_FILESYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost filesystem not found. needed in core mediacycle.")
ENDIF ( )

IF ( Boost_GRAPH_FOUND )
	TARGET_LINK_LIBRARIES(mediacycle ${Boost_GRAPH_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost graph not found. needed in core mediacycle.")
ENDIF ( )

# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)

