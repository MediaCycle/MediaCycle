/**
 * @brief ACOsgBrowserViewQT.h
 * @author Christian Frisson
 * @date 01/12/2009
 * @copyright (c) 2009 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#ifndef HEADER_ACOsgBrowserVIEWQT
#define HEADER_ACOsgBrowserVIEWQT

//
//  ACOsgBrowserViewQT.h
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

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#if USE_QT4
    #include <QtCore/QString>
    #include <QtCore/QTimer>
    #include <QtGui/QKeyEvent>
    #include <QtGui/QApplication>
    #include <QtOpenGL/QGLWidget>
    #include <QtGui/QMainWindow>
    using Qt::WindowFlags;
#else
    class QWidget;
    #include <qtimer.h>
    #include <qgl.h>
    #include <qapplication.h>
    #define WindowFlags WFlags
#endif

#include <iostream>
#include <ACOsgBrowserRenderer.h>
#include <ACOsgBrowserEventHandler.h>

//struct ACOsgBrowserViewData;
struct ACOsgBrowserViewData
{
	ACOsgBrowserRenderer renderer;
};

class ACOsgBrowserViewQT : public QGLWidget, public osgViewer::Viewer
{
	public:
        ACOsgBrowserViewQT( QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0 );
        virtual ~ACOsgBrowserViewQT() {free(_privateData);}
        osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
        const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }
        virtual void paintGL()
        {
			//updateTransformsFromBrowser(0.0);
            frame();
        }

    protected:
        void init();
        virtual void resizeGL( int width, int height );
        virtual void keyPressEvent( QKeyEvent* event );
        virtual void keyReleaseEvent( QKeyEvent* event );
        virtual void mousePressEvent( QMouseEvent* event );
        virtual void mouseReleaseEvent( QMouseEvent* event );
        virtual void mouseMoveEvent( QMouseEvent* event );
        virtual void mouseDoubleClickEvent( QMouseEvent* event );
        osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
        QTimer _timer;

	private:
		MediaCycle *media_cycle;
		ACOsgBrowserViewData *_privateData;
		//ACOsgBrowserRenderer *renderer;
		ACOsgBrowserEventHandler *event_handler;

	public:
		// needs to be called when loops are added or removed
		void prepareFromBrowser();
		// needs to be called when loops positions are changed
		void updateTransformsFromBrowser( double frac);
		void setMediaCycle(MediaCycle* _media_cycle);

	private:
		int mousedown, zoomdown, forwarddown;
		float refx, refy;
		float refcamx, refcamy;
		float refzoom;
		//bool _initialized; // CF cocoa rip
};
#endif