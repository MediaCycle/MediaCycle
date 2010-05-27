/*
 *  ACOsgBrowserRenderer.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
 *  @copyright (c) 2009 – UMONS - Numediart
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

#ifndef __ACOSG_IMAGE_RENDERER_H__
#define __ACOSG_IMAGE_RENDERER_H__

#include "ACOsgMediaRenderer.h"
#include "ACOpenCVInclude.h"

using namespace std;
using namespace osg;

//osg::Image* Convert_OpenCV_TO_OSG_IMAGE(IplImage* cvImg);

class ACOsgImageRenderer : public ACOsgMediaRenderer  {
protected:
	
	int prev_media_index;
	static const int NCOLORS ;
	Vec4Array* colors;
	Vec4Array* colors2;
	Vec4Array* colors3;
	
	Image *image_image;
	Geode* image_geode;
	Geode* border_geode;
	MatrixTransform* image_transform;
	
	void imageGeode(int flip=0, float sizemul=1.0, float zoomin=1.0);
	
public:
	ACOsgImageRenderer();
	~ACOsgImageRenderer();	
	void prepareNodes();
	void updateNodes(double ratio=0.0);
};

#endif
