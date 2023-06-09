/*
 *  AC3DModel.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 06/09/10
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

#ifndef AC3DMODEL_H
#define AC3DMODEL_H

#include "ACMedia.h"
#include "ACMediaData.h"
#include "AC3DModelData.h"
#include <string>
#include <iostream>

// -----------------------------------

class AC3DModel: public ACMedia {

public:
	AC3DModel();
    AC3DModel(const AC3DModel& m);
    virtual ~AC3DModel();
	void saveACLSpecific(std::ofstream &library_file);
	int loadACLSpecific(std::ifstream &library_file);
	bool extractData(std::string fname);
private:	
	void init();
//private:
//	std::vector<float> center;
//	std::vector<float> extent;
};

#endif // AC3DMODEL_H
