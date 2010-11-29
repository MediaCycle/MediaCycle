/*
 *  ACOsgAudioTrackControlsRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 17/11/10
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
 */

#include "ACOsgAudioTrackControlsRenderer.h"

#include <osgGA/GUIEventHandler>

ACOsgAudioTrackControlsRenderer::ACOsgAudioTrackControlsRenderer() {
	
	track_geode = 0;
	//zoom_x = 10.0; zoom_y = 6.0;
	zoom_x = 1.0; zoom_y = 1.0;
	translate_x = 0.0;
	displayed_media_index = -1;
	// Magic numbers!
	zpos = 0.01f;
	xstep = 0.0005f;
	yspan = 0.6f;
	xspan = 0.65f;
}

ACOsgAudioTrackControlsRenderer::~ACOsgAudioTrackControlsRenderer() {
	// track_node->removeChild(0,1);
	if 	(track_geode) {
		track_geode->unref();
		track_geode=0;
	}
}

void ACOsgAudioTrackControlsRenderer::trackGeode() {
	
	StateSet *state;
	
	track_geode = new Geode();
	
	TessellationHints *hints = new TessellationHints();
	hints->setDetailRatio(0.0);
	
	state = track_geode->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
	
	track_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(1.0f,0.0f,0.0f),xspan,yspan,0.0), hints)); //draws a square // Vintage AudioCycle
	//sprintf(name, "some audio element");
	//track_geode->setUserData(new ACRefId(media_index));
	//track_geode->setName(name);
	track_geode->ref();	
}

void ACOsgAudioTrackControlsRenderer::prepareControls() {
	
	track_geode = 0;
	
	trackGeode();
	track_node->addChild(track_geode);
	((ShapeDrawable*)track_geode->getDrawable(0))->setColor(Vec4(0.2,0.2,0.4,1.0));
	std::cout << "track_node_addChild(track_geode)" << std::endl;
	
}

void ACOsgAudioTrackControlsRenderer::updateControls(double ratio) {
	double xstep = 0.00025;
	/*	
	 ACPoint p,p2;//CF dummy
	 p.x = p.y = p.z = p2.x = p2.y = p2.z = 0.0f; // CF dummy
	 
	 double omr = 1.0-ratio;
	 
	 omr = 1;
	 ratio = 0;
	 
	 float zoom = 1.0f;
	 float angle = 0.0f;
	 
	 Matrix T;
	 Matrix Trotate;
	 Matrix curserT;
	 Matrix waveT;
	 
	 float x, y, z;
	 float localscale;
	 float maxdistance = 0.2;
	 float maxscale = 1.5;
	 float minscale = 0.33;				
	 // Apply "rotation" to compensate camera rotation
	 x = omr*p.x + ratio*p2.x;
	 y = omr*p.y + ratio*p2.y;
	 z = 0;
	 T.makeTranslate(Vec3(x, y, z));
	 localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
	 localscale = max(localscale,minscale);
	 
	 localscale = 1.0;//CF
	 */	
	
	//track_node->setMatrix(T);
}
