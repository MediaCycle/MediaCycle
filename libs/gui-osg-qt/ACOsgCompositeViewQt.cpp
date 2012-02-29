//
//  ACOsgCompositeViewQt.cpp
//  MediaCycle
//
//  @author Christian Frisson
//  @date 29/04/10
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
//  Qt QGLWidget and OSG CompositeViewer that wraps
//  a MediaCycle browser and multitrack timeline viewer
//

#define PI 3.1415926535897932384626433832795f

#include "ACOsgCompositeViewQt.h"
#include <cmath>
#include <QDesktopWidget>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <sstream>

namespace fs = boost::filesystem;

using namespace osg;

// ----------- uncomment to use ortho2D projection in the timeline renderer (not implemented)
//#define TIMELINE_RENDERER_ORTHO2D

ACOsgCompositeViewQt::ACOsgCompositeViewQt( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f)
	: QGLWidget(parent, shareWidget, f), media_cycle(0),
	browser_renderer(0), browser_event_handler(0), timeline_renderer(0), timeline_event_handler(0), timeline_controls_renderer(0), hud_renderer(0), hud_view(0),
	#if defined (SUPPORT_AUDIO)
		audio_engine(0),
	#endif //defined (SUPPORT_AUDIO)
    mousedown(0), borderdown(0),
	refx(0.0f), refy(0.0f),
	refcamx(0.0f), refcamy(0.0f),
	refzoom(0.0f),refrotation(0.0f),
	septhick(5),sepy(0.0f),refsepy(0.0f),controls_width(0),screen_width(0),
	library_loaded(false),mouseover(false),
    mediaOnTrack(-1),track_playing(false),
    openMediaExternallyAction(0), browseMediaExternallyAction(0), examineMediaExternallyAction(0), forwardNextLevelAction(0),
    stopPlaybackAction(0), toggleMediaHoverAction(0), triggerMediaHoverAction(0),
    resetBrowserAction(0), rotateBrowserAction(0), zoomBrowserAction(0),
    translateBrowserAction(0), addMediaOnTimelineTrackAction(0), toggleTimelinePlaybackAction(0), adjustTimelineHeightAction(0)
{
	osg_view = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
	setFocusPolicy(Qt::StrongFocus);// CF instead of ClickFocus

	setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	/*
	 CF: other threading models to test are:
		 SingleThreaded
		 CullDrawThreadPerContext
		 ThreadPerContext
		 DrawThreadPerContext
		 CullThreadPerCameraDrawThreadPerContext
		 ThreadPerCamera
		 AutomaticSelection
	 */

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.start(50);

	// Renderers
	browser_renderer = new ACOsgBrowserRenderer();
	timeline_renderer = new ACOsgTimelineRenderer();
	timeline_controls_renderer = new ACOsgTimelineControlsRenderer();
	hud_view = new osgViewer::View;
	hud_renderer = new ACOsgHUDRenderer();

	//this->setAttribute(Qt::WA_Hover, true);
	setMouseTracking(true); //CF necessary for the hover callback

	// Views
	sepy = 0;//height()/4;// CF browser/timeline proportions at startup

	browser_view = new osgViewer::View;
	browser_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	this->updateBrowserView(width(),height());
	this->addView(browser_view);

	timeline_view = new osgViewer::View;
	timeline_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	this->updateTimelineView(width(),height());
	this->addView(timeline_view);

	timeline_controls_view = new osgViewer::View;
	timeline_controls_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	this->updateTimelineControlsView(width(),height());
	this->addView(timeline_controls_view);

	// Event handlers
	browser_event_handler = new ACOsgBrowserEventHandler;
	browser_view->addEventHandler(browser_event_handler);

	timeline_event_handler = new ACOsgTimelineEventHandler;
	timeline_view->addEventHandler(timeline_event_handler); // CF ((osgViewer::Viewer*) (this))->addEventHandler for the simple Viewer
	timeline_event_handler->setRenderer(timeline_renderer);

	// HUD renderer
 	hud_renderer->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	hud_view->setCamera(hud_renderer->getCamera());
	this->updateHUDCamera(width(),height());
    this->addView(hud_view);

	// Audio waveforms
	screen_width = QApplication::desktop()->screenGeometry().width();
	timeline_renderer->setScreenWidth(screen_width);
	#if defined (SUPPORT_AUDIO)
		audio_engine = 0;
	#endif //defined (SUPPORT_AUDIO)

	timeline_renderer->updateSize(width(),sepy);

	osg::setNotifyLevel(osg::WARN);//remove the NaN CullVisitor messages
    this->initInputActions();
}

