//
//  AGOsgCompositeViewQt.cpp
//  MediaCycle+AudioGarden
//
//  @author Christian Frisson
//  @date 01/07/10
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
//  customized for AudioGarden
//

#ifdef USE_AUDIOGARDEN

#define PI 3.1415926535897932384626433832795f

#include "AGOsgCompositeViewQt.h"
#include <cmath>
#include <QDesktopWidget>

using namespace osg;

AGOsgCompositeViewQt::AGOsgCompositeViewQt( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):
	QGLWidget(parent, shareWidget, f),
	mousedown(0), zoomdown(0), forwarddown(0), autoplaydown(0),rotationdown(0),
	borderdown(0), transportdown(0), selectrhythmpattern(false), selectgrains(false),
	refx(0.0f), refy(0.0f),
	refcamx(0.0f), refcamy(0.0f),
	refzoom(0.0f),refrotation(0.0f),
	septhick(5),sepx(0.0f),sepy(0.0f),refsepy(0.0f),screen_width(0),
	selectedRhythmPattern(-1),
	autosynth(false),track_playing(false)
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
	
	synth = new AGSynthesis();
	synthAudio = NULL;
	
	//this->setAttribute(Qt::WA_Hover, true);
	setMouseTracking(true); //CF necessary for the hover callback
	
	// Audio waveforms
	screen_width = QApplication::desktop()->screenGeometry().width();
	timeline_renderer->setScreenWidth(screen_width);
}

void AGOsgCompositeViewQt::setMediaCycle(MediaCycle* _media_cycle)
{
	media_cycle = _media_cycle;
	browser_renderer->setMediaCycle(media_cycle);
	timeline_renderer->setMediaCycle(media_cycle);
	
	sepy = height()/4;// CF browser/timeline proportions at startup
	browser_view = new osgViewer::View;
	browser_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,width(),height()-sepy)); // CF: for OSG y=0 is on the bottom, for Qt on the top
	browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(height()-sepy), 0.001f, 10.0f);
	browser_view->getCamera()->getViewMatrix().makeIdentity();
	browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	//browser_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	this->addView(browser_view);
	
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	browser_view->addEventHandler(event_handler); // CF ((osgViewer::Viewer*) (this))->addEventHandler for the simple Viewer
	
	timeline_view = new osgViewer::View;
	//timeline_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	timeline_view->getCamera()->setClearColor(Vec4f(0.14,0.14,0.28,1.0));
	timeline_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	timeline_view->getCamera()->setViewport(new osg::Viewport(0,0,width(),sepy));
	timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
	timeline_view->getCamera()->getViewMatrix().makeIdentity();
	timeline_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	this->addView(timeline_view);
	
	timeline_event_handler = new ACOsgTimelineEventHandler;
	timeline_event_handler->setMediaCycle(media_cycle);
	timeline_view->addEventHandler(timeline_event_handler); // CF ((osgViewer::Viewer*) (this))->addEventHandler for the simple Viewer
	timeline_event_handler->setRenderer(timeline_renderer);
	
	synth->setMediaCycle(media_cycle);
}

void AGOsgCompositeViewQt::initializeGL()
{/*
  if (getGraphicsWindow()->isRealized()) {
  
  unsigned int _screen_width, _screen_height;
  if ( screen_width != _screen_width){
  _screen_width = timeline_view->getCamera()->getGraphicsContext()->getTraits()->width;
  std::cout << "Initial width: " << _screen_width << std::endl;
  //this->screen_width = _screen_width;
  //timeline_renderer->updateScreenWidth(_screen_width);
  }	
  }*/
}

void AGOsgCompositeViewQt::resizeGL( int width, int height )
{
	//std::cout << "height() " << browser_view->getCamera()->getViewport()->height()+timeline_view->getCamera()->getViewport()->height() << " height " << height << std::endl;
	float prevheight = browser_view->getCamera()->getViewport()->height()+timeline_view->getCamera()->getViewport()->height();
	sepy *= height/prevheight;
	
	osg_view->getEventQueue()->windowResize(0, 0, width, height );
	osg_view->resized(0,0,width,height);
		
	browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,width,height-sepy));
	browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width)/static_cast<double>(height-sepy), 0.001f, 10.0f);
	timeline_view->getCamera()->setViewport(new osg::Viewport(0,0,width,sepy));
	timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);}

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

// CF to do: understand paintGL vs updateGL to use them more correctly
void AGOsgCompositeViewQt::paintGL()
{
	//CF to improve, we want to know if the view is being animated to force a frequent refresh of the positions:
	if (media_cycle->getBrowser()->getState() == AC_CHANGING)
		updateTransformsFromBrowser(media_cycle->getFrac());
	frame();
}

// called according to timer
void AGOsgCompositeViewQt::updateGL()
{
	double frac = 0.0;
	
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
		timeline_view->getCamera()->setViewport(new osg::Viewport(0,0,width(),sepy));
		timeline_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, 1.0f, 0.001f, 10.0f);//static_cast<double>(width())/static_cast<double>(sepy), 0.001f, 10.0f);
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

