/*
 *  ACPDFFeaturesPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 28/05/2011
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
 */

#include "ACPDFFeaturesPlugin.h"
#include "ACPDF.h"

#include <vector>
#include <string>

ACPDFFeaturesPlugin::ACPDFFeaturesPlugin() {
	
    this->mMediaType = MEDIA_TYPE_PDF;
    this->mName = "PDF Features (Page Count)";
    this->mDescription = "PDF Features Plugin (Page Count)";
    this->mId = "";
}

ACPDFFeaturesPlugin::~ACPDFFeaturesPlugin() {
	
}

std::vector<ACMediaFeatures*> ACPDFFeaturesPlugin::calculate(ACMediaData* pdf_data, ACMedia* theMedia, bool _save_timed_feat) {
	std::vector<ACMediaFeatures*> desc;
	
	ACMediaFeatures* page_count;
	
	//ACPDF pdf_media = static_cast< ACPDF > (theMedia);

	vector<float> count;
	count.resize(1);
	count[0] = (float) ((ACPDF*) theMedia)->getPageCount();
	
	page_count = new ACMediaFeatures(count, "Page Count");  

	desc.push_back(page_count);	
		
	return desc;
}