void ACOsgCompositeViewQt::updateBrowserView(int _width, int _height){
	if (browser_view){
		browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,_width,_height-sepy)); // CF: for OSG y=0 is on the bottom, for Qt on the top
		browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(_height-sepy), 0.001f, 10.0f);
		browser_view->getCamera()->getViewMatrix().makeIdentity();
		browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
		//browser_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	}
}

void ACOsgCompositeViewQt::updateHUDCamera(int _width, int _height){
	if(hud_view){
		hud_view->getCamera()->setViewport(new osg::Viewport(0,sepy,_width,_height)); // CF: for OSG y=0 is on the bottom, for Qt on the top
		hud_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,_width, 0, _height));
	}
}

void ACOsgCompositeViewQt::updateTimelineView(int _width, int _height){
	if (timeline_view){
		//timeline_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
		timeline_view->getCamera()->setClearColor(Vec4f(0.14,0.14,0.28,1.0));
		timeline_view->getCamera()->setViewport(new osg::Viewport(controls_width,0,_width-controls_width,sepy));
		#ifdef TIMELINE_RENDERER_ORTHO2D
		//orth2D
		timeline_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,_width_,0,sepy));
		timeline_view->getCamera()->setViewMatrix(osg::Matrix::identity());
		#else
		//perspect
		timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
		timeline_view->getCamera()->getViewMatrix().makeIdentity();
		timeline_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
		#endif
	}
}

void ACOsgCompositeViewQt::updateTimelineControlsView(int _width, int _height){
	if(timeline_controls_view){
		//timeline_controls_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
		timeline_controls_view->getCamera()->setClearColor(Vec4f(1.0f,0.14f,0.28f,0.2f));
		timeline_controls_view->getCamera()->setViewport(new osg::Viewport(0,0,controls_width,sepy));
		timeline_controls_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
		timeline_controls_view->getCamera()->getViewMatrix().makeIdentity();
		timeline_controls_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	}
}

ACOsgCompositeViewQt::~ACOsgCompositeViewQt(){
	//browser_view->removeEventHandler(browser_event_handler); // reqs OSG >= 2.9.x and shouldn't be necessary
	//timeline_view->removeEventHandler(timeline_event_handler); // reqs OSG >= 2.9.x and shouldn't be necessary
	this->clean(false);
	delete browser_renderer; browser_renderer = 0;
	//if (browser_event_handler) delete browser_event_handler;
	browser_event_handler = 0;
	delete timeline_renderer; timeline_renderer = 0;
	//if (timeline_event_handler) delete timeline_event_handler;
	timeline_event_handler = 0;
	delete timeline_controls_renderer; timeline_controls_renderer = 0;
	delete hud_renderer; hud_renderer = 0;
	media_cycle = 0;
    if(openMediaExternallyAction) delete openMediaExternallyAction; openMediaExternallyAction = 0;
    if(browseMediaExternallyAction) delete browseMediaExternallyAction; browseMediaExternallyAction = 0;
    if(examineMediaExternallyAction) delete examineMediaExternallyAction; examineMediaExternallyAction = 0;
    if(forwardNextLevelAction) delete forwardNextLevelAction; forwardNextLevelAction = 0;
    if(stopPlaybackAction) delete stopPlaybackAction; stopPlaybackAction = 0;
    if(toggleMediaHoverAction) delete toggleMediaHoverAction; toggleMediaHoverAction = 0;
    if(resetBrowserAction) delete resetBrowserAction; resetBrowserAction = 0;
    if(rotateBrowserAction) delete rotateBrowserAction; rotateBrowserAction = 0;
    if(zoomBrowserAction) delete zoomBrowserAction; zoomBrowserAction = 0;
    if(translateBrowserAction) delete translateBrowserAction; translateBrowserAction = 0;
    if(addMediaOnTimelineTrackAction) delete addMediaOnTimelineTrackAction; addMediaOnTimelineTrackAction = 0;
    if(toggleTimelinePlaybackAction) delete toggleTimelinePlaybackAction; toggleTimelinePlaybackAction = 0;
    if(adjustTimelineHeightAction) delete adjustTimelineHeightAction; adjustTimelineHeightAction = 0;
}

void ACOsgCompositeViewQt::clean(bool updategl){
	browser_event_handler->clean();
	timeline_event_handler->clean();
    mousedown = borderdown = 0;
	refx =  refy = refcamx = refcamy = refzoom = refrotation = 0.0f;
	browser_renderer->clean();
	timeline_renderer->clean();
	hud_renderer->cleanPointers();//TR NEM2011
	//if (browser_event_handler) delete browser_event_handler;
	//if (timeline_event_handler) delete timeline_event_handler;
	sepy = 0;
	if (updategl)
		this->updateGL();
	library_loaded = false;
}

