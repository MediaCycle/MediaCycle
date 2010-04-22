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
	setFocusPolicy(Qt::ClickFocus);

	//CF comment out this block for CompositeViewers
	getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
	getCamera()->setProjectionMatrixAsPerspective(45.0f, getCamera()->getViewport()->aspectRatio(), 0.1f, 10.0f);
	getCamera()->getViewMatrix().makeIdentity();
	getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	getCamera()->setGraphicsContext(getGraphicsWindow());

	setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

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
	
	//CF uncomment this for CompositeViewer
/*	
	database_view = new osgViewer::View;
	database_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	database_view->getCamera()->setViewport(new osg::Viewport(0,height()/4,width(),3*height()/4));
	database_view->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width())/static_cast<double>(3*height()/4), 0.1f, 10.0f);
	database_view->getCamera()->getViewMatrix().makeIdentity();
	database_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	
	this->addView(database_view);
*/	
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	this->addEventHandler(event_handler); // CF database_view->addEventHandler for CompositeViewer
	
	//CF uncomment this for CompositeViewer	
/*
	element_view = new osgViewer::View;
	//element_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
	element_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
	element_view->getCamera()->setViewport(new osg::Viewport(0,0,width(),height()/4));
	element_view->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width())/static_cast<double>(height()/4), 0.1f, 10.0f);
	element_view->getCamera()->getViewMatrix().makeIdentity();
	element_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	
	this->addView(element_view);
*/
}

void ACOsgBrowserViewQT::resizeGL( int width, int height )
{
  osg_view->getEventQueue()->windowResize(0, 0, width, height );
  osg_view->resized(0,0,width,height);
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

	if(getCamera() && media_cycle) //CF: database_view->getCamera() for CompositeViewer
	{
		
		float x=0.0, y=0.0, zoom, angle;
		float upx, upy;
		
		zoom = media_cycle->getCameraZoom();
		angle = media_cycle->getCameraRotation();
		media_cycle->getCameraPosition(x, y);
		float pi = 3.14; //CF
		upx = cos(-angle+pi/2);
		upy = sin(-angle+pi/2);		
		
		getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));//CF database_view->getCamera() for the CompositeViewer
	}
	
	this->updateTransformsFromBrowser(frac);
	if (frac != 0.0)
		setMouseTracking(true); //CF necessary for the hover callback
	QGLWidget::updateGL();
	media_cycle->setNeedsDisplay(false);
}	

void ACOsgBrowserViewQT::keyPressEvent( QKeyEvent* event )
{
	osg_view->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
	if ( event->text() ==  "z" ) {
		zoomdown = 1;
	}
	else if ( event->text() ==  "a" ) {
		media_cycle->setForwardDown(1);
		forwarddown = 1;
	}
	else if ( event->text() ==  "q" ) {
		media_cycle->setAutoPlay(1);
		autoplaydown = 1;
	}
	else if ( event->text() ==  "r" ) {
		rotationdown = 1;
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
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }

    osg_view->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);

	if ( (media_cycle) && (media_cycle->hasBrowser()) && (forwarddown==1) )
	{
		//media_cycle->setForwardDown(1);
		int loop = media_cycle->getClickedNode();
		//media_cycle->hoverCallback(event->x(),event->y());
		//int loop = media_cycle->getClosestNode();

		if(loop >= 0)
		{
			//media_cycle->incrementLoopNavigationLevels(loop);
			media_cycle->setSelectedNode(loop);
			std::cout << "Selected node: " << loop << std::endl;
			
			// XSCF 250310 added these 3
			media_cycle->pushNavigationState();
			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
			media_cycle->getBrowser()->setState(AC_CHANGING);
			
			media_cycle->updateNeighborhoods();
			media_cycle->updateClusters(false);// was true
			
			// remainders from updateClusters(true)
			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
			media_cycle->getBrowser()->setState(AC_CHANGING);
		}
	}
	
	mousedown = 1;
	refx = event->x();
	refy = event->y();
	media_cycle->getCameraPosition(refcamx, refcamy);
	refzoom = media_cycle->getCameraZoom();
	refrotation = media_cycle->getCameraRotation();
	media_cycle->setNeedsDisplay(1);
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
	if ( mousedown==1 ) {
		if ( zoomdown==1 ) {
			media_cycle->setCameraZoom(refzoom - (y-refy)/50);
			//media_cycle->setCameraZoom(refzoom + (y-refy) / abs (y-refy) * sqrt( pow((y-refy),2) + pow((x-refx),2) )/50 );
		}
		else if ( rotationdown==1 ) {
			float rotation = atan2(-(y-this->height()/2),x-this->width()/2)-atan2(-(refy-this->height()/2),refx-this->width()/2);
			media_cycle->setCameraRotation(refrotation + rotation);
		}	
		else {
			zoom = media_cycle->getCameraZoom();
			angle = media_cycle->getCameraRotation();
			xmove = (refx-x);
			ymove =-(refy-y);
			xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
			ymove2 = ymove*cos(-angle)+xmove*sin(-angle);		
			media_cycle->setCameraPosition(refcamx + xmove2/800/zoom , refcamy + ymove2/800/zoom);
		}
	}
	media_cycle->setNeedsDisplay(1);
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
	
	if( (media_cycle) && (media_cycle->hasBrowser()) && (forwarddown==0) )
	{
		media_cycle->setClickedNode(-1);
	}
	forwarddown = 0;
	mousedown = 0;
	media_cycle->setNeedsDisplay(1);
}

void ACOsgBrowserViewQT::prepareFromBrowser()
{
	setMouseTracking(false); //CF necessary for the hover callback
	renderer->prepareNodes(); 
	renderer->prepareLabels();
	this->setSceneData(renderer->getShapes()); //CF database_view->setSceneData for CompositeViewer
}


void ACOsgBrowserViewQT::updateTransformsFromBrowser( double frac)
{
	int closest_node;	
	// get screen coordinates
	closest_node = renderer->computeScreenCoordinates(this, frac); //CF database_view instead of this for the CompositeViewer
	media_cycle->setClosestNode(closest_node);
	// recompute scene graph	
	renderer->updateNodes(frac); // animation time in [0,1]
	renderer->updateLabels(frac);
}