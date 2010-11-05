/*
 *  ACOsg3DModelRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 06/09/10
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

#include "ACOsg3DModelRenderer.h"

ACOsg3DModelRenderer::ACOsg3DModelRenderer() {
	
	modelangle = 0;
	
	Vec4 color_off(0.4f, 0.4f, 0.4f, 1.0f);	
	Vec4 color_on(0.2f, 0.8f, 0.2f, 1.0f);	
	
	colors_off = new Vec4Array;
	colors_on = new Vec4Array;
	
	colors_off->push_back(color_off);	
	colors_on->push_back(color_on);		
	
	colors_on->ref();
	colors_off->ref();
	
	model_node = 0; border_geode = 0; acti_transform = 0; norm_transform = 0;
	
	media_index = -1;
	prev_media_index = -1;
}

ACOsg3DModelRenderer::~ACOsg3DModelRenderer() {
	
	if (model_node) { model_node->unref(); model_node=0; }
	if (border_geode) { border_geode->unref(); border_geode=0; }
	if (acti_transform) { acti_transform->unref(); acti_transform = 0; }
	if (norm_transform) { norm_transform->unref(); norm_transform = 0; }
	
	colors_on->unref();
	colors_off->unref();
}

void ACOsg3DModelRenderer::modelGeode() {
	
	if (model_node) { model_node->unref(); model_node=0; }

	model_node = osgDB::readNodeFile(media_cycle_filename);		
	osg::StateSet* ss = model_node->getOrCreateStateSet();
	ss->setMode( GL_BLEND, osg::StateAttribute::ON );
	ss->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	
	model_node->ref();	
	
	osg::ComputeBoundsVisitor cbv;
	model_node->accept( cbv );
	const osg::BoundingBox bb( cbv.getBoundingBox() );
	osg::Vec3 ext( bb._max - bb._min );
	
	media_cycle_center.resize(3);
	media_cycle_extent.resize(3);
	
	media_cycle_center[0] = bb.center().x(); media_cycle_center[1] = bb.center().y(); media_cycle_center[2] = bb.center().z();
	media_cycle_extent[0] = ext.x(); media_cycle_extent[1] = ext.y(); media_cycle_extent[2] = ext.z();	
}

void ACOsg3DModelRenderer::borderGeode() {
	
	if (border_geode) { border_geode->unref(); border_geode=0; }
	
	Box* box = new Box(Vec3(media_cycle_center[0], media_cycle_center[1], media_cycle_center[2]),
					   media_cycle_extent[0], media_cycle_extent[1], media_cycle_extent[2]);	
	ShapeDrawable* border_drawable = new ShapeDrawable(box);
	//((Geometry*)(border_drawable))->setColorArray(colors_off);
	//((Geometry*)(border_drawable))->setColorBinding(Geometry::BIND_OVERALL);
		
	border_geode = new Geode();
	
	osg::StateSet* ss = border_geode->getOrCreateStateSet();
	ss->setAttributeAndModes( new osg::PolygonMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ) );
	ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	
	border_geode->addDrawable(border_drawable);	
	border_geode->ref();
}

void ACOsg3DModelRenderer::normTransform() {

	if (!norm_transform) {
		norm_transform = new MatrixTransform();
		norm_transform->ref();
	}
	if (norm_transform->getNumChildren() == 2) {
		norm_transform->removeChild(0, 2);
	}
	else if (norm_transform->getNumChildren() == 1) {
		norm_transform->removeChild(0, 1);
	}	
	norm_transform->addChild(border_geode);
	norm_transform->addChild(model_node);

	double xlim = 0.025, ylim = xlim, zlim = xlim;
	double modelvol;	
	float scale;
	modelvol = xlim * ylim * zlim;
	scale =  pow((float)modelvol/(float)(media_cycle_extent[0]*media_cycle_extent[1]*media_cycle_extent[2]), (float)1.0/(float)3.0);
	
	// translation and scaling in order to normalize according to model dimensions
	// as well as rotation to put them in x-y plane
	Matrix normT;
	normT = Matrix::translate(Vec3(-media_cycle_center[0], -media_cycle_center[1], -media_cycle_center[2]))
			* Matrix::scale(Vec3(scale, scale, scale))
			* Matrix::rotate(-PI/2.0, Vec3(1.0,0.0,0.0));
    norm_transform->setMatrix(normT);		
}

void ACOsg3DModelRenderer::prepareNodes() {
		
	if  (media_cycle_isdisplayed) {
		if (!acti_transform) {
			modelGeode();
			borderGeode();
			normTransform();
			border_geode->setUserData(new ACRefId(node_index));
			
			if (acti_transform) { acti_transform->unref(); acti_transform = 0; }
			
			acti_transform = new MatrixTransform();
			acti_transform->ref();
			//acti_transform->setUserData(new ACRefId(node_index));	// SD TODO - media index?
			acti_transform->addChild(norm_transform);
			media_node->addChild(acti_transform);
			
			prev_media_index = media_index;
		}
	}
}

void ACOsg3DModelRenderer::updateNodes(double ratio) {
		
	if ( media_index!=prev_media_index ) {
		modelGeode();
		borderGeode();
		normTransform();
		border_geode->setUserData(new ACRefId(node_index));
		
		if (acti_transform) { acti_transform->unref(); acti_transform = 0; }
		
		// SD TODO - need to be removed, noramlly done in prepareNodes
		acti_transform = new MatrixTransform();
		acti_transform->ref();
		//acti_transform->setUserData(new ACRefId(node_index));	// SD TODO - media index?
		acti_transform->addChild(norm_transform);
		media_node->addChild(acti_transform);
		
		prev_media_index = media_index;
	}
	
	if( media_cycle_navigation_level >=  media_cycle_global_navigation_level ) {
		//acti_transform->setNodeMask(-1);
	}
	else {
		//acti_transform->setNodeMask(0);
	}
	
	double anglestep = 1.0/4.0; // rotations per seconds
	if ( media_cycle_activity ) {
		//printf("DELTA: %f\n", media_cycle_deltatime);
		media_cycle->setNeedsDisplay3D(true);
		if (border_geode->getDrawable(0)) {
			//((Geometry*)border_geode->getDrawable(0))->setColorArray(colors_on);
		}			
		modelangle += anglestep * 2 * PI * media_cycle_deltatime;
		//modelangle = fmod(modelangle, (double)(2*PI));
	}
	else {
		if (border_geode->getDrawable(0)) {
			//((Geometry*)border_geode->getDrawable(0))->setColorArray(colors_off);
		}
	}
			
	float x, y, z;
	// float zpos = 0.001;
	float localscale;
	float maxdistance = 0.2;
	float maxscale = 6;
	float minscale = 1.2;	
	
	// SD 2010 OCT - This animation has moved from Browser to Renderer
	/*
	double omr = 1.0-ratio;
	x = omr*media_cycle_current_pos.x + ratio*media_cycle_next_pos.x;
	y = omr*media_cycle_current_pos.y + ratio*media_cycle_next_pos.y;
	z = 0;
	 */
	x = media_cycle_view_pos.x;
	y = media_cycle_view_pos.y;
	z = 0;
	
	localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
	localscale = max(localscale, minscale);
	//localscale = maxscale;
	/*
	 if (localscale>minscale) {
		z += 2*zpos;
	}
	else if (media_cycle_activity) {
		z += zpos;
	}
	*/
	
	// rotation due to model activity when model active
	Matrix actiT;
	actiT.makeRotate(modelangle, Vec3( 0.0, 1.0, 0.0 ));
	acti_transform->setMatrix(actiT);
	
	// rotation and scale to compensate camera rotation and to implement mouse motion
	Matrix globalT;
	globalT = Matrix::rotate(-media_cycle_angle, Vec3(0.0,0.0,1.0))
				* Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom)
				* Matrix::translate(Vec3(x, y, z));
	media_node->setMatrix(globalT);
}
