//
//  ACImageBrowserOsgView.m
//  AudioCycle
//
//  @author Stéphane Dupont
//  @date 24/02/08
//  @copyright (c) 2009 – UMONS - Numediart
//  
//  MediaCycle of University of Mons – Numediart institute is 
//  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
//  licence (the “License”); you may not use this file except in compliance 
//  with the License.
//  
//  This program is free software: you can redistribute it and/or 
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  Each use of this software must be attributed to University of Mons – 
//  Numediart Institute
//  
//  Any other additional authorizations may be asked to avre@umons.ac.be 
//  <mailto:avre@umons.ac.be>
//

#import "ACOsgBrowserViewCocoa.h"
#import "ACOsgViewNodeCocoa.h"

/*
struct ACOsgBrowserViewData
{
	ACOsgBrowserRenderer renderer;
};
*/

@implementation ACOsgBrowserViewCocoa

+ (NSOpenGLPixelFormat*)defaultPixelFormat
{
	NSOpenGLPixelFormatAttribute attributes [] = { // 1
		NSOpenGLPFAWindow,
		//NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 32,
		NSOpenGLPFASampleBuffers, 1,
		NSOpenGLPFASamples, 4,
		NSOpenGLPFANoRecovery,
		(NSOpenGLPixelFormatAttribute)nil
        };
	return [[[NSOpenGLPixelFormat alloc]
		 initWithAttributes:attributes] autorelease]; // 2
}

#pragma mark Main
- (void)setMediaCycle:(MediaCycle*)_media_cycle
{
	//NSLog(@"setMediaCycle");
	media_cycle = _media_cycle;
	////_privateData->renderer.setMediaCycle(media_cycle);//CF
	renderer->setMediaCycle(media_cycle);//CF
	renderer_hud->setMediaCycle(media_cycle);
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	[self addEventHandler:(event_handler)];
}

- (void)initCommonACBrowserOsgView
{	
	//NSLog(@"initCommonACBrowserOsgView");
	////_privateData = new ACOsgBrowserViewData();
	renderer = new ACOsgBrowserRenderer();//CF
	renderer_hud = new ACOsgHUDRenderer();
	//_privateData->renderer.setMediaCycle(media_cycle);
	
	// to get multisample, we have to do it by code...
	id pf = [[self class] defaultPixelFormat];
	[self setPixelFormat:pf];
}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
	NSLog(@"initWithFrame:...");
	
	if(self = [super initWithFrame:frameRect pixelFormat:format])
	{
		[self initCommonACBrowserOsgView];
	}
	
	return self;
}

- (void)dealloc
{
	//[[NSNotificationCenter defaultCenter] removeObserver:self];//CF should have been used for the resize event
	////delete _privateData; //CF
	[super dealloc];
}

// required to get mouseMoved: calls
- (BOOL)acceptsFirstResponder { return YES; }

- initWithCoder:coder
{
	NSLog(@"initWithCoder:...");
	if(self = [super initWithCoder:coder])
	{
		[self initCommonACBrowserOsgView];
	}
	
	return self;
}


- (void)awakeFromNib
{
	//NSLog(@"awakeFromNib");
	
	[[self window] setAcceptsMouseMovedEvents:YES];
	
	mousedown = 0;
	zoomdown = 0;
	forwarddown = 0;
	autoplaydown = 0;
}

- (void)prepareOpenGL
{
	//NSLog(@"ACImageBrowserOsgView's prepareOpenGL (0x%x)", self);
	//NSLog(@"pixelFormat : %@", [self pixelFormat]);
	[super prepareOpenGL];
	//[self setCameraManipulator:NULL];
	
	osg::Camera *camera = [self camera];
	
	if(camera)
	{
		//camera->setViewport(0, 0, 300, 300);
		/*camera->setProjectionMatrixAsPerspective(
							 45.0, 
							 camera->getViewport()->aspectRatio(),
							 0.001, 
							 10.0
							 );*/
		camera->getViewMatrix().makeIdentity();
		//camera->setViewMatrixAsLookAt(eye, dir, up);
		camera->setViewMatrixAsLookAt(osg::Vec3(0,0,0.8), osg::Vec3(0,0,0), osg::Vec3(0,1,0));
	}
	else
	{
		NSLog(@"no camera");
	}
}

