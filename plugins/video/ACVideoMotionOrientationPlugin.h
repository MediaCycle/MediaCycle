/*
 *  ACVideoMotionOrientationPlugin.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 15/07/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#ifndef _ACVIDEOMOTIONORIENTATIONPLUGIN_H
#define	_ACVIDEOMOTIONORIENTATIONPLUGIN_H
#include "ACVideoAnalysis.h"
#include "ACPlugin.h"
#include "ACMediaFeatures.h"
#include "ACMediaTimedFeature.h"

//XS TODO : make an ACVideoFeatures Plugin
// with (ACVideoData *) ACMediaData
class ACVideoMotionOrientationPlugin : public ACFeaturesPlugin {
public:
    ACVideoMotionOrientationPlugin();
    ~ACVideoMotionOrientationPlugin();
	virtual std::vector<ACMediaFeatures*> calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat=false);
//	std::string getSavedFileName(){return mtf_file_name;}
private:	
	ACVideoAnalysis* videoAn;
	void clean();
	std::string mtf_file_name; // file in which features have been saved

};

#endif //_ACVIDEOMOTIONORIENTATIONPLUGIN_H
