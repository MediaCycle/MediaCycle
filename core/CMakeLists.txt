FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

# ACClusterPositionsPropellerPlugin.cpp makes boost (1.52+?) clash with boost/mpl/aux_/preprocessed/gcc/bind.hpp if -ftemplate-depth is at 30 by default
# ACFeaturesType.h makes boost (1.52) clash with #include <boost/numeric/ublas/vector_sparse.hpp> if -ftemplate-depth is at 60 (OSX 10.8.3 Xcode 4.6.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-90")

ADD_LIBRARY(mediacycle ${SRC} ${HDR})

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)
IF(UNIX AND NOT APPLE)
	TARGET_LINK_LIBRARIES(mediacycle dl)
ENDIF()

IF(TINYXML_FOUND)
	TARGET_LINK_LIBRARIES(mediacycle ${TINYXML_LIBRARY})
ELSE()
	ADD_DEPENDENCIES(mediacycle tinyxml)
	TARGET_LINK_LIBRARIES(mediacycle tinyxml)
ENDIF()

ADD_DEPENDENCIES(mediacycle armadillo-utils)
TARGET_LINK_LIBRARIES(mediacycle armadillo-utils)

IF (APPLE)
	TARGET_LINK_LIBRARIES(mediacycle stdc++)# CF otherwise "make package" fails
ENDIF(APPLE)
	
IF ( SDIF_FOUND )
	TARGET_LINK_LIBRARIES(mediacycle sdif)
ENDIF ( )

#others
TARGET_LINK_LIBRARIES(mediacycle pthread)
IF ( MINGW )
	TARGET_LINK_LIBRARIES(mediacycle wsock32 ws2_32) # CF: should be detected in main CMakeLists.txt
ENDIF ( MINGW )

TARGET_LINK_LIBRARIES(mediacycle ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_GRAPH_LIBRARY} ${Boost_THREAD_LIBRARY})

# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
##INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
##INSTALL(FILES ${HDR} DESTINATION include/mediacycle)
