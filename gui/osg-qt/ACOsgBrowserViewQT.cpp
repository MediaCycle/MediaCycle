//
//  ACOsgBrowserViewQT.cpp
//  AudioCycle
//
//  Created by Christian F. on 15/04/09.
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

#include "ACOsgBrowserViewQT.h"
#include <cmath>

ACOsgBrowserViewQT::ACOsgBrowserViewQT( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):
	QGLWidget(parent, shareWidget, f),
	mousedown(0), zoomdown(0), forwarddown(0), autoplaydown(0),rotationdown(0),
	refx(0.0f), refy(0.0f),
	refcamx(0.0f), refcamy(0.0f),
	refzoom(0.0f),refrotation(0.0f)
{
	osg_view = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
	setFocusPolicy(Qt::StrongFocus);// CF instead of ClickFocus

	getCamera()->setViewport(new osg::Viewport(0.0f,0.0f,width(),height()));
	getCamera()->setProjectionMatrixAsPerspective(45.0f, getCamera()->getViewport()->aspectRatio(), 0.001f, 10.0f);
	getCamera()->getViewMatrix().makeIdentity();
	getCamera()->setViewMatrixAsLookAt(Vec3(0.0f,0.0f,1.0f), Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,1.0f,0.0f));
	getCamera()->setGraphicsContext(getGraphicsWindow());

	setThreadingModel(osgViewer::Viewer::SingleThreaded);
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

	renderer = new ACOsgBrowserRenderer();
	
	//this->setAttribute(Qt::WA_Hover, true);
	setMouseTracking(true); //CF necessary for the hover callback
}

void ACOsgBrowserViewQT::setMediaCycle(MediaCycle* _media_cycle)
{
	media_cycle = _media_cycle;
	renderer->setMediaCycle(media_cycle);
	
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	((osgViewer::Viewer*) (this))->addEventHandler(event_handler);
}

void ACOsgBrowserViewQT::resizeGL( int width, int height )
{
	osg_view->getEventQueue()->windowResize(0, 0, width, height );
	osg_view->resized(0,0,width,height);
}

void ACOsgBrowserViewQT::paintGL()
{
	if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
		updateTransformsFromBrowser(0.0);
	frame();
}

// called according to timer
void ACOsgBrowserViewQT::updateGL()
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

	if(getCamera() && media_cycle && media_cycle->hasBrowser())
	{
		
		float x=0.0f, y=0.0f, zoom, angle;
		float upx, upy;
		
		zoom = media_cycle->getCameraZoom();
		angle = media_cycle->getCameraRotation();
		media_cycle->getCameraPosition(x, y);
		upx = cos(-angle+PI/2);
		upy = sin(-angle+PI/2);		
		
		getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));
	}
	this->updateTransformsFromBrowser(frac);
	/*
	if (frac >= 0.0 && frac < 1.0 ) {
		setMouseTracking(false); //CF necessary for the hover callback
	}
	else 
		setMouseTracking(true); 
	*/ 
	QGLWidget::updateGL();
	//if (frac < 1.0) {
	//	media_cycle->setNeedsDisplay(true);
	//}
	//else
		media_cycle->setNeedsDisplay(false);
}	

void ACOsgBrowserViewQT::keyPressEvent( QKeyEvent* event )
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
		default:
			break;	
	}
}

void ACOsgBrowserViewQT::keyReleaseEvent( QKeyEvent* event )
{
	osg_view->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

	zoomdown = 0;
	forwarddown = 0;
	media_cycle->setForwardDown(0);
	autoplaydown = 0;
	media_cycle->setAutoPlay(0);
	rotationdown = 0;
}

void ACOsgBrowserViewQT::mousePressEvent( QMouseEvent* event )
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
	
	refx = event->x();
	refy = event->y();
	media_cycle->getCameraPosition(refcamx, refcamy);
	refzoom = media_cycle->getCameraZoom();
	refrotation = media_cycle->getCameraRotation();
	media_cycle->setNeedsDisplay(true);
}

void ACOsgBrowserViewQT::mouseMoveEvent( QMouseEvent* event )
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
	if ( (mousedown==1) && (forwarddown == 0) ) {
		if ( zoomdown==1 ) {
			media_cycle->setCameraZoom(refzoom - (y-refy)/50);
			//media_cycle->setCameraZoom(refzoom + (y-refy) / abs (y-refy) * sqrt( pow((y-refy),2) + pow((x-refx),2) )/50 );
		}
		else if ( rotationdown==1 ) {
			float rotation = atan2(-(y-this->height()/2),x-this->width()/2)-atan2(-(refy-this->height()/2),refx-this->width()/2);
			media_cycle->setCameraRotation(refrotation + rotation);
		}	
		else { // translation
			zoom = media_cycle->getCameraZoom();
			angle = media_cycle->getCameraRotation();
			xmove = (refx-x);
			ymove =-(refy-y);
			xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
			ymove2 = ymove*cos(-angle)+xmove*sin(-angle);		
			media_cycle->setCameraPosition(refcamx + xmove2/800/zoom , refcamy + ymove2/800/zoom);
		}
	}
	media_cycle->setNeedsDisplay(true);
}

void ACOsgBrowserViewQT::mouseReleaseEvent( QMouseEvent* event )
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
		if ( forwarddown==1 )
		{	
			int loop = media_cycle->getClickedNode();
			std::cout << "node " << loop << " selected" << std::endl;
			//media_cycle->hoverCallback(event->x(),event->y());
			//int loop = media_cycle->getClosestNode();
			
			if(loop >= 0)
			{
				media_cycle->incrementLoopNavigationLevels(loop);
				media_cycle->setReferenceNode(loop);
				
				// XSCF 250310 added these 3
				media_cycle->pushNavigationState();

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
		}	
		media_cycle->setClickedNode(-1);
	}
	forwarddown = 0;
	mousedown = 0;
	media_cycle->setNeedsDisplay(true);
}

void ACOsgBrowserViewQT::prepareFromBrowser()
{
	//setMouseTracking(false); //CF necessary for the hover callback
	renderer->prepareNodes(); 
	renderer->prepareLabels();
	this->setSceneData(renderer->getShapes());
}


void ACOsgBrowserViewQT::updateTransformsFromBrowser( double frac)
{
	int closest_node;	
	// get screen coordinates
	closest_node = renderer->computeScreenCoordinates(this, frac);
	media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	renderer->updateNodes(frac); // animation starts at 0.0 and ends at 1.0
	renderer->updateLabels(frac);
}