void AGOsgCompositeViewQt::keyPressEvent( QKeyEvent* event )
{
	osg_view->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	switch( event->key() )
	{
		//CF keys are ordered alphabetically below:
		case Qt::Key_A:
			media_cycle->setForwardDown(1);
			forwarddown = 1;
			break;
		case Qt::Key_C:	
			media_cycle->setCameraRecenter();
			break;
		case Qt::Key_G:
			selectgrains = true;
			break;
		case Qt::Key_L:
			//CF used in ACAudioGardenOsgQt
			break;
		case Qt::Key_M:	
			media_cycle->muteAllSources();
			break;	
		case Qt::Key_P:
			selectrhythmpattern = true;
			break;	
		case Qt::Key_Q:
			media_cycle->setAutoPlay(1);
			autoplaydown = 1;
			break;
		case Qt::Key_R:
			rotationdown = 1;
			break;
		case Qt::Key_Space:
			if ( (media_cycle) && (media_cycle->hasBrowser()) && (timeline_renderer->getTrack(0)!=NULL) )
			{
				transportdown = 1;
				
				int mIx = timeline_renderer->getTrack(0)->getMediaIndex();
				if (mIx == -1){
					if (track_playing)
					{	
						audio_engine->getFeedback()->stopExtSource();
						//audio_engine->getFeedback()->deleteExtSource();
					}	
					else
					{	
						//media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
						audio_engine->getFeedback()->loopExtSource();
						//usleep(2000000);//CF 2 sec, j'arrive!
					}	
					track_playing = track_playing ? false : true; //CF toggling
				}
				else {
					media_cycle->getBrowser()->toggleSourceActivity( mIx );
				}
			}
			break;
		case Qt::Key_Z:
			zoomdown = 1;
			break;	
		default:
			break;
	}
}

void AGOsgCompositeViewQt::keyReleaseEvent( QKeyEvent* event )
{
	osg_view->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	switch( event->key() )
	{
		case Qt::Key_G:
			if (autosynth)
				this->synthesize();
			break;
		default:
			break;
	}		
			
	zoomdown = 0;
	forwarddown = 0;
	media_cycle->setForwardDown(0);
	autoplaydown = 0;
	media_cycle->setAutoPlay(0);
	rotationdown = 0;
	transportdown = 0;
	selectrhythmpattern = false;
	selectgrains = false;
}

void AGOsgCompositeViewQt::mousePressEvent( QMouseEvent* event )
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

void AGOsgCompositeViewQt::mouseMoveEvent( QMouseEvent* event )
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
		}
		else
		{	
			if ( (forwarddown == 0) && (selectrhythmpattern == false) && (selectgrains == false)) 
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

void AGOsgCompositeViewQt::mouseReleaseEvent( QMouseEvent* event )
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
	
	if ( (media_cycle) && (media_cycle->hasBrowser()))
	{
		if ( (forwarddown==1) || (selectrhythmpattern == true) || (selectgrains == true))
		{	
			int loop = media_cycle->getClickedNode();
			std::cout << "node " << loop << " selected" << std::endl;
			//media_cycle->hoverCallback(event->x(),event->y());
			//int loop = media_cycle->getClosestNode();
			
			if(loop >= 0)
			{
				if (forwarddown==1)
				{	
					if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
						media_cycle->incrementLoopNavigationLevels(loop);
					media_cycle->setReferenceNode(loop);
					
					// XSCF 250310 added these 3
					if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
						media_cycle->storeNavigationState();
					//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
					//media_cycle->getBrowser()->setState(AC_CHANGING);
					
					//			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
					//			media_cycle->getBrowser()->setState(AC_CHANGING);
					
					media_cycle->updateDisplay(true); //XS250310 was: media_cycle->updateClusters(true);
					// XSCF 250310 removed this:
					// media_cycle->updateNeighborhoods();
					//					media_cycle->updateClusters(false);// CF was true, equivalent to what's following
					
					// remainders from updateClusters(true)
//					media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
//					media_cycle->getBrowser()->setState(AC_CHANGING);
				}
				else if (selectrhythmpattern == true)
				{
					if (selectedRhythmPattern != -1)
						this->getBrowserRenderer()->resetNodeColor(selectedRhythmPattern);
					
					selectedRhythmPattern = loop;
					if (selectedRhythmPattern != -1)
						this->getBrowserRenderer()->changeNodeColor(selectedRhythmPattern, Vec4(1.0,1.0,1.0,1.0));//CF color the rhythm pattern in white
					
					if (timeline_renderer->getNumberOfTracks()==0){
						this->getTimelineRenderer()->addTrack(loop);
					}
					else
						this->getTimelineRenderer()->getTrack(0)->updateMedia(loop);
					
					/*if ( timeline_renderer->getTrack(0)!=NULL )
					{
						if (track_playing) {
							audio_engine->getFeedback()->stopExtSource();
							audio_engine->getFeedback()->deleteExtSource();
							track_playing = false;
						}	
							
						//CF possible only for audio? then do some tests
						ACAudio* tempAudio = (ACAudio*) media_cycle->getLibrary()->getMedia(loop);

						//delete synthAudio;
						synthAudio = new ACAudio( *tempAudio, false);
						float* tempBuffer = (float*)synthAudio->getMonoSamples();
						audio_engine->getFeedback()->createExtSource(tempBuffer, synthAudio->getNFrames() );
						
						this->getTimelineRenderer()->getTrack(0)->updateMedia( synthAudio ); //media_cycle->getLibrary()->getMedia(loop) );
						delete[] tempBuffer;
						media_cycle->setNeedsDisplay(true);
					}*/
					media_cycle->setNeedsDisplay(true);
				}
				else if (selectgrains == true)
				{
					media_cycle->getBrowser()->toggleNode(loop);
					media_cycle->getBrowser()->dumpSelectedNodes();
				}
			}
		}	
		media_cycle->setClickedNode(-1);
	}
	mousedown = 0;
	borderdown = 0;
	media_cycle->setNeedsDisplay(true);
}

