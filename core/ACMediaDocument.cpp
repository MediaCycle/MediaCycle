/*
 *  ACMediaDocument.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 27/05/11
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

#if defined (SUPPORT_MULTIMEDIA) 

#include "ACMediaDocument.h"
#include <fstream>
#include <osg/ImageUtils>

using namespace std;

// ----------- class constants
const int ACMediaDocument:: default_thumbnail_area = 16384; // 128*128

//------------------------------------------------------------------

ACMediaDocument::ACMediaDocument() : ACMedia() {
	this->init();
}

void ACMediaDocument::init() {
	media_type = MEDIA_TYPE_MIXED;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;
	data=0;
}	

// copy-constructor, used for example to generate fragments
ACMediaDocument::ACMediaDocument(const ACMediaDocument& m, bool reduce) : ACMedia(m) {
	this->init();
	// ... XS TODO
}

ACMediaDocument::~ACMediaDocument() {
	this->deleteData();
	// the osg pointers should be automatically deleted (ref_ptr)
}

// used when we load from XML file
// so there is no need to compute a pointer to the data passed to the plugin
// in this case w and h can be read from XML file
bool ACMediaDocument::computeThumbnail(string _fname, int w, int h){

	return ok;
}

// used in extractData (when features are computed on-the-fly, not read from XML)
// in this case w=0 and h=0 (generally)
bool ACMediaDocument::computeThumbnail(ACMediaDocumentData* data_ptr, int w, int h){
	return ok;
}

bool ACMediaDocument::computeThumbnail(IplImage* img, int w, int h){
	bool ok = true;
	return ok;
}

bool ACMediaDocument::computeThumbnailSize(int w_, int h_){
	return ok;
}	

void ACMediaDocument::extractData(string fname){
	computeThumbnail(data, thumbnail_width , thumbnail_height);
}

void ACMediaDocument::setData(IplImage* _data){
}

void ACMediaDocument::deleteData(){
	if (data)
		delete data;
	data=0;
}

void ACMediaDocument::deleteMedia(){
	map<ACMediaType ,ACMedia*>::iterator iter
	for (iter=mediaContainer.begin(); iter!=mediaContainer;i++){
		if (iter->second!=0){
			deleter iter->second;
			iter->second=0;
		}			
	}
	mediaContainer.clean();
	
		
}
// obsolete + confusing with thumbnail width/height
void ACMediaDocument::saveACLSpecific(ofstream &library_file) {
}

// obsolete + confusing with thumbnail width/height
int ACMediaDocument::loadACLSpecific(ifstream &library_file) {
	return 1;
}

void ACMediaDocument::saveXMLSpecific(TiXmlElement* _media){
}


int ACMediaDocument::loadXMLSpecific(TiXmlElement* _pMediaNode){
	return 1;	
}


#endif //defined (SUPPORT_MULTIMEDIA)