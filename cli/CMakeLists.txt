FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

ADD_EXECUTABLE(mediacycle-cli ${SRC} ${HDR})
ADD_DEPENDENCIES(mediacycle-cli mediacycle)
INCLUDE_DIRECTORIES(. ../audiogarden)
INCLUDE_DIRECTORIES(. ../fftooura ../armadillo)

TARGET_LINK_LIBRARIES(mediacycle-cli mediacycle audiogarden)

INSTALL(TARGETS mediacycle-cli RUNTIME DESTINATION bin)
