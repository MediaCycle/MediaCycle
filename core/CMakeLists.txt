FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(mediacycle SHARED ${SRC} ${HDR})
TARGET_LINK_LIBRARIES(mediacycle cv cxcore highgui gsl gslcblas boost_system boost_filesystem armadillo TiCommon pthread)
# this might require some path adaptation for Windows
# (e.g. set cmake_install_prefix to the good value for Windows )
# OSX is probably ok with installing lib in /usr/local/lib/
INSTALL(TARGETS mediacycle LIBRARY DESTINATION lib)
INSTALL(FILES MediaCycle.h DESTINATION include)

