//
//  ACOsgViewCocoa.mm
//  MediaCycle
//
//  @author Christian Frisson
//  @date 14/10/10
//  @copyright (c) 2010 – UMONS - Numediart
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
//  Forked from TiCore by Raphael Sebbe.
//  @copyright (c) 2007 – UMONS - Numediart
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

#if defined(USE_OSG) && defined(__APPLE__)

#import "ACOsgViewCocoa.h"
#import "ACOsgViewNodeCocoa.h"
//#import <CoreVideo/CoreVideo.h>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>

using namespace osg;
using namespace osgViewer;


@interface NSObject (OSGRedraw)
- (void)requestRedraw;
- (void)requestContinuousUpdate:(BOOL)val;
- (CVReturn)displayFrame:(const CVTimeStamp *)timeStamp;
- (void)createDisplayLink;
@end


class MERef : public Referenced
{
public:
	void *pointer;
	
	MERef(void *ptr) : Referenced(), pointer(ptr) {}
	virtual ~MERef() {}
};

class MEViewer : public Viewer
{
	virtual void requestRedraw()
	{
		MERef *ref = (MERef*)getUserData();
		
		if(ref)
		{
			id object = (id)ref->pointer;
			
			if(object && [object respondsToSelector:@selector(requestRedraw)])
			{
				[object requestRedraw];
			}
		}
	}
	
	virtual void requestContinuousUpdate(bool needed=true)
	{
		//printf("requestContinuousUpdate: %d\n", needed?1:0);
		MERef *ref = (MERef*)getUserData();
		
		if(ref)
		{
			id object = (id)ref->pointer;
			
			if(object && [object respondsToSelector:@selector(requestContinuousUpdate:)])
			{
				[object requestContinuousUpdate:needed];
			}
		}
	}
	
};

struct ACOsgViewCocoaData
{
	ref_ptr<MEViewer> 		viewer;
	observer_ptr<GraphicsWindow> 	window;
	
	ref_ptr<Group>			parent;
	ref_ptr<Node>			externalNode;
};




extern "C" CVReturn MEDisplayLinkCallback (
				CVDisplayLinkRef displayLink,
				const CVTimeStamp *inNow,
				const CVTimeStamp *inOutputTime,
				CVOptionFlags flagsIn,
				CVOptionFlags *flagsOut,
				void *displayLinkContext)
{
	//NSLog(@"displayLink fire");
	
	CVReturn error =
        [(id) displayLinkContext displayFrame:inOutputTime];
	return error;
}

static NSDate *gReferenceDate = nil;

@implementation ACOsgViewCocoa

@synthesize renderDelegate = _renderDelegate;

+ (void)initialize
{
	static bool done = 0;
	
	if(!done && self == [ACOsgViewCocoa class])
	{
		gReferenceDate = [[NSDate date] retain];
	}
	
}

- (void)initCommonACOsgViewCocoa
{
	_osgViewData = new ACOsgViewCocoaData();
	assert(_osgViewData);
	
	Group *parent = new Group();
	
	//group->addChild(gSkeletonRenderer.getRootNode().get());
	//parent->addChild(TiOsgGrid(100., 100., 20, 20).get());
	
	_osgViewData->parent = parent;
	
	_osgViewData->viewer = new MEViewer();
	
	_osgViewData->viewer->setUserData(new MERef((void*)self));
	_osgViewData->window = NULL;
}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
	if(self = [super initWithFrame:frameRect pixelFormat:format])
	{
		[self initCommonACOsgViewCocoa];
	}
	
	return self;
}

- (id)initWithCoder:coder
{
	[super initWithCoder:coder];
	
	[self initCommonACOsgViewCocoa];	
	
	return self;
}

- (void)dealloc
{
	if(CVDisplayLinkIsRunning(_displayLink))
	{
		CVDisplayLinkStop(_displayLink);
	}
	CVDisplayLinkRelease(_displayLink);
	
	delete _osgViewData;
	
	
	
	[super dealloc];
}

- (void)requestRedraw
{
	//NSLog(@"request redraw");
	
	//[self drawRect:[self bounds]];
	//[self setNeedsDisplay:YES];
	if(!_drawing) [self drawRect:[self bounds]];
}

