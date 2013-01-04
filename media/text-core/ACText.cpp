/*
 *  ACText.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 22/10/10
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

#include "ACText.h"
using namespace std;

ACText::ACText() : ACMedia() {
    this->init();
}

ACText::ACText(const ACText& m) : ACMedia(m) {
    this->init();
}

void ACText::init(){
    media_type = MEDIA_TYPE_TEXT;
    docIndex=-1;
}

ACText::~ACText(){
}

void ACText::saveACLSpecific(ofstream &library_file) {
	
	library_file << endl;
}

int ACText::loadACLSpecific(ifstream &library_file) {
	return 1;
}

bool ACText::extractData(string fname){
    ACTextData* text_data = dynamic_cast<ACTextData*>(this->getMediaData());
    if(!text_data)
    {
        std::cerr << "ACText::extractData: no text data set" << std::endl;
        return 0;
    }

    if (text_data!=0){
        if (text_data->getData()==NULL){
            delete text_data;
            text_data=0;
		}
		else
            label=text_data->getLabel();
	}
	//computeThumbnail(image_data, 64, 64);
	//width = thumbnail_width;
	//height = thumbnail_height;
	return true;
}

/*std::string ACText::getTextMetaData(){
    string ret;
    if (data==0){
        this->extractData(filename);
        if (data!=0)
            ret=(*((string*)data->getData()));
        this->deleteData();
    }
    else{
        ret=(*((string*)data->getData()));
    }
    return ret;
    
}*/

