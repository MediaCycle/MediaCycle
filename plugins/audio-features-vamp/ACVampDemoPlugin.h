/*
 *  ACVampDemoPlugin.h
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 08/06/10
 *  Updated by Christian Frisson on 13/08/2012.
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

#ifndef _ACVAMPDEMOPLUGIN_H
#define	_ACVAMPDEMOPLUGIN_H

#include "vamp-plugin-interface.h"
#include "MediaCycle.h"

#include<iostream>

class ACVampDemoPlugin : public ACTimedFeaturesPlugin {
public:
	ACVampDemoPlugin();
	~ACVampDemoPlugin();
	
    virtual std::vector<ACMediaFeatures*> calculate(){return std::vector<ACMediaFeatures*>();}
    virtual std::vector<ACMediaFeatures*> calculate(ACMedia*, bool _save_timed_feat=false);
    virtual ACFeatureDimensions getFeaturesDimensions(){return ACFeatureDimensions();}
};

#endif	/* _ACVAMPDEMOPLUGIN_H */