- (void)updateDisplayLink
{
	if((_continuousUpdate || _playing) && !CVDisplayLinkIsRunning(_displayLink))
	{
		NSLog(@"Restarting DisplayLink");
		CVDisplayLinkStart(_displayLink);
	}
	
	if((!_continuousUpdate && !_playing) && CVDisplayLinkIsRunning(_displayLink))
	{
		NSLog(@"Stopping DisplayLink");
		CVDisplayLinkStop(_displayLink);
	}
}

- (void)requestContinuousUpdate:(BOOL)val 
{
	_continuousUpdate = val;
	
	[self updateDisplayLink];
}

- (void)setPlaying:(BOOL)val
{
	if(_displayLink == NULL) [self createDisplayLink];
	
	
	bool old_playing = _playing;
	
	if(_playing && !val)
	{
		if([_renderDelegate respondsToSelector:@selector(osgViewDidStopPlaying:)])
		{
			[_renderDelegate osgViewDidStopPlaying:self];
		}
	}
	
	
	_playing = val;
	
	[self updateDisplayLink];
	
	// this is done after updating the display link so that we get a time !!!
	if(!old_playing && val) 
	{
		if([_renderDelegate respondsToSelector:@selector(osgViewWillStartPlaying:)])
		{
			[_renderDelegate osgViewWillStartPlaying:self];
		}
	}
}

- (BOOL)playing
{
	return _playing;
}

- (NSPoint)osgConvertPoint:(NSPoint)p
{
	NSPoint res = [self convertPoint:p fromView:nil];
	NSSize s = [self bounds].size;
	
	
	res.y = s.height - res.y;
	
	return res;
}

- (void)awakeFromNib
{
	assert(_osgViewData);
}

- (void)prepareOpenGL
{
	//NSLog(@"prepareOpenGL : context = 0x%x",[self openGLContext]);
	
	if(_openGLPrepared) return;
	[super prepareOpenGL];
	_openGLPrepared = YES;
	
	//NSLog(@"prepareOpenGL");
	NSSize bounds = [self bounds].size;
	
	//[[self window] setAcceptsMouseMovedEvents:YES];
	
	/*[[self openGLContext] makeCurrentContext];
	osg::DisplaySettings * ds = osg::DisplaySettings::instance();
	ds->setNumMultiSamples(4);
	
	ref_ptr< DisplaySettings > displaySettings = new DisplaySettings;
	displaySettings->setNumMultiSamples(4);
	_osgViewData->viewer->setDisplaySettings( displaySettings.get() );
	*/
	_osgViewData->window = _osgViewData->viewer->setUpViewerAsEmbeddedInWindow(100,100,bounds.width, bounds.height);

	//_osgViewData->window->setDisplaySettings( displaySettings.get() );
	
	//_osgViewData->viewer->setThreadingModel(ViewerBase::DrawThreadPerContext);
	
	_osgViewData->viewer->addEventHandler(new osgViewer::StatsHandler);
	//	
	
	assert(_osgViewData->parent.valid());
	
	_osgViewData->viewer->setSceneData(_osgViewData->parent.get());
	//_osgViewData->viewer->setCameraManipulator(new osgGA::TrackballManipulator);
	_osgViewData->viewer->addEventHandler(new osgViewer::StatsHandler);
	
	_osgViewData->viewer->realize();
	//_osgViewData->viewer->getEventQueue()->getCurrentEventState()->setMouseYOrientation(osgGA::GUIEventAdapter::Y_INCREASING_UPWARDS);
	
	[self createDisplayLink];
	//CVDisplayLinkStart(_displayLink);
	
}

- (void)reshape
{
	NSSize bounds = [self bounds].size;
	int w = bounds.width, h = bounds.height;
	
	if (_osgViewData->window.valid()) 
	{
		_osgViewData->window->resized(_osgViewData->window->getTraits()->x, _osgViewData->window->getTraits()->y, w, h);
		_osgViewData->window->getEventQueue()->windowResize(_osgViewData->window->getTraits()->x, _osgViewData->window->getTraits()->y, w, h );
	}
}

