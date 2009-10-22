FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(tinyxml SHARED ${SRC} ${HDR})

#gsl - uncomment if needed
TARGET_LINK_LIBRARIES(tinyxml)

INSTALL(TARGETS tinyxml LIBRARY DESTINATION lib)
INSTALL(FILES ${HDR} DESTINATION include/tinyxml)