void ACOsgCompositeViewQt::setMediaCycle(MediaCycle* _media_cycle)
{
	media_cycle = _media_cycle;
	browser_renderer->setMediaCycle(media_cycle);
	timeline_renderer->setMediaCycle(media_cycle);
	timeline_controls_renderer->setMediaCycle(media_cycle);
	browser_event_handler->setMediaCycle(media_cycle);
	timeline_event_handler->setMediaCycle(media_cycle);
	hud_renderer->setMediaCycle(media_cycle);
}

#if defined (SUPPORT_AUDIO)
void ACOsgCompositeViewQt::setAudioEngine(ACAudioEngine *engine)
{
	audio_engine=engine;
	if(timeline_renderer)timeline_renderer->setAudioEngine(engine);
	if(timeline_event_handler)timeline_event_handler->setAudioEngine(engine);
}
#endif //defined (SUPPORT_AUDIO)

void ACOsgCompositeViewQt::initializeGL()
{
	/*if (getGraphicsWindow()->isRealized()) {
		unsigned int _screen_width, _screen_height;
		if ( screen_width != _screen_width){
			_screen_width = timeline_view->getCamera()->getGraphicsContext()->getTraits()->width;
			std::cout << "Initial width: " << _screen_width << std::endl;
			//this->screen_width = _screen_width;
			//timeline_renderer->updateScreenWidth(_screen_width);
		}
	}*/
}

void ACOsgCompositeViewQt::resizeGL( int w, int h )
{
	if (isRealized()){
		//std::cout << "height() " << browser_view->getCamera()->getViewport()->height()+timeline_view->getCamera()->getViewport()->height() << " height " << height << std::endl;
		float prevheight = browser_view->getCamera()->getViewport()->height()+timeline_view->getCamera()->getViewport()->height();
		sepy *= h/prevheight;
		timeline_renderer->updateSize(width(),sepy);
	}

	osg_view->getEventQueue()->windowResize(0, 0, w, h);
	osg_view->resized(0,0,w,h);

	if (isRealized()){
		this->updateBrowserView(w,h);
		this->updateTimelineView(w,h);
		this->updateTimelineControlsView(w,h);
		this->updateHUDCamera(w,h);
//hud_renderer->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,w,0,h));
		/*
		 if (getGraphicsWindow()->isRealized()) {

			 unsigned int _screen_width, _screen_height;
			 if ( screen_width != _screen_width){
				 _screen_width = timeline_view->getCamera()->getGraphicsContext()->getTraits()->width;
				 std::cout << "Updating width: " << _screen_width << std::endl;
				 this->screen_width = _screen_width;
				 timeline_renderer->updateScreenWidth(_screen_width);
			 }
		 }
		 */
	}
}

// CF to do: understand paintGL vs updateGL to use them more correctly
void ACOsgCompositeViewQt::paintGL()
{
	frame(); // put this first otherwise we don't get a clean background in the browser
	if (media_cycle == 0) return;

	//CF to improve, we want to know if the view is being animated to force a frequent refresh of the positions:
	//SD 2010feb22 to allow auto update whith threaded import
	//if (media_cycle && media_cycle->hasBrowser() && media_cycle->getBrowser()->getState() == AC_CHANGING)
		updateTransformsFromBrowser(media_cycle->getFrac());
}

// called according to timer
void ACOsgCompositeViewQt::updateGL()
{
	double frac = 0.0;
	if (media_cycle == 0) return;

	if(media_cycle && media_cycle->hasBrowser())
	{
		if(mouseover!=this->underMouse()){
			if(this->underMouse())//Qt
				media_cycle->getBrowser()->addMousePointer();
			else {
				media_cycle->getBrowser()->removeMousePointer();
			}
			//std::cout << "Mouse " << this->underMouse() << std::endl;
			mouseover=this->underMouse();
		}
		media_cycle->updateState();
		frac = media_cycle->getFrac();

        //CF this is a temporary solution until we implement signals/slots in the core
        if(media_cycle->getBrowser()->getModeChanged()){
            //std::cout << "ACOsgCompositeViewQt::updateGL intercepted browsing mode change" << std::endl;
            browser_renderer->clean();
        }
	}

	if (!media_cycle->getNeedsDisplay()) {
		return;
	}

	if(browser_view->getCamera() && media_cycle && media_cycle->hasBrowser())
	{

		float x=0.0, y=0.0, zoom, angle;
		float upx, upy;

		zoom = media_cycle->getCameraZoom();
		angle = media_cycle->getCameraRotation();
		media_cycle->getCameraPosition(x, y);
		upx = cos(-angle+PI/2);
		upy = sin(-angle+PI/2);

		this->updateBrowserView(width(),height());
		browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));
		this->updateTimelineView(width(),height());
		this->updateTimelineControlsView(width(),height());
		this->updateHUDCamera(width(),height());
	}

	this->updateTransformsFromBrowser(frac);
	this->updateTransformsFromTimeline(frac);
	/*
	if (frac != 0.0)
		setMouseTracking(true); //CF necessary for the hover callback
	*/
	QGLWidget::updateGL();
    media_cycle->setNeedsDisplay(false);
}