- (void)keyDown:(NSEvent*)event 
{
	NSString* event_characters = [event characters];
	unichar unicode_character = [event_characters characterAtIndex:0];
	_osgViewData->window->getEventQueue()->keyPress(static_cast<osgGA::GUIEventAdapter::KeySymbol>(unicode_character));
}

- (void)keyUp:(NSEvent*)event 
{
	NSString* event_characters = [event characters];
	unichar unicode_character = [event_characters characterAtIndex:0];
	_osgViewData->window->getEventQueue()->keyRelease(static_cast<osgGA::GUIEventAdapter::KeySymbol>(unicode_character));
}

- (void)osgMouseDown:(NSEvent*)event buttonIndex:(int)bindex
{
	//NSLog(@"MouseDown");
	/*int bindex = 1;
	
	
	
	if([event buttonNumber] == NSLeftMouseDown) bindex = 1;
	else if([event buttonNumber] == NSRightMouseDown) bindex = 3;
	else bindex = 2;
	*/
	NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [self osgConvertPoint:event_location];
	int x = local_point.x, y = local_point.y;
	
	
	
	assert(_osgViewData->window.valid());
	_osgViewData->window->getEventQueue()->mouseButtonPress( x, y, bindex );
	_osgViewData->viewer->eventTraversal();
	
	
	//[self setNeedsDisplay:YES];
}

- (void)mouseDown:(NSEvent*)event 
{
	[self osgMouseDown:event buttonIndex:1];
}

- (void)rightMouseDown:(NSEvent*)event 
{
	[self osgMouseDown:event buttonIndex:3];
}


- (void)otherMouseDown:(NSEvent*)event 
{
	[self osgMouseDown:event buttonIndex:2];
}