- (void)drawRect:(NSRect)rect
{	
	//std::cout << "drawRect" << std::endl;
	//NSLog(@"ACImageBrowserOsgView's drawRect (0x%x)", self);
	double frac = 0.0;
	
	if(media_cycle && media_cycle->hasBrowser())
	{
		media_cycle->updateState();
		
		frac = media_cycle->getFrac();
	}
	
	if (!media_cycle->getNeedsDisplay()) {
		return;
	}	
	
	osg::Camera *camera = [self camera];
	
	if(camera && media_cycle)
	{
		
		float x=0.0, y=0.0, zoom, angle;
		float upx, upy;
		
		zoom = media_cycle->getCameraZoom();
		angle = media_cycle->getCameraRotation();
		media_cycle->getCameraPosition(x, y);
		
		upx = cos(-angle+pi/2);
		upy = sin(-angle+pi/2);		
		
		camera->setViewMatrixAsLookAt(osg::Vec3(x*1.0,y*1.0,0.8 / zoom), osg::Vec3(x*1.0,y*1.0,0), osg::Vec3(upx, upy, 0));
	}
	
	[self updateTransformsFromBrowser:frac];
	[super drawRect:rect];
	
	if (media_cycle->getNeedsDisplay3D()) {
		media_cycle->setNeedsDisplay(true);
		media_cycle->setNeedsDisplay3D(false);
	}
	else {
		media_cycle->setNeedsDisplay(false);
	}
	
	//glSwapAPPLE();
}

- (void)viewDidMoveToWindow
{
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowResized:) name:NSWindowDidResizeNotification object:[self window]];	
}

- (void)windowResized:(NSNotification *)notification;
{
	NSSize size = [[self window] frame].size;
	media_cycle->setNeedsDisplay(true);
	//NSLog(@"window width = %f, window height = %f", size.width, size.height);
}


- (void)keyDown:event
{	
	[super keyDown:event];
	
	NSString *theArrow = [event charactersIgnoringModifiers];
	unichar keyChar = 0;
	if ( [theArrow length] == 0 )
		return;            // reject dead keys
	if ( [theArrow length] == 1 ) {
		keyChar = [theArrow characterAtIndex:0];
		//std::cout << "Key (Cocoa) " << keyChar << std::endl;
		if ( keyChar == 122 ) {		// 'z'
			zoomdown = 1;
			return;
		}
		if ( keyChar == 97 ) {		// 'a'
			media_cycle->setForwardDown(1);
			forwarddown = 1;
			return;
		}
		if ( keyChar == 113 ) {		// 'q'
			media_cycle->setAutoPlay(1);
			autoplaydown = 1;
			return;
		}
		if ( keyChar == 109 ) {		// 'm'
			media_cycle->muteAllSources();
			return;
		}
	}
}

- (void)keyUp:event
{
	[super keyUp:event];
	zoomdown = 0;
	forwarddown = 0;
	media_cycle->setForwardDown(0);
	autoplaydown = 0;
	media_cycle->setAutoPlay(0);
}

- (void)mouseDown:event
{
	[super mouseDown:event];
	
	if ([event clickCount] == 1 && media_cycle && media_cycle->hasBrowser() && forwarddown==1)
	{
                int media = media_cycle->getClickedNode();
                NSLog(@"click on %d when 'a' key pressed", media);
		
                if(media >= 0)
		{
			// SD MIC demo - store need to be done befoe increment I think
			// XSCF 250310 added these 3
			if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
				media_cycle->storeNavigationState();
			
			if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
                                media_cycle->incrementNavigationLevels(media);
                        media_cycle->setReferenceNode(media);
			
			//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
			//media_cycle->getBrowser()->setState(AC_CHANGING);
			
			//			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
			//			media_cycle->getBrowser()->setState(AC_CHANGING);
			
			media_cycle->updateDisplay(true); //XS250310 was: media_cycle->updateClusters(true);
			// XSCF 250310 removed this:
			// media_cycle->updateNeighborhoods();
//			media_cycle->updateClusters(false);// was true
//			
//			// remainders from updateClusters(true)
//			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
//			media_cycle->getBrowser()->setState(AC_CHANGING);
			media_cycle->setNeedsDisplay(true);
		}
	}
		
	NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	mousedown = 1;
	refx = eventLocation.x; 
	refy = eventLocation.y;
	media_cycle->getCameraPosition(refcamx, refcamy);
	refzoom = media_cycle->getCameraZoom();
	
	media_cycle->setNeedsDisplay(true);
}

- (void)mouseMoved:(NSEvent*)event
{
	[super mouseMoved:event];
	
	media_cycle->setNeedsDisplay(true);
}

- (void) mouseDragged:(NSEvent*) event
{
	float zoom, angle;
	float xmove, ymove, xmove2, ymove2;
	float x, y;
	NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	x = eventLocation.x; 
	y = eventLocation.y;
	if (mousedown) {
		if (zoomdown) {
			media_cycle->setCameraZoom(refzoom + (y-refy)/50);
		}
		else {
			zoom = media_cycle->getCameraZoom();
			angle = media_cycle->getCameraRotation();
			xmove = (refx-x);
			ymove = (refy-y);
			xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
			ymove2 = ymove*cos(-angle)+xmove*sin(-angle);		
			media_cycle->setCameraPosition(refcamx + xmove2/800/zoom , refcamy + ymove2/800/zoom);
			//NSLog(@"zoom %f eventLocations: %f %f and setCameraPosition: %f %f",zoom,eventLocation.x,eventLocation.y,refcamx + xmove2/800/zoom , refcamy + ymove2/800/zoom);
		}
	}
	
	media_cycle->setNeedsDisplay(true);
}

