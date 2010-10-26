/*
 *  ACAudioCycleOsgiPad.mm
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 26/10/10
 *
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 *  Adapted from Thomas Hogarth's 2009 examples from:
 *  http://github.com/stmh/osg/tree/iphone
 */

#import "ACAudioCycleOsgiPad.h"
#include <osgGA/TrackballManipulator>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osg/ShapeDrawable>
#include <osg/DisplaySettings>

#define kAccelerometerFrequency		30.0 // Hz
#define kFilteringFactor			0.1
#define PI 3.1415926535897932384626433832795f

@implementation ACAudioCycleOsgiPad

//
//Called once app has finished launching, create the viewer then realize. Can't call viewer->run as will 
//block the final inialization of the windowing system
//
- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
    osg::setNotifyLevel(osg::INFO);
	
	_root = new osg::MatrixTransform();	
    
    // try msaa. available for iOS >= 4.0
    osg::DisplaySettings* settings = osg::DisplaySettings::instance();
    settings->setNumMultiSamples(4);
	
	unsigned int w(640);
	unsigned int h(480);
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if (wsi) {
		wsi->getScreenResolution(0, w, h);
	}
	
	_viewer = new osgViewer::Viewer();
	
	osg_view = new osgViewer::GraphicsWindowEmbedded(0,0,w,h);
	
	 _viewer->getCamera()->setViewport(new osg::Viewport(0.0f,0.0f,w,h));
	 _viewer->getCamera()->setProjectionMatrixAsPerspective(45.0f, _viewer->getCamera()->getViewport()->aspectRatio(), 0.001f, 10.0f);
	 _viewer->getCamera()->getViewMatrix().makeIdentity();
	 _viewer->getCamera()->setViewMatrixAsLookAt(Vec3(0.0f,0.0f,1.0f), Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,1.0f,0.0f));
	
	// CF debug this
	//_viewer->getCamera()->setGraphicsContext(wsi->getGraphicsWindow());//CF!!!!!! check
	 
	renderer = new ACOsgBrowserRenderer();
	
	media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");

	
	renderer->setMediaCycle(media_cycle);
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	_viewer->addEventHandler(event_handler);
	
	_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);//SingleThreaded DrawThreadPerContext
	_viewer->realize();
	
	osg::setNotifyLevel(osg::INFO);
	
	[NSTimer scheduledTimerWithTimeInterval:1.0/30.0 target:self selector:@selector(updateScene) userInfo:nil repeats:YES]; 
	
	//Configure and start accelerometer
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	
	audio_engine = new ACAudioEngine();
	audio_engine->setMediaCycle(media_cycle);
	
	media_cycle->setClusterNumber(5);
}


//
//Timer called function to update our scene and render the viewer
//
- (void)updateScene {
	
	//std::cout<<"Update scene" << std::endl;
	 double frac = 0.0;
	 
	 if(media_cycle && media_cycle->hasBrowser())
	 {
	 media_cycle->updateState();
	 frac = media_cycle->getFrac();
	 }
	 
	 if (!media_cycle->getNeedsDisplay()) {
	 return;
	 }

	//CF debug the commented parts
	/*
	 if(_viewer->getCamera() && media_cycle && media_cycle->hasBrowser())
	 {
	 
	 float x=0.0f, y=0.0f, zoom, angle;
	 float upx, upy;
	 
	 zoom = media_cycle->getCameraZoom();
	 angle = media_cycle->getCameraRotation();
	 media_cycle->getCameraPosition(x, y);
	 upx = cos(-angle+PI/2);
	 upy = sin(-angle+PI/2);		
	 
	 _viewer->getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));
	 }
	 */
	
	/*
	 [self updateTransformsFromBrowser:frac];
	 */
	 
	/*

	if (media_cycle && media_cycle->hasBrowser() && media_cycle->getBrowser()->getState() == AC_CHANGING) {
		if (media_cycle->getNeedsDisplay()) {
			[self updateTransformsFromBrowser:media_cycle->getFrac()];
		}
	}
	*/
	
	_viewer->frame();
	
	
	media_cycle->setNeedsDisplay(false);
}


- (void)applicationWillResignActive:(UIApplication *)application {

}


- (void)applicationDidBecomeActive:(UIApplication *)application {


	[self prepareFromBrowser];
	media_cycle->setNeedsDisplay(true);
	
	//NSString *filePath = [[NSBundle mainBundle] pathForResource:@"zero-g-pro-pack-small-mc-ipad" ofType:@"acl"];  
	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"dummy-ipad" ofType:@"acl"];  
	media_cycle->importACLLibrary((string)[filePath UTF8String]);
	media_cycle->normalizeFeatures();
	media_cycle->libraryContentChanged();
	[self updatedLibrary];
	
	//CF: for the moment, there's just a grey node, the following needs be debugged:
	/*
	[self updateTransformsFromBrowser:0];
	media_cycle->setNeedsDisplay(true);
	[self updateScene];
	  */
}


-(void)applicationWillTerminate:(UIApplication *)application{
	delete event_handler;
	delete renderer;
	delete audio_engine;
	delete media_cycle;
	_root = NULL;
	_viewer = NULL;
} 

//
//Accelorometer
//
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
	accel[0] = acceleration.x * kFilteringFactor + accel[0] * (1.0 - kFilteringFactor);
	accel[1] = acceleration.y * kFilteringFactor + accel[1] * (1.0 - kFilteringFactor);
	accel[2] = acceleration.z * kFilteringFactor + accel[2] * (1.0 - kFilteringFactor);
}

- (void)prepareFromBrowser
{
	renderer->prepareNodes();
	renderer->prepareLabels();
		
	//CF Cocoa style
	//[self setNode:_privateData->renderer.getShapes()];//CF
	//[self setNode:renderer->getShapes()];//C
		
	//CF Qt style
	_viewer->setSceneData(renderer->getShapes());//CF	
	
	//CF Thomas Hogarth style
	//_root->addChild(renderer->getShapes());
	//_viewer->setSceneData(_root.get());
}

- (void)updateTransformsFromBrowser:(double)frac
{
	int closest_node;
	// get screen coordinates
	closest_node = renderer->computeScreenCoordinates(_viewer, frac);//CF
	media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	renderer->updateNodes(frac); // animation time in [0,1] //CF
	renderer->updateLabels(frac);//CF
}

- (void)updatedLibrary
{	

	media_cycle->setReferenceNode(0);//CF we want to debug the view and positions using the same layout at each relaunch!
	
	media_cycle->getBrowser()->setState(AC_CHANGING);
	media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	
	[self prepareFromBrowser];
	media_cycle->setNeedsDisplay(true);
	
	
}
- (void)dealloc {
	delete event_handler;
	delete renderer;
	delete audio_engine;
	delete media_cycle;
	_root = NULL;
	_viewer = NULL;
	[super dealloc];
}

@end
