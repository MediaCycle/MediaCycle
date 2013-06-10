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

ACPDFFeaturesPlugin::ACPDFFeaturesPlugin() : ACFeaturesPlugin() {
    this->mMediaType = MEDIA_TYPE_PDF;
    this->mName = "PDF Features (Page Count)";
    this->mDescription = "PDF Features Plugin (Page Count)";
    this->mId = "";
}

ACPDFFeaturesPlugin::~ACPDFFeaturesPlugin() {	
}

ACFeatureDimensions ACPDFFeaturesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Page Count"] = 1;
    return featureDimensions;
}

std::vector<ACMediaFeatures*> ACPDFFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
	std::vector<ACMediaFeatures*> desc;

    std::cout << "ACPDFFeaturesPlugin::calculate: author '" << (std::string) ((ACPDF*) theMedia)->getAuthor() << "'" << std::endl;
    std::cout << "ACPDFFeaturesPlugin::calculate: creator '" << (std::string) ((ACPDF*) theMedia)->getCreator() << "'" << std::endl;
    std::cout << "ACPDFFeaturesPlugin::calculate: subject '" << (std::string) ((ACPDF*) theMedia)->getSubject() << "'" << std::endl;
    std::cout << "ACPDFFeaturesPlugin::calculate: title '" << (std::string) ((ACPDF*) theMedia)->getTitle() << "'" << std::endl;
    std::cout << "ACPDFFeaturesPlugin::calculate: keywords '" << (std::string) ((ACPDF*) theMedia)->getKeywords() << "'" << std::endl;

    ACMediaFeatures* page_count;
	vector<float> count;
    count.push_back( (float) ((ACPDF*) theMedia)->getPageCount());
    page_count = new ACMediaFeatures(count, "Page Count");
	desc.push_back(page_count);	
	return desc;
}
