/*
 *  ACVideoDancersPlugin.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 22/09/09
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

// XS: this was named ACVideoPlugin before
// on 17/01/2011 it was changed to ACVideoDancersPlugin 
// because it is very specific to the Dancers! project
// it requires the videos to be in two folders (Front, Top) corresponding to shots from the 2 cameras

#ifndef _ACVIDEODANCERSPLUGIN_H
#define	_ACVIDEODANCERSPLUGIN_H

#include "ACVideoAnalysis.h"
#include "ACPlugin.h"
#include "ACMediaFeatures.h"
#include "ACMediaTimedFeature.h"

#include<iostream>

class ACVideoDancersPlugin : public ACFeaturesPlugin {
public:
    ACVideoDancersPlugin();
    ~ACVideoDancersPlugin();
    std::vector<ACMediaFeatures*> calculate(ACMedia*, bool _save_timed_feat=false);
    virtual ACFeatureDimensions getFeaturesDimensions();
	
private:
	std::vector<ACMediaFeatures*> _calculate(std::string aFileName, bool _save_timed_feat=false);
	std::vector<ACMediaFeatures*>  calculateTop(std::string aFileName);
	std::vector<ACMediaFeatures*>  calculateFront(std::string aFileName);
	std::string changeLastFolder(std::string path, std::string folder);
	std::string extractDirectory(std::string path);
	std::string extractFilename(std::string path);
	
	ACMediaFeatures* calculateMeanOfTrajectory(ACVideoAnalysis* video);
	ACMediaFeatures* calculateStdOfTrajectory(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMaxOfTrajectory(ACVideoAnalysis* video);
	ACMediaFeatures* calculateContractionIndex(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMeanSpeedOfTrajectory(ACVideoAnalysis* video);
//	ACMediaFeatures* calculateMostOccupiedCell(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMeanBoundingBoxRatio(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMeanBlobPixelsSpeed(ACVideoAnalysis* video);
};

#endif	/* _ACVIDEODANCERSPLUGIN_H */
