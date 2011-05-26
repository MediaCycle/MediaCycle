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

#if defined (SUPPORT_TEXT)
#include "ACText.h"
using namespace std;


ACText::ACText() : ACMedia() {
    media_type = MEDIA_TYPE_TEXT;
	docIndex=-1;
	data =NULL;
}

ACText::~ACText(){
	if (data!=NULL)
		delete data;
}

void ACText::saveACLSpecific(ofstream &library_file) {
	
	library_file << endl;
}

int ACText::loadACLSpecific(ifstream &library_file) {
	

	
	return 1;
}

void ACText::extractData(string fname){
	if (data)
		delete data;
	// XS todo : store the default header (16 or 64 below) size somewhere...
	 data = new ACTextData(fname);
	//computeThumbnail(image_data, 64, 64);
	//width = thumbnail_width;
	//height = thumbnail_height;
	return;
}

void* ACText::getThumbnailPtr(){
	return NULL;

}
#endif //defined (SUPPORT_TEXT)