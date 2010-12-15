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

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    
    gettimeofday(&tv, &tz);
    
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}


ACOsgVideoRenderer::ACOsgVideoRenderer() {
	
	image_stream = 0;
	initializing = false;
	st = 0.0f;
}

ACOsgVideoRenderer::~ACOsgVideoRenderer() {
	osg::ImageStream::StreamStatus streamStatus = image_stream->getStatus();
	switch (streamStatus) {
		case osg::ImageStream::INVALID:
			//std::cout << "Image stream invalid status" << std::endl;
			break;
		case osg::ImageStream::PLAYING:
			image_stream->pause();
			break;
		case osg::ImageStream::PAUSED:
			break;
		case osg::ImageStream::REWINDING:
			//std::cout << "Image stream rewinding" << std::endl;
			break;
		default:
			break;
	}
}

void ACOsgVideoRenderer::prepareNodes() {
	
	float reference_time, time_multiplier;
	
	imageGeode(1, 2.0, 1.0); // 1 is for flip -- necessary for video
	
	media_node->addChild(image_transform);
	image_stream = dynamic_cast<osg::ImageStream*>(image_image);
	image_stream->setLoopingMode(ImageStream::LOOPING);
	
	std::cout << "Image valid? " << image_image->valid() << " and stream valid? " << image_stream->valid() << std::endl;
	
	//CF hack to make the first frame appear, the test might not be accurate
	st = getTime();
	// version 1, done while preparing nodes, maybe more blocking for the GUI
	initializing = false;
	while (image_image->isImageTranslucent())
		image_stream->play();
	image_stream->pause();
	image_stream->rewind();
	double en = getTime();
	std::cout << "Refresh time: " << en-st << std::endl;
	// version 2, done thru updating nodes, maybe less precise
	/*initializing = true;
	image_stream->play();*/
	//hack end	

	/*std::cout << "Reference time: "<<image_stream->getReferenceTime()<<std::endl;
	std::cout << "Time multiplier: "<<image_stream->getTimeMultiplier()<<std::endl;
	std::cout << "Image stream length " << image_stream->getLength() << std::endl;*/
}

void ACOsgVideoRenderer::updateNodes(double ratio) {
		
	ACOsgImageRenderer::updateNodes();

	if (initializing){
		if (!(image_image->isImageTranslucent())){
			image_stream->pause();
			image_stream->rewind();
			double en = getTime();
			std::cout << "Refresh time: " << en-st << std::endl;
			initializing = false;
		}
	}	
	else{
		const ACMediaNode &attribute = media_cycle->getMediaNode(node_index);
		
		//std::cout << "Image translucent? " << image_image->isImageTranslucent() << std::endl;
		
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
}
#endif//CF APPLE_IOS