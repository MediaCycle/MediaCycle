FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.h)

ADD_EXECUTABLE(MediaCycle ${SRC} ${HDR})

TARGET_LINK_LIBRARIES(MediaCycle cv cxcore highgui gsl gslcblas boost_system boost_filesystem armadillo TiCommon pthread)

