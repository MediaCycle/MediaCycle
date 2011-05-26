/*
 *  ACCosKMeansPlugin.h
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 20/10/10
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


#ifndef _ACCosKMeansPlugin_H
#define	_ACCosKMeansPlugin_H

#include "MediaCycle.h"
#include "ACMediaBrowser.h"
#include "ACKMeansPlugin.h"

#include<iostream>

class ACCosKMeansPlugin : public ACKMeansPlugin {
public:
	ACCosKMeansPlugin();
	~ACCosKMeansPlugin();
	double compute_distance(vector<ACMediaFeatures*> &, vector<ACMediaFeatures*> &, const vector<float> &, bool );
	double compute_distance(vector<ACMediaFeatures*> &, const vector<vector <float> > &, const vector<float> &, bool );
	
	
	
private:
};

#endif	/* _ACCosKMeansPlugin_H */
