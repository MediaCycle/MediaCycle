//
//  ACAudioCycleOsgiPadView.mm
//  MediaCycle
//
//  @author Stéphane Dupont
//  @date 05/01/11
//  @copyright (c) 2011 – UMONS - Numediart
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

#import "ACAudioCycleOsgiPadView.h"

using namespace osg;

@implementation ACAudioCycleOsgiPadView

+ (void)initialize {
	
}

- (void)initCommonACAudioCycleOsgiPadView {
	
	osg::setNotifyLevel(osg::INFO);
	
	//get the view size
	CGRect lFrame = [self bounds];
	unsigned int x = lFrame.origin.x;
	unsigned int y = lFrame.origin.y;
	unsigned int w = lFrame.size.width;
	unsigned int h = lFrame.size.height;
			
	//create our graphics context directly so we can pass our own window 
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	
	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowIOS::WindowData((UIView*)self);
	
	// Setup the traits parameters
	traits->x = x+0;
	traits->y = y+0;
	traits->width = w-0;
	traits->height = h-0;
	traits->depth = 16; //keep memory down, default is currently 24
	//traits->alpha = 8;
	//traits->stencil = 8;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	//traits->windowName = "osgViewer";	
	traits->inheritedWindowData = windata;
	
	// Create the Graphics Context
	osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());

	//create the viewer    
	_viewer = new osgViewer::Viewer();

	_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);//SingleThreaded DrawThreadPerContext

	//if the context was created then attach to our viewer
	if(graphicsContext) {
		_viewer->getCamera()->setGraphicsContext(graphicsContext);
		//_viewer->setUpViewerAsEmbeddedInWindow(0, 0, traits->width, traits->height);	
		_viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		_viewer->getCamera()->setProjectionMatrixAsPerspective(1.25f, _viewer->getCamera()->getViewport()->aspectRatio(), 0.001f, 100.0f);
		//_viewer->getCamera()->getViewMatrix().makeIdentity();
		// SD TODO - note the -1.0 here indicating an up-down inversion bug in OSG
		_viewer->getCamera()->setViewMatrixAsLookAt(osg::Vec3(0.0f,0.0f,1.0f), osg::Vec3(0.0f,0.0f,0.0f), osg::Vec3(0.0f,-1.0f,0.0f));
		//_viewer->setCameraManipulator(new osgGA::TrackballManipulator);

	}
	
	_root = new MatrixTransform();
	
	// Create Objects
	
	media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");

	audio_engine = new ACAudioEngine();
	audio_engine->setMediaCycle(media_cycle);
		
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	_viewer->addEventHandler(event_handler);
	//_viewer->addEventHandler(new osgViewer::StatsHandler);
	
	renderer = new ACOsgBrowserRenderer();
	renderer->setMediaCycle(media_cycle);
	
	//[self prepareFromBrowser];
	media_cycle->setNeedsDisplay(true);
	
	// Performs some initialization
	
	media_cycle->setClusterNumber(10);
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	//NSString *filePath = [[NSBundle mainBundle] pathForResource:@"zero-g-pro-pack-small-mc-ipad" ofType:@"acl"];  
	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"zero-g-pro-pack-mc-ipod-processed-11" ofType:@"acl"];  
	media_cycle->setPath((string)[resourcePath UTF8String]);
	media_cycle->importACLLibrary((string)[filePath UTF8String]);
	media_cycle->normalizeFeatures();
	media_cycle->libraryContentChanged();
	//media_cycle->setWeight(0, 0.0);
	//media_cycle->setWeight(1, 1.0);
	[self updatedLibrary];	
	[self updateTransformsFromBrowser:0];
	media_cycle->setNeedsDisplay(true);
	[self updateScene];
	
	[NSTimer scheduledTimerWithTimeInterval:1.0/5.0 target:self selector:@selector(updateScene) userInfo:nil repeats:YES]; 
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
	if (self) {
        // Custom initialization
		
    }
    return self;
}


- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
	
    if (self) {		
        // Initialization code.
		
		[self initCommonACAudioCycleOsgiPadView];
		
    }
    return self;
}

- (id)initWithCoder:coder {
	
	[super initWithCoder:coder];
	
	[self initCommonACAudioCycleOsgiPadView];
	
	return self;
}

- (void)loadView
{
	
}

- (void)viewDidLoad
{
	
}

- (void)awakeFromNib
{
	
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect { 
	[self updateScene];
}

- (void)dealloc {
	if (event_handler)
		delete event_handler;
	if (renderer)
		delete renderer;
	if (audio_engine)
		delete audio_engine;
	if (media_cycle)
	delete media_cycle;
	_root = NULL;
	_viewer = NULL;
	[super dealloc];
}
	
// SD TODO - drawRect is not calle naturally so a timer calling this is necessary - find out why
//
//Timer called function to update our scene and render the viewer
//
- (void)updateScene {
	if (_viewer) {
		double frac = 0.0;
		if(media_cycle && media_cycle->hasBrowser()) {
			media_cycle->updateState();
			frac = media_cycle->getFrac();
		}		
		if (!media_cycle->getNeedsDisplay()) {
			return;
		}		
		if(_viewer->getCamera() && media_cycle && media_cycle->hasBrowser()) {
			float x=0.0f, y=0.0f, zoom, angle;
			float upx, upy;
			zoom = media_cycle->getCameraZoom();
			angle = media_cycle->getCameraRotation();
			media_cycle->getCameraPosition(x, y);
			upx = cos(-angle+PI/2);
			upy = sin(-angle+PI/2);		
			_viewer->getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0, 10.0 * 0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(-upx, -upy, 0));
		}
		if (media_cycle && media_cycle->hasBrowser() && media_cycle->getBrowser()->getState() == AC_CHANGING) {
			if (media_cycle->getNeedsDisplay()) {
				[self updateTransformsFromBrowser:media_cycle->getFrac()];
			}
		}
		_viewer->frame();
		media_cycle->setNeedsDisplay(false);
	}
}

- (void)prepareFromBrowser {
	renderer->prepareNodes();
	_root->addChild(renderer->getShapes());
	//NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	//NSString *filePath = [[NSBundle mainBundle] pathForResource:@"hog" ofType:@"osg"];  
	//osg::ref_ptr<osg::Node> model = (osgDB::readNodeFile([filePath UTF8String]));
	//_root->addChild(model);
	_viewer->setSceneData(_root.get());
}

- (void)updateTransformsFromBrowser:(double)frac {
	int closest_node;
	// get screen coordinates
	closest_node = renderer->computeScreenCoordinates(_viewer, frac);//CF
	media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	renderer->updateNodes(frac);
	renderer->updateLabels(frac);
}

- (void)updatedLibrary {	
	media_cycle->setReferenceNode(0);//CF we want to debug the view and positions using the same layout at each relaunch!
	media_cycle->getBrowser()->setState(AC_CHANGING);
	media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	[self prepareFromBrowser];
	media_cycle->setNeedsDisplay(true);
}

@end
