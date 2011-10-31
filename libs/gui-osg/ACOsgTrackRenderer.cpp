/*
 *  ACOsgTrackRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 28/04/10
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

#include "ACOsgTrackRenderer.h"
using namespace osg;

ACOsgTrackRenderer::ACOsgTrackRenderer()
: media_cycle(0), track_index(-1),media_index(-1),media(0),media_from_lib(true),media_changed(false),
screen_width(0.0f),width(0.0f),height(0.0f),screen_width_changed(false),width_changed(false),height_changed(false)
{
	#ifdef SUPPORT_AUDIO
		audio_engine = 0;
	#endif//def SUPPORT_AUDIO
	track_node = new MatrixTransform();
	displayed_media_index = -1;
	// Magic numbers!
	zpos = 0.0f;//0.01f;
	xstep = 0.0005f;
	yspan = 0.666f;
	xspan = 0.666f;
	selection_sensing_width = xspan/200;
	manual_selection = false;
	this->initSelection();
}

void ACOsgTrackRenderer::initSelection()
{
	playback_min_width = xspan/100;
	selection_center_pos = -xspan/2.0f;
	selection_begin_pos = selection_center_pos - playback_min_width;
	selection_end_pos = selection_center_pos + playback_min_width;
	selection_begin_pos_changed = true;
	selection_end_pos_changed = true;
	selection_center_pos_changed = true;
}

void ACOsgTrackRenderer::updateMedia(ACMedia* _media)
{
	media = _media;
	media_index = -1;
	media_from_lib = false;
	media_changed = true;
	this->initSelection();
}

/*void ACOsgTrackRenderer::updateMedia(int _media_index)
{
	media = media_cycle->getLibrary()->getMedia(_media_index);
	media_index = _media_index;
	media_from_lib = true;
	media_changed = true;
	this->initSelection();
}*/

void ACOsgTrackRenderer::clearMedia()
{
	media = 0;
	media_index = -1;
	media_from_lib = true;
	media_changed = true;
	this->initSelection();
}

void ACOsgTrackRenderer::updateScreenWidth(int _screen_width)
{
	if ( screen_width != _screen_width){
		this->screen_width = _screen_width;
		screen_width_changed = true;
		this->updateTracks();
		screen_width_changed = false;
	}	
}

void ACOsgTrackRenderer::updateSize(int _width,float _height)
{
	if (( height != _height) || ( width != _width)){
		this->height = _height;
		this->width = _width;
		height_changed = true;
		width_changed = true;
		this->updateTracks();
		height_changed = false;
		width_changed = false;
	}	
}

void ACOsgTrackRenderer::resizeSelectionFromBegin(float _begin)
{
	if(_begin > this->selection_center_pos - playback_min_width/2.0f)
		return;
	
	float _extent = _begin - selection_begin_pos;
	this->selection_begin_pos = _begin;
	this->selection_end_pos -= _extent;
	selection_begin_pos_changed=true;
	selection_end_pos_changed=true;
}

void ACOsgTrackRenderer::resizeSelectionFromEnd(float _end)
{
	if(_end < this->selection_center_pos + playback_min_width/2.0f)
		return;

	float _extent = _end - selection_end_pos;
	this->selection_end_pos = _end;
	this->selection_begin_pos -= _extent;
	selection_begin_pos_changed=true;
	selection_end_pos_changed=true;
}

void ACOsgTrackRenderer::moveSelection(float _center)
{
	//float length = selection_end_pos - selection_begin_pos;
	float _extent = _center - selection_center_pos;
	this->selection_begin_pos += _extent;
	this->selection_center_pos += _extent;
	this->selection_end_pos += _extent;
	selection_begin_pos_changed=true;
	selection_end_pos_changed=true;
	selection_center_pos_changed=true;
}

void ACOsgTrackRenderer::createDummySegments()
{
	if(media){
		if (media->getNumberOfSegments()==0){
			//std::cout << "Dummy segments" << std::endl;
			for (unsigned int s=0;s<4;s++){
				ACMedia* seg = ACMediaFactory::getInstance().create(media);
				seg->setParentId(media->getId());
				media->addSegment(seg);//dummy
			}	
			float media_start = media->getStart();
			float media_end = media->getEnd();
			media->getSegment(0)->setStart(media_start);
			media->getSegment(0)->setEnd((media_end-media_start)/4.0f);
			media->getSegment(1)->setStart((media_end-media_start)/4.0f);
			media->getSegment(1)->setEnd(3*(media_end-media_start)/8.0f);
			media->getSegment(2)->setStart(3*(media_end-media_start)/8.0f);
			media->getSegment(2)->setEnd((media_end-media_start)/2.0f);
			media->getSegment(3)->setStart((media_end-media_start)/2.0f);
			media->getSegment(3)->setEnd(media_end);
		}
		//else
		//	std::cout << media->getNumberOfSegments() << " segments" << std::endl;
	}	
}