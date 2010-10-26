/*
 *  ACOsgBrowserRenderer.cpp
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

#include "ACOsgVideoRenderer.h"

#if !defined (APPLE_IOS)

ACOsgVideoRenderer::ACOsgVideoRenderer() {
	
	image_stream = 0;
}

ACOsgVideoRenderer::~ACOsgVideoRenderer() {
	
}

void ACOsgVideoRenderer::prepareNodes() {
	
	float reference_time, time_multiplier;
	
	imageGeode(1, 2.0, 1.0);
	
	media_node->addChild(image_transform);
	
	image_stream = dynamic_cast<osg::ImageStream*>(image_image);
	image_stream->setLoopingMode(ImageStream::LOOPING);
	
	reference_time = image_stream->getReferenceTime();
	reference_time = 30.0;
	image_stream->setReferenceTime(reference_time);
	
	time_multiplier = image_stream->getTimeMultiplier();
	image_stream->setTimeMultiplier(time_multiplier);
 
}

void ACOsgVideoRenderer::updateNodes(double ratio) {
		
	ACOsgImageRenderer::updateNodes();

	const ACMediaNode &attribute = media_cycle->getMediaNode(node_index);
	
	if (attribute.getActivity()==1) {
		image_stream->play();
	}
	else {
		image_stream->pause();
	}
}
#endif//CF APPLE_IOS