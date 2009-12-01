MESSAGE("gui/cocoa")

FILE(GLOB_RECURSE SRC *.m *.mm *.nib)
FILE(GLOB HDR *.h)

IF( USE_COCOA)

INCLUDE_DIRECTORIES(. ../osg)

SET(APP_TYPE MACOSX_BUNDLE)
ADD_EXECUTABLE(mediacycle-cocoa-osg ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${SRC} ${HDR})
ADD_DEPENDENCIES(mediacycle-cocoa-osg mediacycle)
ADD_DEPENDENCIES(mediacycle-cocoa-osg mediacycle-osg)
set_target_properties(mediacycle-cocoa-osg PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-cocoa/Info.plist)
set_source_files_properties(MainMenu.nib PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
#-lmediacycle -lTiCommon -lTiIo -lTiOsc -lTiRendering 
#-losg -losgDB -losgGA -losgViewer -lOpenThreads 
#-lboost_system -lcv -lcvaux -lhighgui
TARGET_LINK_LIBRARIES(mediacycle-cocoa-osg mediacycle)
TARGET_LINK_LIBRARIES(mediacycle-cocoa-osg mediacycle-osg)
TARGET_LINK_LIBRARIES(mediacycle-cocoa-osg TiCommon TiIo TiOsc TiRendering)
#TARGET_LINK_LIBRARIES(mediacycle-cocoa-osg TiCommon TiRendering) #OSC support pluginized
TARGET_LINK_LIBRARIES(mediacycle-cocoa-osg osg osgDB osgGA osgViewer)

set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
IF ( OPENGL_FOUND AND GLUT_FOUND)
	TARGET_LINK_LIBRARIES(mediacycle-cocoa-osg ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
ELSE ( OPENGL_FOUND AND GLUT_FOUND )
	MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
ENDIF ( OPENGL_FOUND AND GLUT_FOUND )

ENDIF( USE_COCOA)
