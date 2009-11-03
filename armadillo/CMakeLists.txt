FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(armadillo-utils SHARED ${SRC} ${HDR})

#gsl - uncomment if needed
TARGET_LINK_LIBRARIES(armadillo-utils fftooura armadillo atlas lapack)
INCLUDE_DIRECTORIES(. ../fftooura)

INSTALL(TARGETS armadillo-utils LIBRARY DESTINATION lib)
INSTALL(FILES ${HDR} DESTINATION include/armadillo-utils)

