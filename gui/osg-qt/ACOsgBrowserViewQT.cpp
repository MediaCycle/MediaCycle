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
#include <MediaCycleLight.h>
#include "ACOsgBrowserRenderer.h"

////extern MediaCycle *media_cycle;

/*
struct ACOsgBrowserViewData
{
	ACOsgBrowserRenderer renderer;
};
*/

ACOsgBrowserViewQT::ACOsgBrowserViewQT( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):
	#if USE_QT4
		QGLWidget(parent, shareWidget, f),
	#else
		QGLWidget(parent, name, shareWidget, f),
	#endif
	mousedown(0), zoomdown(0), forwarddown(0),
	refx(0.0f), refy(0.0f),
	refcamx(0.0f), refcamy(0.0f),
	refzoom(0.0f)
{
	_gw = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
	#if USE_QT4
			setFocusPolicy(Qt::ClickFocus);
	#else
			setFocusPolicy(QWidget::ClickFocus);
	#endif


	getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
	getCamera()->setProjectionMatrixAsPerspective(45.0f, getCamera()->getViewport()->aspectRatio(), 0.1f, 10.0f);

	getCamera()->getViewMatrix().makeIdentity();
	//getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
	getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.3), Vec3(0,0,0), Vec3(0,1,0));
	//getCamera()->setViewMatrixAsLookAt(Vec3(10,10,10), Vec3(0,0,0), Vec3(1,1,0));
	getCamera()->setGraphicsContext(getGraphicsWindow());
/*
	event_handler = new ACOsgBrowserEventHandler();
	addEventHandler(event_handler);
*/
	setThreadingModel(osgViewer::Viewer::SingleThreaded);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.start(10);

	_privateData = new ACOsgBrowserViewData();
	//renderer = new ACOsgBrowserRenderer();

}

void ACOsgBrowserViewQT::setMediaCycle(MediaCycle* _media_cycle)
{
	media_cycle = _media_cycle;
	_privateData->renderer.setMediaCycle(media_cycle);
	event_handler = new ACOsgBrowserEventHandler;
	event_handler->setMediaCycle(media_cycle);
	this->addEventHandler(event_handler);
}

void ACOsgBrowserViewQT::resizeGL( int width, int height )
{
  _gw->getEventQueue()->windowResize(0, 0, width, height );
  _gw->resized(0,0,width,height);
}

void ACOsgBrowserViewQT::keyPressEvent( QKeyEvent* event )
{
	#if USE_QT4
	  _gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
		if (  event->text() ==  "a")
			forwarddown = 1;
		if (  event->text() ==  "z")
			zoomdown = 1;
	#else
	  _gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) event->ascii() );
	#endif
}

void ACOsgBrowserViewQT::keyReleaseEvent( QKeyEvent* event )
{
	#if USE_QT4
		  _gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
	#else
		  _gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) event->ascii() );
	#endif

	zoomdown = 0;
	forwarddown = 0;
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
    _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);

	if (media_cycle && media_cycle->hasBrowser() && forwarddown==1)
	{
		int loop = media_cycle->getClickedLoop();
		//NSLog(@"click on %d when 'a' key pressed", loop);		
		
		if(loop >= 0)
		{
			media_cycle->incrementLoopNavigationLevels(loop);
			media_cycle->setSelectedObject(loop);
			media_cycle->updateClusters(true);
			// audio_cycle->saveNavigationState();
		}
	}
	
	//NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	mousedown = 1;
	refx = event->x();
	refy = event->y();
	media_cycle->getCameraPosition(refcamx, refcamy);
	refzoom = media_cycle->getCameraZoom();
}

void ACOsgBrowserViewQT::mouseDoubleClickEvent( QMouseEvent* event )
{
	if (media_cycle && media_cycle )
	{
		std::cout << "Double click" << std::endl;
		int loop = media_cycle->getClickedLoop();
		//NSLog(@"double click on %d", loop);		
		
		if(loop >= 0)
		{
			media_cycle->incrementLoopNavigationLevels(loop);
			media_cycle->setSelectedObject(loop);
			media_cycle->updateClusters(true);
			// audio_cycle->saveNavigationState();
		}
	}
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
		updateTransformsFromBrowser(1.0);
    _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);

	if(media_cycle && media_cycle)
	{
		media_cycle->setClickedLoop(-1);
	}
	
	mousedown = 0;
}

void ACOsgBrowserViewQT::mouseMoveEvent( QMouseEvent* event )
{
    _gw->getEventQueue()->mouseMotion(event->x(), event->y());

	float zoom;
	float x, y;
	//NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	x = event->x(); 
	y = event->y();
	if (mousedown) {
		if (zoomdown) {
			media_cycle->setCameraZoom(refzoom + (y-refy)/50);
		}
		else {
			zoom = media_cycle->getCameraZoom();
			media_cycle->setCameraPosition(refcamx + (refx-x)/width()/zoom , refcamy + (refy-y)/height()/zoom);
		}
	}

}

void ACOsgBrowserViewQT::prepareFromBrowser()
{
	_privateData->renderer.prepareNodes(); 
	_privateData->renderer.prepareLabels();
	setSceneData(_privateData->renderer.getShapes());
}


void ACOsgBrowserViewQT::updateTransformsFromBrowser( double frac)
{
	int closest_loop;
	std::cout << "osgViewer::Viewer*" << std::endl;
	
	osgViewer::Viewer* view = this;
	std::cout << "osgViewer::Viewer* done" << std::endl;	
	// get screen coordinates
	closest_loop = _privateData->renderer.computeScreenCoordinates(view, frac);
	media_cycle->setClosestLoop(closest_loop);
	// recompute scene graph	
	_privateData->renderer.updateNodes(frac); // animation time in [0,1]
	_privateData->renderer.updateLabels(frac);
}


/*EOF*/
