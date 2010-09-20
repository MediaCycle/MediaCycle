/*
 *  ACOsg3DModelRenderer.h
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

#ifndef __ACOSG_3DMODEL_RENDERER_H__
#define __ACOSG_3DMODEL_RENDERER_H__

#include "ACOsgMediaRenderer.h"

#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>

using namespace std;
using namespace osg;

class ACOsg3DModelRenderer : public ACOsgMediaRenderer  {
protected:
		
	Vec4Array* colors_off;
	Vec4Array* colors_on;
	double modelangle;
	vector<float> media_cycle_center;
	vector<float> media_cycle_extent;
	
	Node* model_node;
	Geode* border_geode;
	MatrixTransform* acti_transform;
	MatrixTransform* norm_transform;
	
	void modelGeode();
	void borderGeode();
	void normTransform();
	
public:
	
	ACOsg3DModelRenderer();
	~ACOsg3DModelRenderer();
	
	void prepareNodes();
	void updateNodes(double ratio=0.0);
};

#endif
