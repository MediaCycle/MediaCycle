FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(fftooura ${SRC} ${HDR})
SET_TARGET_PROPERTIES("fftooura" PROPERTIES FOLDER "${FOLDERNAME}")

#gsl - uncomment if needed
#TARGET_LINK_LIBRARIES(mediacycle gsl gslcblas)

#INSTALL(TARGETS fftooura LIBRARY DESTINATION lib)
#INSTALL(FILES ${HDR} DESTINATION include/fftooura)
MESSAGE("[X] fftooura")