void ACOsgCompositeViewQt::addInputAction(ACInputActionQt* _action)
{
    //this->addAction(dynamic_cast<QAction*>(_action));
    this->addAction(_action);
    this->inputActions.append(_action);
}

void ACOsgCompositeViewQt::initInputActions(){

    // CF add extra shortcuts from QKeySequence::StandardKey

    openMediaExternallyAction = new ACInputActionQt(tr("Open Media File"), this);
    openMediaExternallyAction->setShortcut(Qt::Key_O);
    openMediaExternallyAction->setKeyEventType(QEvent::KeyPress);
    openMediaExternallyAction->setMouseEventType(QEvent::MouseButtonRelease);
    openMediaExternallyAction->setToolTip(tr("Open the media file with the default application"));
    connect(openMediaExternallyAction, SIGNAL(triggered()), this, SLOT(openMediaExternally()));
    this->addInputAction(openMediaExternallyAction);

    browseMediaExternallyAction = new ACInputActionQt(tr("Browse Media File"), this);
    browseMediaExternallyAction->setShortcut(Qt::Key_F);
    browseMediaExternallyAction->setKeyEventType(QEvent::KeyPress);
    browseMediaExternallyAction->setMouseEventType(QEvent::MouseButtonRelease);
    browseMediaExternallyAction->setToolTip(tr("Browse the media file with the default file browser"));
    connect(browseMediaExternallyAction, SIGNAL(triggered()), this, SLOT(browseMediaExternally()));
    this->addInputAction(browseMediaExternallyAction);

    /*examineMediaExternallyAction = new ACInputActionQt(tr("Examine Media File"), this);
    examineMediaExternallyAction->setShortcut(Qt::Key_I);
    examineMediaExternallyAction->setKeyEventType(QEvent::KeyPress);
    examineMediaExternallyAction->setMouseEventType(QEvent::MouseButtonRelease);
    examineMediaExternallyAction->setToolTip(tr("Examine the media file with the default file browser properties"));
    connect(examineMediaExternallyAction, SIGNAL(triggered()), this, SLOT(examineMediaExternally()));
    this->addInputAction(examineMediaExternallyAction);*/

    //CF separate this clusters/neighbors action into two?
    forwardNextLevelAction = new ACInputActionQt(tr("Recluster/Unwrap"), this);
    forwardNextLevelAction->setToolTip(tr("Recluster the cluster or unwrap neighbors around the selected node"));
    forwardNextLevelAction->setShortcut(Qt::Key_A);
    forwardNextLevelAction->setKeyEventType(QEvent::KeyPress);
    forwardNextLevelAction->setMouseEventType(QEvent::MouseButtonPress);
    connect(forwardNextLevelAction, SIGNAL(triggered(bool)), this, SLOT(forwardNextLevel(bool)));
    this->addInputAction(forwardNextLevelAction);

    stopPlaybackAction = new ACInputActionQt(tr("Stop Playback"), this);
    stopPlaybackAction->setShortcut(Qt::Key_M);
    stopPlaybackAction->setKeyEventType(QEvent::KeyPress);
    stopPlaybackAction->setToolTip(tr("Stop the playback of all played media nodes"));
    connect(stopPlaybackAction, SIGNAL(triggered()), this, SLOT(stopPlayback()));
    this->addInputAction(stopPlaybackAction);

    toggleMediaHoverAction = new ACInputActionQt(tr("Toggle Media Hover"), this);
    toggleMediaHoverAction->setShortcut(Qt::Key_W);
    toggleMediaHoverAction->setKeyEventType(QEvent::KeyPress);
    toggleMediaHoverAction->setToolTip(tr("Toggle Media Hover (fast browsing with playback or zoom)"));
    connect(toggleMediaHoverAction, SIGNAL(toggled(bool)), this, SLOT(toggleMediaHover(bool)));
    this->addInputAction(toggleMediaHoverAction);

    triggerMediaHoverAction = new ACInputActionQt(tr("Trigger Media Hover"), this);
    triggerMediaHoverAction->setShortcut(Qt::Key_Q);
    triggerMediaHoverAction->setKeyEventType(QEvent::KeyPress);
    //triggerMediaHoverAction->setAutoRepeat(false); // works on OSX but not Ubuntu (10.04)
    triggerMediaHoverAction->setToolTip(tr("Trigger Media Hover (fast browsing with playback or zoom)"));
    connect(triggerMediaHoverAction, SIGNAL(triggered(bool)), this, SLOT(triggerMediaHover(bool)));
    this->addInputAction(triggerMediaHoverAction);

    resetBrowserAction = new ACInputActionQt(tr("Reset Browser"), this);
    resetBrowserAction->setShortcut(Qt::Key_C);
    resetBrowserAction->setKeyEventType(QEvent::KeyRelease);
    resetBrowserAction->setToolTip(tr("Reset the browser view (center, rotation, zoom)"));
    connect(resetBrowserAction, SIGNAL(triggered()), this, SLOT(resetBrowser()));
    this->addInputAction(resetBrowserAction);

    rotateBrowserAction = new ACInputActionQt(tr("Rotate Browser"), this);
    rotateBrowserAction->setShortcut(Qt::Key_R);
    rotateBrowserAction->setKeyEventType(QEvent::KeyPress);
    rotateBrowserAction->setMouseEventType(QEvent::MouseMove);
    rotateBrowserAction->setToolTip(tr("Rotate the browser view"));
    connect(rotateBrowserAction, SIGNAL(mouseMovedXY(float,float)), this, SLOT(rotateBrowser(float,float)));
    this->addInputAction(rotateBrowserAction);

    zoomBrowserAction = new ACInputActionQt(tr("Zoom Browser"), this);
    zoomBrowserAction->setShortcut(Qt::Key_Z);
    zoomBrowserAction->setKeyEventType(QEvent::KeyPress);
    zoomBrowserAction->setMouseEventType(QEvent::MouseMove);
    zoomBrowserAction->setToolTip(tr("Zoom the browser view"));
    connect(zoomBrowserAction, SIGNAL(mouseMovedXY(float,float)), this, SLOT(zoomBrowser(float,float)));
    this->addInputAction(zoomBrowserAction);

    translateBrowserAction = new ACInputActionQt(tr("Translate Browser"), this);
    translateBrowserAction->setMouseEventType(ACEventQt::MousePressedMove);
    translateBrowserAction->setToolTip(tr("Translate the browser view"));
    connect(translateBrowserAction, SIGNAL(mouseMovedXY(float,float)), this, SLOT(translateBrowser(float,float)));
    this->addInputAction(translateBrowserAction);

    addMediaOnTimelineTrackAction = new ACInputActionQt(tr("Timeline Media"), this);
    addMediaOnTimelineTrackAction->setShortcut(Qt::Key_T);
    addMediaOnTimelineTrackAction->setKeyEventType(QEvent::KeyPress);
    addMediaOnTimelineTrackAction->setMouseEventType(QEvent::MouseButtonRelease);
    addMediaOnTimelineTrackAction->setToolTip(tr("Visualize media on a timeline track"));
    connect(addMediaOnTimelineTrackAction, SIGNAL(triggered()), this, SLOT(addMediaOnTimelineTrack()));
    this->addInputAction(addMediaOnTimelineTrackAction);

    toggleTimelinePlaybackAction = new ACInputActionQt(tr("Toggle Timeline Playback"), this);
    toggleTimelinePlaybackAction->setShortcut(Qt::Key_Space);
    toggleTimelinePlaybackAction->setKeyEventType(QEvent::KeyPress);
    toggleTimelinePlaybackAction->setToolTip(tr("Toggle timeline playback"));
    connect(toggleTimelinePlaybackAction, SIGNAL(toggled(bool)), this, SLOT(toggleTimelinePlayback(bool)));
    this->addInputAction(toggleTimelinePlaybackAction);

    /*adjustTimelineHeightAction = new ACInputActionQt(tr("Adjust Timeline Height"), this);
    //adjustTimelineHeightAction->setShortcut(Qt::Key_Z);
    //adjustTimelineHeightAction->setKeyEventType(QEvent::KeyPress);
    adjustTimelineHeightAction->setMouseEventType(QEvent::MouseMove);
    adjustTimelineHeightAction->setToolTip(tr("Adjust the timeline height"));
    connect(adjustTimelineHeightAction, SIGNAL(mouseMovedY(float)), this, SLOT(adjustTimelineHeight(float)));
    this->addInputAction(adjustTimelineHeightAction);*/
}

