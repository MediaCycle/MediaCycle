ADD_EXECUTABLE(mediacycle-avlc avlaughtercycle.cpp)
ADD_DEPENDENCIES(mediacycle-avlc mediacycle)

TARGET_LINK_LIBRARIES(mediacycle-avlc mediacycle)

INSTALL(TARGETS mediacycle-avlc RUNTIME DESTINATION bin)
