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
: media_cycle(0), audio_engine(0), track_index(-1),media_index(-1),media(0),media_from_lib(true),media_changed(false),
screen_width(0.0f),width(0.0f),height(0.0f),screen_width_changed(false),width_changed(false),height_changed(false)
{
	track_node = new MatrixTransform();
	displayed_media_index = -1;
	// Magic numbers!
	zpos = 0.01f;
	xstep = 0.0005f;
	yspan = 0.666f;
	xspan = 0.666f;
	selection_sensing_width = xspan/200;
	manual_selection = false;
	this->initSelection();
}

void ACOsgTrackRenderer::initSelection()
{
	playback_min_width = yspan/10;
	selection_begin_pos = -xspan/2.0f;// arbitrarily at 1/4 of the width from the left
	selection_end_pos = -xspan/2.0f + playback_min_width;// arbitrarily at 3/4 of the width from the left
	selection_center_pos = 0.0f;
	selection_begin_pos_changed = false;
	selection_end_pos_changed = false;
	selection_center_pos_changed = false;
}

void ACOsgTrackRenderer::updateMedia(ACMedia* _media)
{
	media = _media;
	media_index = -1;
	media_from_lib = false;
	media_changed = true;
	this->initSelection();
}

void ACOsgTrackRenderer::updateMedia(int _media_index)
{
	media = media_cycle->getLibrary()->getMedia(_media_index);
	media_index = _media_index;
	media_from_lib = true;
	media_changed = true;
	this->initSelection();
}

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

void ACOsgTrackRenderer::setSelectionBegin(float begin)
{
	/*if (begin > selection_end_pos - playback_min_width)//min section width -> magic number to refine
		begin = selection_end_pos - playback_min_width;
	if (begin < -xspan/2.0f)
		begin = -xspan/2.0f;*/
	
	this->selection_center_pos += (begin - this->selection_begin_pos)/2.0f;
	this->selection_begin_pos = begin;
	selection_begin_pos_changed=true;
	selection_center_pos_changed=true;
}

void ACOsgTrackRenderer::setSelectionEnd(float end)
{
	/*if (end < selection_begin_pos + playback_min_width)//min section width -> magic number to refine
		end = selection_begin_pos + playback_min_width;
	if (end > xspan/2.0f)
		end = xspan/2.0f;*/

	this->selection_center_pos += (end - this->selection_end_pos)/2.0f;
	this->selection_end_pos = end;
	selection_end_pos_changed=true;
	selection_center_pos_changed=true;
}

float ACOsgTrackRenderer::getSelectionBegin()
{
	return this->selection_begin_pos;
}

float ACOsgTrackRenderer::getSelectionEnd()
{
	return this->selection_end_pos;
}

void ACOsgTrackRenderer::setSelectionCenter(float center)
{
	selection_begin_pos += center - selection_center_pos;
	selection_end_pos += center - selection_center_pos; 
	selection_center_pos = center;
	selection_begin_pos_changed=true;
	selection_end_pos_changed=true;
	selection_center_pos_changed=true;
}

float ACOsgTrackRenderer::getSelectionCenter()
{
	return selection_center_pos;
}
/*
void ACOsgTrackRenderer::setSelectionZoneWidth(float _width)
{
	float previous_width = selection_end_pos - selection_begin_pos;
	selection_begin_pos += (_width - previous_width)/2.0f;
	selection_end_pos += (_width - previous_width)/2.0f;
	selection_begin_pos_changed=true;
	selection_end_pos_changed=true;
}

float ACOsgTrackRenderer::getSelectionZoneWidth()
{
	return selection_end_pos - selection_begin_pos;
}
*/