/*
 *  ArchipelText.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 8/06/12
 *  @copyright (c) 2012 – UMONS - Numediart
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
#include "ArchipelText.h"
#include "ArchipelReader.h"

using namespace std;
ArchipelTextData::ArchipelTextData():ACTextData(){
}

ArchipelTextData::~ArchipelTextData(){
	if (text_ptr!=0){
		text_ptr->clear();
		delete text_ptr;
		text_ptr=0;
	}
}
ArchipelTextData::ArchipelTextData(std::string _fname):ACTextData(){
	file_name=_fname;
	this->readData(_fname);
}

bool ArchipelTextData::readData(std::string _fname){
	if (text_ptr != NULL) {
		delete text_ptr;
		text_ptr=0;
	}
	archipelReader reader(_fname);
	if (reader.isArchipel()){
		std::string desc;
		text_ptr = new std::string(reader.getText());
		if( text_ptr == NULL) {
			cerr << "<ACMediaData::readTextData> file can not be read !" << endl;
			return false;
		}
		
		if ((*text_ptr)==string(""))
			*text_ptr=string("null");
		else {
			cout << (*text_ptr)<<"\n";
		}
		label=reader.getArtist()+string("/")+reader.getAlbumName();
		cout << label<<"\n";
		//	cout << (*text_ptr)<<"\n";
		return true;
	}
	else {
			return false;
	}

}


ArchipelText::ArchipelText():ACText(){
}

bool ArchipelText::extractData(std::string fname){
	if (data){
		delete data;
		data=0;
	}
	// XS todo : store the default header (16 or 64 below) size somewhere...
	data = new ArchipelTextData(fname);
	if (data!=0){
		if (data->getData()==NULL){
			delete data;
			data=0;
		}
		else
			label=data->getLabel();
	}
	//computeThumbnail(image_data, 64, 64);
	//width = thumbnail_width;
	//height = thumbnail_height;
	return true;
}


#endif