void ACOsgCompositeViewQt::openMediaExternally(){
    //std::cout << "Open externally " << std::endl;
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        int loop = media_cycle->getClickedNode();
        //int loop = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        //std::cout << "node " << loop << " selected" << std::endl;

        if(loop >= 0)
        {
            #if defined (__APPLE__)
            std::stringstream command;
            ACMediaType _media_type = media_cycle->getLibrary()->getMedia(loop)->getMediaType();
            if (_media_type == MEDIA_TYPE_IMAGE || _media_type == MEDIA_TYPE_VIDEO)
                command << "open -a Preview '";
            else if (_media_type == MEDIA_TYPE_TEXT)
                command << "open '"; // uses TextEdit or other default text application if customized OS-wide
            else
                command << "open -R '"; // no iTunes for audio! 3Dmodel default applications?
            command << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ;
            try {
                system(command.str().c_str());
            }
            catch (const exception& e) {
                cout << "ACOsgCompositeViewQt: caught exception while trying to open media file " << media_cycle->getLibrary()->getMedia(loop)->getFileName() << " with the OSX-wide prefered application: " << e.what() << endl;
            }
            #endif //defined (__APPLE__)
        }
    }

}

void ACOsgCompositeViewQt::browseMediaExternally(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        //int loop = media_cycle->getClickedNode();
        int loop = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        //std::cout << "node " << loop << " selected" << std::endl;

        if(loop >= 0)
        {
            #if defined (__APPLE__)
            std::stringstream command;
            //command << "open " << fs::path(media_cycle->getLibrary()->getMedia(loop)->getFileName()).parent_path();// opens the containing directory using the Finder
            command << "open -R '" << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ;// opens the containing directory using the Finder and highlights the file!
            try {
                system(command.str().c_str());
            }
            catch (const exception& e) {
                cout << "ACOsgCompositeViewQt: caught exception while trying to open media file " << media_cycle->getLibrary()->getMedia(loop)->getFileName() << " with the OSX Finder: " << e.what() << endl;
            }
            #endif //defined (__APPLE__)
        }
    }
}

