FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h)

IF ( USE_OSG )
ADD_LIBRARY(mediacycle-osg STATIC ${SRC} ${HDR})
ADD_DEPENDENCIES(mediacycle-osg mediacycle)

#-lmediacycle -lTiCommon -lTiIo -lTiOsc -lTiRendering 
#-losg -losgDB -losgGA -losgViewer -lOpenThreads 
#-lboost_system -lcv -lcvaux -lhighgui
TARGET_LINK_LIBRARIES(mediacycle-osg mediacycle)
TARGET_LINK_LIBRARIES(mediacycle-osg TiCommon TiIo TiOsc TiRendering)
TARGET_LINK_LIBRARIES(mediacycle-osg osg osgDB osgGA osgViewer osgText)
TARGET_LINK_LIBRARIES(mediacycle-osg OpenThreads)

FIND_PACKAGE ( Boost COMPONENTS system )
IF ( Boost_SYSTEM_FOUND )
	MESSAGE( "Found boost system in ${Boost_SYSTEM_LIBRARY}" )
	TARGET_LINK_LIBRARIES(mediacycle-osg ${Boost_SYSTEM_LIBRARY})
ELSE ( )
	MESSAGE(FATAL_ERROR "Boost system not found.")
ENDIF ( )

TARGET_LINK_LIBRARIES(mediacycle-osg cv cvaux highgui)

#everything is already set in the first CMakeLists.txt (but the linkage of course) :
# -DUSE_OPENGL
# include_directories
IF ( OPENGL_FOUND AND GLUT_FOUND)
	TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
ELSE ( OPENGL_FOUND AND GLUT_FOUND )
	MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
ENDIF ( OPENGL_FOUND AND GLUT_FOUND )

ENDIF ( USE_OSG )

