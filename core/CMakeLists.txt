FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(mediacycle STATIC ${SRC} ${HDR})

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

#somehow OSX cannot link properly blas when using armadillo
IF ( APPLE )
    TARGET_LINK_LIBRARIES(mediacycle blas)
ENDIF ( APPLE )
#others
TARGET_LINK_LIBRARIES(mediacycle armadillo TiCommon pthread)

#OpenCV
IF ( USE_OPENCV )
    IF ( UNIX )
        TARGET_LINK_LIBRARIES(mediacycle ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY})
    ELSE ( UNIX )
        TARGET_LINK_LIBRARIES(mediacycle cv cxcore highgui)
    ENDIF ( UNIX )
ENDIF ( USE_OPENCV )

#Boost
FIND_PACKAGE ( Boost COMPONENTS system )
IF ( Boost_SYSTEM_FOUND )
	MESSAGE( "Found boost system in ${Boost_SYSTEM_LIBRARY}" )
	TARGET_LINK_LIBRARIES(mediacycle ${Boost_SYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost system not found.")
ENDIF ( )
FIND_PACKAGE ( Boost COMPONENTS filesystem )
IF ( Boost_FILESYSTEM_FOUND )
	MESSAGE( "Found boost filesystem in ${Boost_FILESYSTEM_LIBRARY}" )
	TARGET_LINK_LIBRARIES(mediacycle ${Boost_FILESYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost filesystem not found.")
ENDIF ( )

# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)