/*
void ACOsgCompositeViewQt::examineMediaExternally(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        //int loop = media_cycle->getClickedNode();
        int loop = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        //std::cout << "node " << loop << " selected" << std::endl;

        if(loop >= 0)
        {
            #if defined (__APPLE__)
            std::stringstream command;
            //command << "open " << fs::path(media_cycle->getLibrary()->getMedia(loop)->getFileName()).parent_path();// opens the containing directory using the Finder
            command << "open -R '" << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ;// opens the containing directory using the Finder and highlights the file!
            try {
                system(command.str().c_str());
            }
            catch (const exception& e) {
                cout << "ACOsgCompositeViewQt: caught exception while trying to open media file " << media_cycle->getLibrary()->getMedia(loop)->getFileName() << " with the OSX Finder: " << e.what() << endl;
            }
            #endif //defined (__APPLE__)
        }
    }
}*/

void ACOsgCompositeViewQt::forwardNextLevel(bool toggle){
    if (media_cycle == 0) return;
    media_cycle->setForwardDown(true);
    if (media_cycle->hasBrowser()){
        int loop = media_cycle->getClickedNode();
        //std::cout << "node " << loop << " selected" << std::endl;
        if(loop >= 0){
            if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS){
                // store first otherwise we store the next state
                media_cycle->storeNavigationState();
                media_cycle->incrementLoopNavigationLevels(loop);
            }
            // in neighbors mode, the node is already unwrapped with forward down and node clicked
            media_cycle->setReferenceNode(loop);
            media_cycle->updateDisplay(true);
        }
   }
}

void ACOsgCompositeViewQt::stopPlayback(){
    if (media_cycle == 0) return;
    media_cycle->setAutoPlay(0);
    media_cycle->muteAllSources();
}

void ACOsgCompositeViewQt::toggleMediaHover(bool toggle){
    if (media_cycle == 0) return;
    //std::cout << "Toggle Media hover " << toggle << std::endl;
    media_cycle->setAutoPlay(toggle);
}

void ACOsgCompositeViewQt::triggerMediaHover(bool trigger){
    if (media_cycle == 0) return;
    //std::cout << "Trigger Media hover " << trigger << std::endl;
    media_cycle->setAutoPlay(trigger);
}


void ACOsgCompositeViewQt::resetBrowser(){
    if (media_cycle == 0) return;
    media_cycle->setCameraRecenter();
}

