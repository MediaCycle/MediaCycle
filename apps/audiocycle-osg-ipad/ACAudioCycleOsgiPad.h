/*
 *  ACAudioCycleOsgiPad.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 26/10/10
 *
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 *  Adapted from Thomas Hogarth's 2009 examples from:
 *  http://github.com/stmh/osg/tree/iphone
 */

#include "osgPlugins.h"

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>

#include <MediaCycle.h>
#include <ACOsgBrowserRenderer.h>
#include <ACOsgBrowserEventHandler.h>
#include <ACAudioEngine.h>

#import <UIKit/UIKit.h>


@interface ACAudioCycleOsgiPad : NSObject <UIApplicationDelegate, UIAccelerometerDelegate> {

	UIAccelerationValue		accel[3];
	
	osg::ref_ptr<osgViewer::Viewer> _viewer;
	osg::ref_ptr<osg::MatrixTransform> _root;
	
	MediaCycle				*media_cycle;
	ACAudioEngine			*audio_engine;
	ACOsgBrowserEventHandler *event_handler;
	ACOsgBrowserRenderer *renderer;
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> osg_view;
}

- (void)updateScene;
- (void)prepareFromBrowser;
- (void)updateTransformsFromBrowser:(double)frac;
- (void)updatedLibrary;

@end

