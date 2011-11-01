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

#if defined (SUPPORT_VIDEO)
#include "ACOsgVideoRenderer.h"
#include "ACVideo.h"

using namespace osg;

ACOsgVideoRenderer::ACOsgVideoRenderer() {
	media_type = MEDIA_TYPE_VIDEO;
	image_stream = 0;
}

ACOsgVideoRenderer::~ACOsgVideoRenderer() {
	if (image_stream) image_stream->quit();
}

void ACOsgVideoRenderer::prepareNodes() {
	
//	float reference_time, time_multiplier;
	
	if (!image_geode) {
		imageGeode(true, 2.0, 1.0); // true is for flip -- necessary for video
		media_node->addChild(image_transform);
	}
	
	//image_stream = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getStream();
	image_stream = ((ACVideo*)(media))->getStream();
	
	//std::cout << "Movie length " << image_stream->getLength() << " and framerate "<< media->getFrameRate() << std::endl;//image_stream->getFrameRate() << std::endl;
	
	// Hack to display a first valid frame,
	image_stream->play();
}

void ACOsgVideoRenderer::updateNodes(double ratio) {
		
	ACOsgImageRenderer::updateNodes();

	const ACMediaNode &attribute = media_cycle->getMediaNode(node_index);
	osg::ImageStream::StreamStatus streamStatus = image_stream->getStatus();
		
	switch (streamStatus) {
		case osg::ImageStream::INVALID:
			std::cout << "Image stream invalid status" << std::endl;
			break;
		case osg::ImageStream::PLAYING:
			if (attribute.getActivity()==0)
				image_stream->pause();
			break;
		case osg::ImageStream::PAUSED:
			if (attribute.getActivity()==1)
				image_stream->play();
			break;
		case osg::ImageStream::REWINDING:
			std::cout << "Image stream rewinding" << std::endl;
			break;
		default:
			break;
	}
}
#endif //defined (SUPPORT_VIDEO)