void ACOsgCompositeViewQt::rotateBrowser(float x, float y){
    if (media_cycle == 0) return;
    float rotation = atan2(-(y-this->height()/2),x-this->width()/2)-atan2(-(refy-this->height()/2),refx-this->width()/2);
    media_cycle->setCameraRotation(refrotation + rotation);
}

void ACOsgCompositeViewQt::zoomBrowser(float x, float y){
    if (media_cycle == 0) return;
    //float refzoom(0),refy(0);
    media_cycle->setCameraZoom(refzoom - (y-refy)/50);
    //media_cycle->setCameraZoom(refzoom + (y-refy) / abs (y-refy) * sqrt( pow((y-refy),2) + pow((x-refx),2) )/50 );
}

void ACOsgCompositeViewQt::translateBrowser(float x, float y){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        // CF don't translate when in the timeline
        if (!( (y >= height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() ) ) && (y <= height() - ( browser_view->getCamera()->getViewport()->y() + septhick) ))) // if clicked on browser view far enough of the central border
            return;
        //if (y>sepy) // CF find better check (mouse in widget test?)

        int loop = media_cycle->getClickedNode();
        //int loop = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        if(loop == -1)
        {
            //float refx(0),refy(0);
            float zoom = media_cycle->getCameraZoom();
            float angle = media_cycle->getCameraRotation();
            float xmove = (refx-x);
            float ymove =-(refy-y);
            float xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
            float ymove2 = ymove*cos(-angle)+xmove*sin(-angle);
            media_cycle->setCameraPosition(refcamx + xmove2/800/zoom , refcamy + ymove2/800/zoom);
        }
    }
}

void ACOsgCompositeViewQt::addMediaOnTimelineTrack(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        int loop = media_cycle->getClickedNode();

        if (mediaOnTrack != -1)
            this->getBrowserRenderer()->resetNodeColor(mediaOnTrack);

        mediaOnTrack = loop;
        if (mediaOnTrack != -1){
            this->getBrowserRenderer()->changeNodeColor(mediaOnTrack, Vec4(1.0,1.0,1.0,1.0));//CF color the node of the media on track in white

            //if ( timeline_renderer->getTrack(0)!=0 )
            //{

            if (sepy==0)
            {
                sepy = height()/4;// CF browser/timeline proportions at startup
                timeline_renderer->updateSize(width(),sepy);
                this->updateBrowserView(width(),height());
                this->updateTimelineView(width(),height());
                this->updateTimelineControlsView(width(),height());
                this->updateHUDCamera(width(),height());

                media_cycle->setNeedsDisplay(true);
            }
            //if (track_playing) {

            //media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
            //track_playing = false;
            //}
            if (timeline_renderer->getNumberOfTracks()==0){
                //this->getTimelineRenderer()->addTrack(loop);
                this->getTimelineRenderer()->addTrack(media_cycle->getLibrary()->getMedia(loop));
            }
            else {
                //this->getTimelineRenderer()->getTrack(0)->updateMedia( loop ); //media_cycle->getLibrary()->getMedia(loop) );
                this->getTimelineRenderer()->getTrack(0)->updateMedia(media_cycle->getLibrary()->getMedia(loop));
            }
            //this->getTimelineControlsRenderer()->getControls(0)->updateMedia( loop ); //media_cycle->getLibrary()->getMedia(loop) );
            media_cycle->setNeedsDisplay(true);
        }
    }
}

void ACOsgCompositeViewQt::toggleTimelinePlayback(bool toggle){
    if (media_cycle == 0) return;
    if ( (media_cycle) && (media_cycle->hasBrowser()) && (timeline_renderer->getTrack(0)!=0) ) {
        //media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
        media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMedia()->getId() );
    }
}

/*void ACOsgCompositeViewQt::adjustTimelineHeight(float y){
    if (media_cycle == 0) return;
}*/

void ACOsgCompositeViewQt::propagateEventToActions( QEvent* event )
{
    //int cnt = 0;
    QListIterator<QAction*> _action(this->actions());
    while (_action.hasNext()){
        //std::cout << "action " << ++cnt << std::endl;
        ACInputActionQt *_act = static_cast<ACInputActionQt *>(_action.next());
        _act->eventAbsorber(event);
    }
}