void AGOsgCompositeViewQt::prepareFromBrowser()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	browser_renderer->prepareNodes(); 
	browser_renderer->prepareLabels();
	browser_view->setSceneData(browser_renderer->getShapes());
}


void AGOsgCompositeViewQt::updateTransformsFromBrowser( double frac)
{
	int closest_node;	
	// get screen coordinates
	closest_node = browser_renderer->computeScreenCoordinates(browser_view, frac);
	media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	browser_renderer->updateNodes(frac); // animation starts at 0.0 and ends at 1.0
	browser_renderer->updateLabels(frac);
}

void AGOsgCompositeViewQt::prepareFromTimeline()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	timeline_renderer->prepareTracks(); 
	timeline_view->setSceneData(timeline_renderer->getShapes());
	//timeline_view->setSceneData(osgDB::readNodeFile("cessnafire.osg"));
}


void AGOsgCompositeViewQt::updateTransformsFromTimeline( double frac)
{
	//int closest_track;	
	// get screen coordinates
	/////////closest_track = timeline_renderer->computeScreenCoordinates(timeline_view, frac); //CF this instead of browser_view for the the simple Viewer
	////////media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	timeline_renderer->updateTracks(frac); // animation starts at 0.0 and ends at 1.0
}

void AGOsgCompositeViewQt::synthesize()
{
	if ( this->getSelectedRhythmPattern() > -1 && media_cycle->getBrowser()->getSelectedNodes().size() > 0)
	{	
		// Stop the track playback
		if (track_playing) {
			audio_engine->getFeedback()->stopExtSource();
			audio_engine->getFeedback()->deleteExtSource();
			track_playing = false;
		}	
		
		// Synthesize
		synth->compute(this->getSelectedRhythmPattern(), media_cycle->getBrowser()->getSelectedNodes());
		//synth->saveAsWav("./synthesis.wav");
		// Display the synthesis
		if (synthAudio) delete synthAudio;
		synthAudio = new ACAudio();
		synthAudio->setData(synth->getSound(),synth->getLength());
		//synthAudio->computeWaveform( this->getSynth()->getSound()  );
		this->getTimelineRenderer()->getTrack(0)->updateMedia( synthAudio ); //media_cycle->getLibrary()->getMedia(loop) );
		media_cycle->setNeedsDisplay(true);
		
		// Playback the synthesis
		audio_engine->getFeedback()->createExtSource(this->getSynth()->getSound(), this->getSynth()->getLength());
		audio_engine->getFeedback()->loopExtSource();
		track_playing = true;
	}
}

void AGOsgCompositeViewQt::resetSynth()
{
	// Stop the track playback
	if (track_playing) {
		audio_engine->getFeedback()->stopExtSource();
		audio_engine->getFeedback()->deleteExtSource();
		track_playing = false;
	}	
	
	// Unselect pattern and grains
	if (selectedRhythmPattern != -1 )
		this->getBrowserRenderer()->resetNodeColor(selectedRhythmPattern);
	selectedRhythmPattern = -1;
	media_cycle->getBrowser()->unselectNodes();
	
	// Empty the synthesizer buffer but keep the synthesis parameters
	synth->resetSound();
	
	// Empty the visual track
	this->getTimelineRenderer()->getTrack(0)->clearMedia();
	media_cycle->setNeedsDisplay(true);	
}	

void AGOsgCompositeViewQt::stopSound()
{
	// Stop the track playback
	if (track_playing) {
		audio_engine->getFeedback()->stopExtSource();
		audio_engine->getFeedback()->deleteExtSource();
		track_playing = false;
	}
}

#endif