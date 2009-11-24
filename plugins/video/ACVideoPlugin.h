/*
 *  ACVideoPlugin.h
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

#ifndef _ACVIDEOPLUGIN_H
#define	_ACVIDEOPLUGIN_H

#include "ACVideoAnalysis.h"
#include "ACPlugin.h"
#include "ACMediaFeatures.h"

#include<iostream>

class ACVideoPlugin : public ACPlugin {
public:
    ACVideoPlugin();
    ~ACVideoPlugin();
    virtual int initialize();
	std::vector<ACMediaFeatures*>  calculate();
	std::vector<ACMediaFeatures*>  calculate(std::string aFileName);
	std::vector<ACMediaFeatures*>  calculateTop(std::string aFileName);
	std::vector<ACMediaFeatures*>  calculateFront(std::string aFileName);
	std::string changeLastFolder(std::string path, std::string folder);
	std::string extractDirectory(std::string path);
	std::string extractFilename(std::string path);
	virtual int start() {return 0;};
	virtual int stop() {return 0;};
	float getWidth();
	float getHeight();
	float getDuration();
	
private:
	ACMediaFeatures* calculateMeanOfTrajectory(ACVideoAnalysis* video);
	ACMediaFeatures* calculateStdOfTrajectory(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMaxOfTrajectory(ACVideoAnalysis* video);
	ACMediaFeatures* calculateContractionIndex(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMeanSpeedOfTrajectory(ACVideoAnalysis* video);
//	ACMediaFeatures* calculateMostOccupiedCell(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMeanBoundingBoxRatio(ACVideoAnalysis* video);
	ACMediaFeatures* calculateMeanPixelSpeed(ACVideoAnalysis* video);
};

#endif	/* _ACVIDEOPLUGIN_H */
