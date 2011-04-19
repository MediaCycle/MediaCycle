//
//  ACOsgCompositeViewQt.cpp
//  MediaCycle
//
//  @author Christian Frisson
//  @date 29/04/10
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
	browser_renderer(0), browser_event_handler(0), timeline_renderer(0), timeline_event_handler(0), timeline_controls_renderer(0), hud_renderer(0),
	#if defined (SUPPORT_AUDIO)
		audio_engine(0),
	#endif //defined (SUPPORT_AUDIO)
	mousedown(0), zoomdown(0), forwarddown(0), autoplaydown(0),rotationdown(0),
	finddown(0),infodown(0),opendown(0),
	borderdown(0), transportdown(0), 
	refx(0.0f), refy(0.0f),
	refcamx(0.0f), refcamy(0.0f),
	refzoom(0.0f),refrotation(0.0f),
	septhick(5),sepy(0.0f),refsepy(0.0f),controls_width(0),screen_width(0),
	library_loaded(false),
	trackdown(0),mediaOnTrack(-1),track_playing(false)
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
	_timer.start(10);

	browser_renderer = new ACOsgBrowserRenderer();
	timeline_renderer = new ACOsgTimelineRenderer();
	timeline_controls_renderer = new ACOsgTimelineControlsRenderer();	
	hud_renderer = new ACOsgHUDRenderer();
	
	//this->setAttribute(Qt::WA_Hover, true);
	setMouseTracking(true); //CF necessary for the hover callback
	
	// Audio waveforms
	screen_width = QApplication::desktop()->screenGeometry().width();
	timeline_renderer->setScreenWidth(screen_width);
	#if defined (SUPPORT_AUDIO)
		audio_engine = 0;
	#endif //defined (SUPPORT_AUDIO)
	
	//
	sepy = 0;//height()/4;// CF browser/timeline proportions at startup
	browser_view = new osgViewer::View;
	browser_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,width(),height()-sepy)); // CF: for OSG y=0 is on the bottom, for Qt on the top
	browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(height()-sepy), 0.001f, 10.0f);
	browser_view->getCamera()->getViewMatrix().makeIdentity();
	browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	browser_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	this->addView(browser_view);
	
	timeline_view = new osgViewer::View;
	timeline_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	//timeline_view->getCamera()->setClearColor(Vec4f(0.14,0.14,0.28,1.0));
	timeline_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	timeline_view->getCamera()->setViewport(new osg::Viewport(controls_width,0,width()-controls_width,sepy));
	
	//perspect
	timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
	timeline_view->getCamera()->getViewMatrix().makeIdentity();
	timeline_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	this->addView(timeline_view);
	
	timeline_controls_view = new osgViewer::View;
	//timeline_controls_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	timeline_controls_view->getCamera()->setClearColor(Vec4f(1.0f,0.14f,0.28f,0.2f));
	timeline_controls_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	timeline_controls_view->getCamera()->setViewport(new osg::Viewport(0,0,controls_width,sepy));
	timeline_controls_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
	timeline_controls_view->getCamera()->getViewMatrix().makeIdentity();
	timeline_controls_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	this->addView(timeline_controls_view);
	
	timeline_renderer->updateSize(width(),sepy);
	
	browser_event_handler = new ACOsgBrowserEventHandler;
	browser_view->addEventHandler(browser_event_handler);
	
	timeline_event_handler = new ACOsgTimelineEventHandler;
	timeline_view->addEventHandler(timeline_event_handler); // CF ((osgViewer::Viewer*) (this))->addEventHandler for the simple Viewer
	timeline_event_handler->setRenderer(timeline_renderer);
}

ACOsgCompositeViewQt::~ACOsgCompositeViewQt(){
	//browser_view->removeEventHandler(browser_event_handler); // reqs OSG >= 2.9.x and shouldn't be necessary
	//timeline_view->removeEventHandler(timeline_event_handler); // reqs OSG >= 2.9.x and shouldn't be necessary
	this->clean(false);
	delete browser_renderer;
	//if (browser_event_handler) delete browser_event_handler;
	browser_event_handler = 0;
	delete timeline_renderer;
	//if (timeline_event_handler) delete timeline_event_handler;
	timeline_event_handler = 0;
	delete timeline_controls_renderer;
	media_cycle = 0;
}

