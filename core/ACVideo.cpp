/*
 *  ACVideo.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 25/05/09
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

#include "ACVideo.h"
#include <string>

using namespace std;

//#include "ACAnalysedVideo.h"
//#include "ACVideoFeaturesFactory.h"

ACVideo::ACVideo() : ACMedia() {
	_type = VIDEO;
}	

ACVideo::~ACVideo() {
}

void ACVideo::import(string _path) { 
	cout << "importing..." << _path << endl;
	filename=_path;
/*	ACAnalysedVideo* full_video = new ACAnalysedVideo(_path);
	ACVideoFeaturesFactory* factory = new ACVideoFeaturesFactory(full_video);
	features.push_back(factory->calculateFeature("Speed")); // XS TODO fichier

	delete factory;
	delete full_video;
*/
}


// TODO : complete this (Load/Save)