void ACOsgCompositeViewQt::keyPressEvent( QKeyEvent* event )
{
    this->propagateEventToActions(event);
	if (media_cycle == 0) return;
	osg_view->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void ACOsgCompositeViewQt::keyReleaseEvent( QKeyEvent* event )
{
    this->propagateEventToActions(event);
	if (media_cycle == 0) return;
	osg_view->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void ACOsgCompositeViewQt::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    mousedown = 1;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    osg_view->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
    //browser_view->getEventQueue()->mouseButtonPress(event->x(), event->y()-sepy, button);
    this->propagateEventToActions(event);

    if (media_cycle == 0) return;
	// CF: for OSG y=0 is on the bottom, for Qt on the top
	// browser view top (OSG coordinates): browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y()
	// browser view bottom (OSG coordinates): browser_view->getCamera()->getViewport()->y()
	// browser view top (Qt coordinates): height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() )
	// browser view bottom (Qt coordinates): height() - ( browser_view->getCamera()->getViewport()->y() )

	refx = event->x();
	refy = event->y();
	if ( (event->y() >= height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() ) ) && (event->y() <= height() - ( browser_view->getCamera()->getViewport()->y() + septhick) )) // if clicked on browser view far enough of the central border
	{
		media_cycle->getCameraPosition(refcamx, refcamy);
		refzoom = media_cycle->getCameraZoom();
		refrotation = media_cycle->getCameraRotation();
	}
	else if ( (event->y() >= height() - ( browser_view->getCamera()->getViewport()->y() + septhick)) && (event->y() <= height() - ( browser_view->getCamera()->getViewport()->y() - septhick)) ) // if clicked on timeline view far enough of the central border
	{
		borderdown = 1;
		refsepy = sepy;
	}
	media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::mouseMoveEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    osg_view->getEventQueue()->mouseMotion(event->x(), event->y());
	if (media_cycle == 0) return;

	float zoom, angle;
	float xmove, ymove, xmove2, ymove2;
	float x, y;
	x = event->x();
	y = event->y();
    if ( mousedown == 1 )
	{
		if ( borderdown == 1)
		{
			if ( (y<=height()-septhick) && (y>=septhick))
				sepy = refsepy + (refy-y);
			else
			{
				if ( y>height()-septhick )
					sepy = septhick;
				else if ( y<septhick )
					sepy = height()-septhick;
			}
			timeline_renderer->updateSize(width(),sepy);
        }
    }
    this->propagateEventToActions(event);
	media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::mouseReleaseEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    osg_view->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
    this->propagateEventToActions(event);
	if (media_cycle == 0) return;
    media_cycle->setClickedNode(-1);
	mousedown = 0;
    borderdown = 0;
	media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::prepareFromBrowser()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	browser_renderer->prepareNodes();
	browser_renderer->prepareLabels();
	hud_renderer->preparePointers(browser_view);
	browser_view->setSceneData(browser_renderer->getShapes());
	library_loaded = true;
	//addCamera(renderer_hud->getCamera());
}

// XS TODO this is called for instance when click on node
// check that it does not do too many things
void ACOsgCompositeViewQt::updateTransformsFromBrowser( double frac)
{
	if (media_cycle == 0) return;

	browser_renderer->prepareNodes();
	browser_renderer->prepareLabels();
	hud_renderer->preparePointers();

	// get screen coordinates
	//int closest_node;//CF to deprecate
	//closest_node = browser_renderer->computeScreenCoordinates(browser_view, frac);//CF to deprecate
	//media_cycle->setClosestNode(closest_node);//CF to deprecate
	browser_renderer->computeScreenCoordinates(browser_view, frac);

	// recompute scene graph
	browser_renderer->updateNodes(frac); // animation starts at 0.0 and ends at 1.0
	browser_renderer->updateLabels(frac);

	hud_renderer->updatePointers(browser_view);
	/*osgViewer::ViewerBase::Views views;
	bool have_views;
	view->getViews(views,have_views);
	renderer_hud->updatePointers(views[0]);*/
}

void ACOsgCompositeViewQt::prepareFromTimeline()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	timeline_renderer->prepareTracks();
	timeline_view->setSceneData(timeline_renderer->getShapes());
	timeline_controls_renderer->prepareControls();
	//timeline_controls_view->setSceneData(timeline_controls_renderer->getShapes());
}


void ACOsgCompositeViewQt::updateTransformsFromTimeline( double frac)
{
	if (media_cycle == 0) return;
	//int closest_track;
	// get screen coordinates
	/////////closest_track = timeline_renderer->computeScreenCoordinates(timeline_view, frac); //CF this instead of browser_view for the the simple Viewer
	////////media_cycle->setClosestNode(closest_node);
	// recompute scene graph
	timeline_renderer->updateTracks(frac); // animation starts at 0.0 and ends at 1.0
	//timeline_controls_renderer->updateControls(frac); // animation starts at 0.0 and ends at 1.0
}