void ACOsgCompositeViewQt::clean(bool updategl){
	browser_event_handler->clean();
	timeline_event_handler->clean();
	mousedown = zoomdown = forwarddown = autoplaydown = rotationdown = 0;
	refx =  refy = refcamx = refcamy = refzoom = refrotation = 0.0f;
	browser_renderer->clean();
	timeline_renderer->clean();
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

void ACOsgCompositeViewQt::setAudioEngine(ACAudioEngine *engine)
{
	audio_engine=engine;
	if(timeline_renderer)timeline_renderer->setAudioEngine(engine);
	if(timeline_event_handler)timeline_event_handler->setAudioEngine(engine);
}

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
		browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,w,h-sepy));
		browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(w)/static_cast<double>(h-sepy), 0.001f, 10.0f);

		timeline_view->getCamera()->setViewport(new osg::Viewport(controls_width,0,w-controls_width,sepy));
		
		#ifdef TIMELINE_RENDERER_ORTHO2D
		//orth2D
		timeline_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,width,0,sepy));
		timeline_view->getCamera()->setViewMatrix(osg::Matrix::identity());
		#else
		//perspect
		timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);}
		#endif
		timeline_controls_view->getCamera()->setViewport(new osg::Viewport(0,0,controls_width,sepy));
		timeline_controls_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);}

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
		media_cycle->updateState();
		frac = media_cycle->getFrac();
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
		
		browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));
		//timeline_view->getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));
		browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,width(),height()-sepy));
		browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(height()-sepy), 0.001f, 10.0f);
		timeline_view->getCamera()->setViewport(new osg::Viewport(controls_width,0,width()-controls_width,sepy));
		#ifdef TIMELINE_RENDERER_ORTHO2D
		//orth2D
		timeline_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,width(),0,sepy));
		timeline_view->getCamera()->setViewMatrix(osg::Matrix::identity());
		#else
		//perspect
		timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
		#endif
		timeline_controls_view->getCamera()->setViewport(new osg::Viewport(0,0,controls_width,sepy));
		timeline_controls_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
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

void ACOsgCompositeViewQt::keyPressEvent( QKeyEvent* event )
{
	if (media_cycle == 0) return;
	osg_view->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	switch( event->key() )
	{
		case Qt::Key_A:
			media_cycle->setForwardDown(1);
			forwarddown = 1;
			break;
		case Qt::Key_C:	
			media_cycle->setCameraRecenter();
			break;
		case Qt::Key_F:	
			finddown = 1;
			break;		
		case Qt::Key_I:	
			infodown = 1;
			break;		
		case Qt::Key_M:	
			media_cycle->muteAllSources();
			break;
		case Qt::Key_O:	
			opendown = 1;
			break;		
		case Qt::Key_Q:
			media_cycle->setAutoPlay(1);
			autoplaydown = 1;
			break;
		case Qt::Key_R:
			rotationdown = 1;
			break;
		case Qt::Key_T:
			trackdown = 1;
			break;	
		case Qt::Key_Z:
			zoomdown = 1;
			break;
		case Qt::Key_Space:
			if ( (media_cycle) && (media_cycle->hasBrowser()) && (timeline_renderer->getTrack(0)!=0) ) {
				transportdown = 1;
				media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
				track_playing = track_playing ? false : true; //CF toggling
			}	
			break;
		default:
			break;
	}
}

void ACOsgCompositeViewQt::keyReleaseEvent( QKeyEvent* event )
{
	if (media_cycle == 0) return;
	osg_view->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	zoomdown = 0;
	forwarddown = 0;
	media_cycle->setForwardDown(0);
	autoplaydown = 0;
	media_cycle->setAutoPlay(0);
	rotationdown = 0;
	finddown = 0;
	infodown = 0;
	opendown = 0;
	transportdown = 0;
	trackdown = 0;
}

