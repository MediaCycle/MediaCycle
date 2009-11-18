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
    IF ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY}) #CF OpenCV Framework support
    ELSE ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle cv cxcore highgui)
    ENDIF ( APPLE )
ENDIF ( USE_OPENCV )

#Boost
IF ( USE_BOOST_SERIALIZATION )
    IF ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle boost_system boost_filesystem)
    ELSE ( APPLE )
        TARGET_LINK_LIBRARIES(mediacycle boost_system boost_filesystem)
    ENDIF ( APPLE )
ENDIF ( USE_BOOST_SERIALIZATION )

# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)