- (void)osgMouseUp:(NSEvent*)event buttonIndex:(int)bindex
{
		//NSLog(@"MouseUp");
/*	int bindex = 1;
	
	if([event buttonNumber] == NSLeftMouseUp) bindex = 1;
	else if([event buttonNumber] == NSRightMouseUp) bindex = 3;
	else bindex = 2;
*/	
	NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [self osgConvertPoint:event_location];
	int x = local_point.x, y = local_point.y;
	
	assert(_osgViewData->window.valid());
	_osgViewData->window->getEventQueue()->mouseButtonRelease( x, y, bindex );	
	_osgViewData->viewer->eventTraversal();
	
	//[self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent*)event
{
	[self osgMouseUp:event buttonIndex:1];
}
- (void)rightMouseUp:(NSEvent*)event
{
	[self osgMouseUp:event buttonIndex:3];	
}
- (void)otherMouseUp:(NSEvent*)event
{
	[self osgMouseUp:event buttonIndex:2];
}

- (void)mouseMoved:(NSEvent*)event
{
	NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [self osgConvertPoint:event_location];
	int x = local_point.x, y = local_point.y;
	
	if(NSPointInRect(local_point, [self bounds]))
	{
		assert(_osgViewData->window.valid());
		_osgViewData->window->getEventQueue()->mouseMotion( x, y );
		_osgViewData->viewer->eventTraversal();
	}
}

- (void)osgMouseDragged:(NSEvent*)event buttonIndex:(int)bindex
{
	//NSLog(@"MouseDragged");
	
	NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [self osgConvertPoint:event_location];
	int x = local_point.x, y = local_point.y;
	
	assert(_osgViewData->window.valid());
	_osgViewData->window->getEventQueue()->mouseMotion( x, y );
	_osgViewData->viewer->eventTraversal();
	//[self setNeedsDisplay:YES];
	
}

- (void)mouseDragged:(NSEvent*)event
{
	[self osgMouseDragged:event buttonIndex:1];
}

- (void)rightMouseDragged:(NSEvent*)event
{
	[self osgMouseDragged:event buttonIndex:3];
}

- (void)otherMouseDragged:(NSEvent*)event
{
	[self osgMouseDragged:event buttonIndex:2];
}

/*- (void)mouseMoved:(NSEvent*)event
{
	NSPoint event_location = [event locationInWindow];
	NSPoint local_point = [self osgConvertPoint:event_location];
	int x = local_point.x, y = local_point.y;
	
	assert(_osgViewData->window.valid());
	_osgViewData->window->getEventQueue()->mouseMotion( x, y );
	[self setNeedsDisplay:YES];
}
*/
- (void)drawRect:(NSRect)rect
{
	//if([[NSThread currentThread] isMainThread]) return;
	
	if(!_openGLPrepared) return;
	
	
	// there is a problem in OSG when rendering from multiple threads at the same time
	@synchronized([self class])
	//@synchronized(self)
	{
		//NSLog(@"ACOsgViewCocoaData's drawRect (0x%x)", self);
		_drawing = YES;
		//if(![[self window] isVisible]) return;
		[[self openGLContext] makeCurrentContext];
		
		if(_playing && [_renderDelegate respondsToSelector:@selector(osgViewWillRenderFrame:)])
		{
			[_renderDelegate osgViewWillRenderFrame:self];
		}
		
		if (_osgViewData->window.valid()) _osgViewData->viewer->frame();
		glFlush();
		
		if(_playing && [_renderDelegate respondsToSelector:@selector(osgViewDidRenderFrame:)])
		{
			[_renderDelegate osgViewDidRenderFrame:self];
		}
		//[[self openGLContext] flushBuffer];
		//glSwapAPPLE();
		
		//if(!CVDisplayLinkIsRunning(_displayLink)) CVDisplayLinkStart(_displayLink);
		_drawing = NO;
	}
}

- (void)createDisplayLink
{
	if(_displayLink != NULL) return;
	CVReturn            error = kCVReturnSuccess;
	CGDirectDisplayID   displayID = CGMainDisplayID();// 1
	
	error = CVDisplayLinkCreateWithCGDisplay(displayID, &_displayLink);// 2
	if(error)
	{
		NSLog(@"DisplayLink created with error:%d", error);
		_displayLink = NULL;
		return;
	}
	else 
	{
		NSLog(@"DisplayLink created with no error");
	}
	error = CVDisplayLinkSetOutputCallback(_displayLink,// 3
					       MEDisplayLinkCallback, self);
	
}

- (double)currentTime
{
	return [[NSDate date] timeIntervalSinceDate:gReferenceDate];
	
	/*CVTimeStamp timeStamp;
	
	@synchronized(self)
	{
		if(CVDisplayLinkGetCurrentTime(_displayLink, &timeStamp) == kCVReturnSuccess)
		{
			
			if(timeStamp.videoTimeScale > 0)
				return (double)timeStamp.videoTime / (double)timeStamp.videoTimeScale;
		}
	}
	return 0.0;*/
}

- (CVReturn)displayFrame:(const CVTimeStamp *)timeStamp
{
	CVReturn rv = kCVReturnError;
	NSAutoreleasePool *pool;
	
	pool = [[NSAutoreleasePool alloc] init];
	
	@synchronized (self)
	{
		[self drawRect:NSZeroRect];
		rv = kCVReturnSuccess;
	}
	[pool release];
	return rv;
}


@end

@implementation ACOsgViewCocoa (Node)

- (void)setNode:(Node*)node
{
	assert(_osgViewData->parent.valid());
	
	if(_osgViewData->externalNode.valid())
	{
		_osgViewData->parent->removeChild(_osgViewData->externalNode.get());
	}
	
	_osgViewData->externalNode = node;
	_osgViewData->parent->addChild(_osgViewData->externalNode.get());
	
	[self drawRect:[self bounds]];
	//[self setNeedsDisplay:YES];
}

- (void)addEventHandler:(osgGA::GUIEventHandler*)eventHandler
{
	_osgViewData->viewer->addEventHandler(eventHandler);
}

- (void)setCameraManipulator:(osgGA::MatrixManipulator*)manipulator
{
	_osgViewData->viewer->setCameraManipulator(manipulator);
}

- (osg::Camera*)camera
{
	return _osgViewData->viewer->getCamera();
}

- (osgViewer::Viewer*)viewer
{
	return _osgViewData->viewer;
}

@end

#endif