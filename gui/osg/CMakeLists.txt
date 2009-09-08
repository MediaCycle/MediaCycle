FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h)

ADD_LIBRARY(mediacycle-osg SHARED ${SRC} ${HDR})
ADD_DEPENDENCIES(mediacycle-osg mediacycle)

#-lmediacycle -lTiCommon -lTiIo -lTiOsc -lTiRendering 
#-losg -losgDB -losgGA -losgViewer -lOpenThreads 
#-lboost_system -lcv -lcvaux -lhighgui
TARGET_LINK_LIBRARIES(mediacycle-osg mediacycle)
TARGET_LINK_LIBRARIES(mediacycle-osg TiCommon TiIo TiOsc TiRendering)
TARGET_LINK_LIBRARIES(mediacycle-osg osg osgDB osgGA osgViewer)
TARGET_LINK_LIBRARIES(mediacycle-osg OpenThreads)
IF ( APPLE )
TARGET_LINK_LIBRARIES(mediacycle-osg boost_system-mt)
ELSEIF ( APPLE )
TARGET_LINK_LIBRARIES(mediacycle-osg boost_system)
ENDIF( APPLE )
TARGET_LINK_LIBRARIES(mediacycle-osg cv cvaux highgui)

#everything is already set in the first CMakeLists.txt (but the linkage of course) :
# -DUSE_OPENGL
# include_directories
IF ( OPENGL_FOUND AND GLUT_FOUND)
	TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
ELSE ( OPENGL_FOUND AND GLUT_FOUND )
	MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
ENDIF ( OPENGL_FOUND AND GLUT_FOUND )


