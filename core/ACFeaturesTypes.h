/*
 *  ACFeaturesTypes.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
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

// Definitions used along with features

// XS: well, this is not really useful if we have a name for each feature.
// XS TODO: just remove this file and all references to it.

#ifndef _ACFEATURESTYPES_H
#define _ACFEATURESTYPES_H

#include <vector>

typedef unsigned int DistanceType;
typedef unsigned int FeatureType; // to keep track of the feature's ID

typedef std::vector<float> FeaturesVector;
// other option : make FeaturesVector a class

static const FeatureType FT_BASE=100;
static const FeatureType FT_SHAPE=101;
static const FeatureType FT_COLOR=102;
static const FeatureType FT_TEXTURE=103;

static const FeatureType FT_SPEED=201; // for video

static const FeatureType FT_TIMBRE=301;

#endif  // ACFEATURESTYPES_H
