FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_LIBRARY(vamp-hostsdk STATIC ${SRC} ${HDR})
INCLUDE_DIRECTORIES(. ..)
ADD_DEPENDENCIES(vamp-hostsdk vamp-sdk)

#gsl - uncomment if needed
TARGET_LINK_LIBRARIES(vamp-hostsdk vamp-sdk)
