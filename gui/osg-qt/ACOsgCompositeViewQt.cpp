//
//  ACOsgCompositeViewQt.cpp
//  MediaCycle
//
//  Created by Christian F. on 29/04/10.
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

#define PI 3.1415926535897932384626433832795f

#include "ACOsgCompositeViewQt.h"
#include <cmath>

ACOsgCompositeViewQt::ACOsgCompositeViewQt( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):
	QGLWidget(parent, shareWidget, f),
	mousedown(0), zoomdown(0), forwarddown(0), autoplaydown(0),rotationdown(0),
	borderdown(0), transportdown(0), setrhythmpatterndown(0),
	refx(0.0f), refy(0.0f),
	refcamx(0.0f), refcamy(0.0f),
	refzoom(0.0f),refrotation(0.0f),
	septhick(5),sepx(0.0f),sepy(0.0f),refsepy(0.0f)
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
	
	//this->setAttribute(Qt::WA_Hover, true);
	setMouseTracking(true); //CF necessary for the hover callback
}

void ACOsgCompositeViewQt::setMediaCycle(MediaCycle* _media_cycle)
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
}

void ACOsgCompositeViewQt::resizeGL( int width, int height )
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
}

void ACOsgCompositeViewQt::paintGL()
{
	if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
	{	
		updateTransformsFromBrowser(0.0);
		//updateTransformsFromTimeline(0.0);
	}	
	frame();
}

// called according to timer
void ACOsgCompositeViewQt::updateGL()
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

void ACOsgCompositeViewQt::keyPressEvent( QKeyEvent* event )
{
	osg_view->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	switch( event->key() )
	{
		case Qt::Key_Z:
			zoomdown = 1;
			break;
		case Qt::Key_A:
			media_cycle->setForwardDown(1);
			forwarddown = 1;
			break;
		case Qt::Key_Q:
			media_cycle->setAutoPlay(1);
			autoplaydown = 1;
			break;
		case Qt::Key_R:
			rotationdown = 1;
			break;
		case Qt::Key_M:	
			media_cycle->muteAllSources();
			break;
		case Qt::Key_Space:
			if ( (media_cycle) && (media_cycle->hasBrowser()) && (timeline_renderer->getTrack(0)!=NULL) ) {
				transportdown = 1;
				media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
			}	
			break;
		case Qt::Key_P:
			setrhythmpatterndown = 1;
			break;	
		default:
			break;
	}
}

void ACOsgCompositeViewQt::keyReleaseEvent( QKeyEvent* event )
{
	osg_view->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	zoomdown = 0;
	forwarddown = 0;
	media_cycle->setForwardDown(0);
	autoplaydown = 0;
	media_cycle->setAutoPlay(0);
	rotationdown = 0;
	transportdown = 0;
	setrhythmpatterndown = 0;
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
	media_cycle->setNeedsDisplay(1);
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
			if ( (forwarddown == 0) && (setrhythmpatterndown == 0) ) 
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
	media_cycle->setNeedsDisplay(1);
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
	
	if ( (media_cycle) && (media_cycle->hasBrowser()))
	{
		if ( (forwarddown==1) || (setrhythmpatterndown == 1) )
		{	
			int loop = media_cycle->getClickedNode();
			std::cout << "node " << loop << " selected" << std::endl;
			//media_cycle->hoverCallback(event->x(),event->y());
			//int loop = media_cycle->getClosestNode();
			
			if(loop >= 0)
			{
				if (forwarddown==1)
				{	
					//media_cycle->incrementLoopNavigationLevels(loop);
					media_cycle->setReferenceNode(loop);
					
					// XSCF 250310 added these 3
					media_cycle->pushNavigationState();
					media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
					media_cycle->getBrowser()->setState(AC_CHANGING);
					
					media_cycle->updateNeighborhoods();
					media_cycle->updateClusters(false);// CF was true, equivalent to what's following
					
					// remainders from updateClusters(true)
					media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
					media_cycle->getBrowser()->setState(AC_CHANGING);
				}
				else if (setrhythmpatterndown == 1)
				{
					if ( timeline_renderer->getTrack(0)!=NULL ) {
						if ( (timeline_renderer->getTrack(0)->getMediaIndex() != loop) )
						{	
							//if (timeline_renderer->getTrack(0)->getMediaIndex() != -1)
								media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex(), 0 );
							timeline_renderer->getTrack(0)->setMediaIndex(loop);
						}	
					}
				}	
			}
		}	
		media_cycle->setClickedNode(-1);
	}
	mousedown = 0;
	borderdown = 0;
	media_cycle->setNeedsDisplay(1);
}

void ACOsgCompositeViewQt::prepareFromBrowser()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	browser_renderer->prepareNodes(); 
	browser_renderer->prepareLabels();
	browser_view->setSceneData(browser_renderer->getShapes());
}


void ACOsgCompositeViewQt::updateTransformsFromBrowser( double frac)
{
	int closest_node;	
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
	//timeline_view->setSceneData(osgDB::readNodeFile("cessnafire.osg"));
}


void ACOsgCompositeViewQt::updateTransformsFromTimeline( double frac)
{
	//int closest_track;	
	// get screen coordinates
	/////////closest_track = timeline_renderer->computeScreenCoordinates(timeline_view, frac); //CF this instead of browser_view for the the simple Viewer
	////////media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	timeline_renderer->updateTracks(frac); // animation starts at 0.0 and ends at 1.0
}