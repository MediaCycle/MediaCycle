file(GLOB yaafe_core_SOURCES *.cpp)
file(GLOB yaafe_core_HEADERS *.h)
list(APPEND yaafe_core_SOURCES DataFlowParser.l.c)

set(yaafe_core_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src_cpp/ ${DL_INCLUDE_DIR})
set(yaafe_core_LIBS ${DL_LIBRARY})

if (WITH_TIMERS)
    list(APPEND yaafe_core_LIBS ${RT_LIBRARY})
endif (WITH_TIMERS)

include_directories(${yaafe_core_INCLUDE_DIRS})
add_library (yaafe-core ${yaafe_core_SOURCES} ${yaafe_core_HEADERS})
set_target_properties(yaafe-core PROPERTIES VERSION ${YAAFE_LIBVERSION} SOVERSION ${YAAFE_API_VERSION})

target_link_libraries(yaafe-core ${yaafe_core_LIBS})

#CF install(TARGETS yaafe-core DESTINATION ${CMAKE_INSTALL_LIBDIR} EXPORT yaafe-targets)
#CF install(FILES DirectedGraph.h Ports.h Buffer.h Component.h ComponentHelpers.h DataFlow.h Engine.h utils.h ComponentFactory.h DESTINATION ${CMAKE_INSTALL_INCLUDE}/yaafe-core)

