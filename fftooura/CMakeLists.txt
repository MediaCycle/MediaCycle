FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

INCLUDE_DIRECTORIES(.)
ADD_LIBRARY(fftooura STATIC ${SRC} ${HDR})

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

#INSTALL(TARGETS fftooura LIBRARY DESTINATION lib)
#INSTALL(FILES ${HDR} DESTINATION include/fftooura)
MESSAGE("[X] fftooura")
