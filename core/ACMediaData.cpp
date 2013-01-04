/*
 *  ACMediaData.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/12/09
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

// This class provides a container for the data belonging to each media
#include "ACMediaData.h"
#include <iostream>

ACMediaData::ACMediaData(){
    media_type = MEDIA_TYPE_NONE;
    file_name = "";
    label = "";
    data = 0;
}

ACMediaData::~ACMediaData()
{
    if(data)
        delete data;
    data = 0;
}

bool ACMediaData::isTemporal(){
    return (bool)(dynamic_cast<ACTemporalData*>(this));
}

bool ACMediaData::isSpatial(){
    return (bool)(dynamic_cast<ACSpatialData*>(this));
}