- (void)mouseUp:(NSEvent*)event
{
	[super mouseUp:event];
	
	if(media_cycle && media_cycle->hasBrowser()) {
		media_cycle->setClickedNode(-1);
	}
	
	mousedown = 0;
	
	media_cycle->setNeedsDisplay(true);
}

// SD - Macbook Pro Trackpad Gestures
//		This is not documented by Apple
//		More should come with the OSX 10.6 API
// 2-finger zoom
- (void)magnifyWithEvent:(NSEvent*)event
{
	float deltaZ = [event deltaZ];
	//float ref  = [event standardMagnificationThreshold];
	refzoom = media_cycle->getCameraZoom();
	media_cycle->setCameraZoom(refzoom + (deltaZ)/100);
	refzoom = refzoom + (deltaZ)/100;

}

- (void)rotateWithEvent:(NSEvent*)event
{
	float rotation = [event rotation];
	refrotation = media_cycle->getCameraRotation();
	media_cycle->setCameraRotation(refrotation + (rotation)/25);
	refrotation = refrotation + (rotation)/25;
	//float ref  = [event standardRotationThreshold];
}

// 3 points swipe
- (void)swipeWithEvent:(NSEvent*)event
{
	/*
	float zoom, angle;
	float xmove, ymove, xmove2, ymove2;
	float x, y;
	float deltaX = [event deltaX];
	float deltaY = [event deltaY];
	xmove2 = deltaX;
	ymove2 = deltaY;
	zoom = audio_cycle->getAudioBrowser()->getCameraZoom();
	audio_cycle->getAudioBrowser()->getCameraPosition(x, y);
	audio_cycle->getAudioBrowser()->setCameraPosition(refcamx+xmove2/80/zoom, refcamy+ymove2/80/zoom);
	 */
}

- (void)scrollWheel:(NSEvent*)event
{
	//CF to disable the jog wheel outer wheel scrolling
/*	
	float zoom, angle;
	float xmove, ymove, xmove2, ymove2;
	float x, y;	
	float deltaX = [event deltaX];
	float deltaY = [event deltaY];
//	float deltaZ = [event deltaZ];
	xmove = deltaX;
	ymove = -deltaY;
	zoom = media_cycle->getCameraZoom();
	angle = media_cycle->getCameraRotation();
	xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
	ymove2 = ymove*cos(-angle)+xmove*sin(-angle);		
	media_cycle->getCameraPosition(x, y);
	media_cycle->setCameraPosition(x+xmove2/400/zoom, y+ymove2/400/zoom);
 */
}

- (void)prepareFromBrowser
{
	NSLog(@"adding node");
	
	@synchronized(self)
	{
		
	/////_privateData->renderer.prepareNodes();//CF
	////_privateData->renderer.prepareLabels();//CF
	renderer->prepareNodes();
	renderer->prepareLabels();		
	renderer_hud->preparePointers();
		
	//[self setNode:_privateData->renderer.getShapes()];//CF
	[self setNode:renderer->getShapes()];//CF	
	[self addCamera:renderer_hud->getCamera()];
	}
}

- (void)updateTransformsFromBrowser:(double)frac
{
	int closest_node;

	osgViewer::Viewer* view = [self viewer];
	
	@synchronized(self)
	{
		// SD 2010 OCT
		renderer->prepareNodes();
		renderer->prepareLabels();
		renderer_hud->preparePointers();
		
		// get screen coordinates
		////closest_node = _privateData->renderer.computeScreenCoordinates(view, frac);//CF
		closest_node = renderer->computeScreenCoordinates(view, frac);//CF
		media_cycle->setClosestNode(closest_node);
		// recompute scene graph	
		////_privateData->renderer.updateNodes(frac); // animation time in [0,1] //CF
		////_privateData->renderer.updateLabels(frac); //CF
		renderer->updateNodes(frac); // animation time in [0,1] //CF
		renderer->updateLabels(frac);//CF		
		
		renderer_hud->updatePointers(view);
		/*
		osgViewer::ViewerBase::Views views;
		bool have_views;
		view->getViews(views,have_views);
		renderer_hud->updatePointers(views[0]);*/
	}
}

//CF workaround due to issues with accessing C++ pointer through Objective-C functions...
- (float) getMouseDistanceAtNode:(int)closest_node {return renderer->getDistanceMouse()[closest_node];}

@end