void ACOsgCompositeViewQt::mousePressEvent( QMouseEvent* event )
{
	if (media_cycle == 0) return;
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
	
	/*if ( (media_cycle) && (media_cycle->hasBrowser()))
	{
		if ( (finddown == 1) || (opendown == 1) || (forwarddown==1) || (trackdown == 1) )
		{	
			int loop = media_cycle->getClickedNode();
			std::cout << "node " << loop << " selected" << std::endl;
			//media_cycle->hoverCallback(event->x(),event->y());
			//int loop = media_cycle->getClosestNode();
			
			if(loop >= 0)
			{
				
				if (finddown == 1)
				{
#if defined (__APPLE__)
					std::stringstream command;
					//command << "open " << fs::path(media_cycle->getLibrary()->getMedia(loop)->getFileName()).parent_path();// opens the containing directory using the Finder
					command << "open -R '" << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ;// opens the containing directory using the Finder and highlights the file!
					system(command.str().c_str());
#endif //defined (__APPLE__)
				}
				else if (opendown == 1)
				{
#if defined (__APPLE__)
					std::stringstream command;
					ACMediaType _media_type = media_cycle->getLibrary()->getMedia(loop)->getMediaType();
					if (_media_type == MEDIA_TYPE_IMAGE || _media_type == MEDIA_TYPE_VIDEO)
						command << "open -a Preview '";
					else 
						command << "open -R '"; // no iTunes for audio! 3Dmodel default applications?
					command << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ; 
					system(command.str().c_str());
#endif //defined (__APPLE__)
				}	
				else if (forwarddown==1)
				{
					// XSCF 250310 added these 3
					// XS 260810 put this "if" first other+wise we store the next state
					if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
						media_cycle->storeNavigationState();
					
					if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
						media_cycle->incrementLoopNavigationLevels(loop);
					media_cycle->setReferenceNode(loop);
					
					
					//			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
					//			media_cycle->getBrowser()->setState(AC_CHANGING);
					
					media_cycle->updateDisplay(true); //XS250310 was: media_cycle->updateClusters(true);
					// XSCF 250310 removed this:
					// media_cycle->updateNeighborhoods();
					//	media_cycle->updateClusters(false);// CF was true, equivalent to what's following
					
					//				// remainders from updateClusters(true)
					//				media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
					//				media_cycle->getBrowser()->setState(AC_CHANGING);
				}
				else if (trackdown == 1)
				{
					if (mediaOnTrack != -1)
						this->getBrowserRenderer()->resetNodeColor(mediaOnTrack);
					
					mediaOnTrack = loop;
					if (mediaOnTrack != -1)
						this->getBrowserRenderer()->changeNodeColor(mediaOnTrack, Vec4(1.0,1.0,1.0,1.0));//CF color the node of the media on track in white
					
					
					
					//if ( timeline_renderer->getTrack(0)!=0 )
					//{
					
					if (sepy==0)
					{
						sepy = height()/4;// CF browser/timeline proportions at startup
						timeline_renderer->updateSize(width(),sepy);
						browser_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
						browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,width(),height()-sepy)); // CF: for OSG y=0 is on the bottom, for Qt on the top
						browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(height()-sepy), 0.001f, 10.0f);
						browser_view->getCamera()->getViewMatrix().makeIdentity();
						browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
						//browser_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
						
						//timeline_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
						timeline_view->getCamera()->setClearColor(Vec4f(0.14,0.14,0.28,1.0));
						timeline_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
						timeline_view->getCamera()->setViewport(new osg::Viewport(controls_width,0,width()-controls_width,sepy));
#ifdef TIMELINE_RENDERER_ORTHO2D
						//orth2D
						timeline_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,width(),0,sepy));
						timeline_view->getCamera()->setViewMatrix(osg::Matrix::identity());
#else
						//perspect
						timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
						timeline_view->getCamera()->getViewMatrix().makeIdentity();
						timeline_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
#endif
						timeline_controls_view->getCamera()->setClearColor(Vec4f(1.0,0.14,0.28,1.0));
						timeline_controls_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
						timeline_controls_view->getCamera()->setViewport(new osg::Viewport(0,0,controls_width,sepy));
						timeline_controls_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
						timeline_controls_view->getCamera()->getViewMatrix().makeIdentity();
						timeline_controls_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
						
						
						media_cycle->setNeedsDisplay(true);
					}	
					//if (track_playing) {
					//
					//	media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
					//	track_playing = false;
					//	}	
					if (timeline_renderer->getNumberOfTracks()==0){
						this->getTimelineRenderer()->addTrack(loop);
					}
					else
						this->getTimelineRenderer()->getTrack(0)->updateMedia( loop ); //media_cycle->getLibrary()->getMedia(loop) );
					//this->getTimelineControlsRenderer()->getControls(0)->updateMedia( loop ); //media_cycle->getLibrary()->getMedia(loop) );
					media_cycle->setNeedsDisplay(true);
					//}
				}
			}
		}	
		media_cycle->setClickedNode(-1);
	}*/
	media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::mouseMoveEvent( QMouseEvent* event )
{
	if (media_cycle == 0) return;
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
		else
		{	
			if (infodown == 1)
			{
				int closest_node = media_cycle->getClosestNode();
				if (closest_node > -1)
					std::cout << "Closest node: " << closest_node << std::endl;
			}	
			
			if ( (forwarddown == 0) && (trackdown == 0)) 
			{
				if ( (event->y() >= height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() ) ) && (event->y() <= height() - ( browser_view->getCamera()->getViewport()->y() + septhick) ))
				{
					if ( zoomdown==1 )
					{
						media_cycle->setCameraZoom(refzoom - (y-refy)/50);
						//media_cycle->setCameraZoom(refzoom + (y-refy) / abs (y-refy) * sqrt( pow((y-refy),2) + pow((x-refx),2) )/50 );
					}
					else if ( rotationdown==1 )
					{
						float rotation = atan2(-(y-this->height()/2),x-this->width()/2)-atan2(-(refy-this->height()/2),refx-this->width()/2);
						media_cycle->setCameraRotation(refrotation + rotation);
					}	
					else // translation
					{
						zoom = media_cycle->getCameraZoom();
						angle = media_cycle->getCameraRotation();
						xmove = (refx-x);
						ymove =-(refy-y);
						xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
						ymove2 = ymove*cos(-angle)+xmove*sin(-angle);		
						media_cycle->setCameraPosition(refcamx + xmove2/800/zoom , refcamy + ymove2/800/zoom);
					}
				}
			}
		}
	}
	media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::mouseReleaseEvent( QMouseEvent* event )
{
	if (media_cycle == 0) return;
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
	
	if (media_cycle->hasBrowser())
	{
		if ( (finddown == 1) || (opendown == 1) || (forwarddown==1) || (trackdown == 1) )
		{	
			int loop = media_cycle->getClickedNode();
			std::cout << "node " << loop << " selected" << std::endl;
			//media_cycle->hoverCallback(event->x(),event->y());
			//int loop = media_cycle->getClosestNode();
			
			if(loop >= 0)
			{
				
				if (finddown == 1)
				{
					#if defined (__APPLE__)
						std::stringstream command;
						//command << "open " << fs::path(media_cycle->getLibrary()->getMedia(loop)->getFileName()).parent_path();// opens the containing directory using the Finder
						command << "open -R '" << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ;// opens the containing directory using the Finder and highlights the file!
						system(command.str().c_str());
					#endif //defined (__APPLE__)
				}
				else if (opendown == 1)
				{
					#if defined (__APPLE__)
						std::stringstream command;
						ACMediaType _media_type = media_cycle->getLibrary()->getMedia(loop)->getMediaType();
						if (_media_type == MEDIA_TYPE_IMAGE || _media_type == MEDIA_TYPE_VIDEO)
							command << "open -a Preview '";
						else 
							command << "open -R '"; // no iTunes for audio! 3Dmodel default applications?
						command << media_cycle->getLibrary()->getMedia(loop)->getFileName() << "'" ; 
						system(command.str().c_str());
					#endif //defined (__APPLE__)
				}	
				else if (forwarddown==1)
				{
					if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS){
						// store first otherwise we store the next state
						media_cycle->storeNavigationState();
						media_cycle->incrementLoopNavigationLevels(loop);
					}
					media_cycle->setReferenceNode(loop);
					media_cycle->updateDisplay(true);
				}
				else if (trackdown == 1)
				{
					if (mediaOnTrack != -1)
						this->getBrowserRenderer()->resetNodeColor(mediaOnTrack);
					
					mediaOnTrack = loop;
					if (mediaOnTrack != -1)
						this->getBrowserRenderer()->changeNodeColor(mediaOnTrack, Vec4(1.0,1.0,1.0,1.0));//CF color the node of the media on track in white
						
					/*if ( timeline_renderer->getTrack(0)!=0 )
					{*/
						
						if (sepy==0)
						{
							sepy = height()/4;// CF browser/timeline proportions at startup
							timeline_renderer->updateSize(width(),sepy);
							browser_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
							browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,width(),height()-sepy)); // CF: for OSG y=0 is on the bottom, for Qt on the top
							browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(height()-sepy), 0.001f, 10.0f);
							browser_view->getCamera()->getViewMatrix().makeIdentity();
							browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
							//browser_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
							
							//timeline_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
							timeline_view->getCamera()->setClearColor(Vec4f(0.14,0.14,0.28,1.0));
							timeline_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
							timeline_view->getCamera()->setViewport(new osg::Viewport(controls_width,0,width()-controls_width,sepy));
							#ifdef TIMELINE_RENDERER_ORTHO2D
							//orth2D
							timeline_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,width(),0,sepy));
							timeline_view->getCamera()->setViewMatrix(osg::Matrix::identity());
							#else
							//perspect
							timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
							timeline_view->getCamera()->getViewMatrix().makeIdentity();
							timeline_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
							#endif
							timeline_controls_view->getCamera()->setClearColor(Vec4f(1.0,0.14,0.28,1.0));
							timeline_controls_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
							timeline_controls_view->getCamera()->setViewport(new osg::Viewport(0,0,controls_width,sepy));
							timeline_controls_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
							timeline_controls_view->getCamera()->getViewMatrix().makeIdentity();
							timeline_controls_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
							
							
							media_cycle->setNeedsDisplay(true);
						}	
						/*
						if (track_playing) {
						
							media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
							track_playing = false;
							}	
						*/
						if (timeline_renderer->getNumberOfTracks()==0){
							this->getTimelineRenderer()->addTrack(loop);
						}
						else
							this->getTimelineRenderer()->getTrack(0)->updateMedia( loop ); //media_cycle->getLibrary()->getMedia(loop) );
						//this->getTimelineControlsRenderer()->getControls(0)->updateMedia( loop ); //media_cycle->getLibrary()->getMedia(loop) );
						media_cycle->setNeedsDisplay(true);
					//}
				}
			}
		}	
		media_cycle->setClickedNode(-1);
	}
	mousedown = 0;
	borderdown = 0;
	media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::prepareFromBrowser()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	browser_renderer->prepareNodes(); 
	browser_renderer->prepareLabels();
	hud_renderer->preparePointers();
	browser_view->setSceneData(browser_renderer->getShapes());
	library_loaded = true;
}

// XS TODO this is called for instance when click on node
// check that it does not do too many things
void ACOsgCompositeViewQt::updateTransformsFromBrowser( double frac)
{
	if (media_cycle == 0) return;
	int closest_node;	
	
	browser_renderer->prepareNodes();
	browser_renderer->prepareLabels();
	hud_renderer->preparePointers();
	
	// get screen coordinates
	closest_node = browser_renderer->computeScreenCoordinates(browser_view, frac);
	media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	browser_renderer->updateNodes(frac); // animation starts at 0.0 and ends at 1.0
	browser_renderer->updateLabels(frac);
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
