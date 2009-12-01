FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

INCLUDE_DIRECTORIES(.)

ADD_LIBRARY(audioengine STATIC ${SRC} ${HDR})
ADD_DEPENDENCIES(audioengine mediacycle)

TARGET_LINK_LIBRARIES(audioengine mediacycle)
