/*
 *  ACOsgTrackControlsRenderer.cpp
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

#include "ACOsgTrackControlsRenderer.h"

ACOsgTrackControlsRenderer::ACOsgTrackControlsRenderer()
: track_index(-1),media_index(-1),media_from_lib(true),media_changed(false)
{
	track_node = new MatrixTransform();
}

void ACOsgTrackControlsRenderer::updateMedia(ACMedia* _media)
{
	media = _media;
	media_index = -1;
	media_from_lib = false;
	media_changed = true;
}

void ACOsgTrackControlsRenderer::updateMedia(int _media_index)
{
	media = media_cycle->getLibrary()->getMedia(_media_index);
	media_index = _media_index;
	media_from_lib = true;
	media_changed = true;
}

void ACOsgTrackControlsRenderer::clearMedia()
{
	media = NULL;
	media_index = -1;
	media_from_lib = true;
	media_changed = true;
}