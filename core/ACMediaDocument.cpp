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

#include <boost/filesystem.hpp>
#include "ACMediaFactory.h"

#include "MCMultiMediaXmlReader.h"

using namespace std;

// ----------- class constants
const int ACMediaDocument:: default_thumbnail_area = 16384; // 128*128

//------------------------------------------------------------------

ACMediaDocument::ACMediaDocument() : ACMedia() {
	this->init();
	mediaID=0;
	activeMedia=0;
}

int ACMediaDocument::import(std::string _filename, int _mid, ACPluginManager *acpl, bool _save_timed_feat){	
	filename=_filename;	
	string extension = boost::filesystem::extension(filename);
	string path=boost::filesystem::path(_filename).parent_path().string()+string("/");

	if (extension==string(".xml")){
		//read the xml file. We begin with the Mediacycle xml style
		MCMultiMediaXmlReader* xmlDoc=new MCMultiMediaXmlReader(filename);
		label=xmlDoc->getLabel();
		unsigned int nbMedia=xmlDoc->getNumberOfMedia();
		for (unsigned int i=0;i<nbMedia; i++){
			string s_media_type=xmlDoc->getMediaType(i);
			ACMediaType mediaType;
			stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(s_media_type);
			
			if( iterm == stringToMediaType.end() ) {
				cout << "ACMediaDocument::import media type not found : " << s_media_type << endl;
				continue;
			}
			else {
				mediaType = iterm->second;
				//		cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;
			}
			string mediaFileName=xmlDoc->getMediaPath(i);
			
			string mediaExtension = boost::filesystem::extension(mediaFileName);
			ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
			if (fileMediaType!=mediaType){
				cout << "ACMediaDocument::import other media type, skipping " << s_media_type << endl;
				continue;
			}
			string mediaRef=xmlDoc->getMediaReference(i);
			ACMedia *media = ACMediaFactory::getInstance().create(mediaExtension);
			if (media->import(path+mediaFileName, this->getMediaID(), acpl, _save_timed_feat)){
				if (this->addMedia(mediaRef, media))
					this->incrementMediaID();
			}
			else 
				delete media;
		}
		delete xmlDoc;
	}
	else {
		ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(extension);
		if (fileMediaType==MEDIA_TYPE_VIDEO){
			//create a container with a video and an audio media
		}
		else {
			if (fileMediaType==MEDIA_TYPE_PDF){}
				//create a container with a text media and some image media
		}

	}
	if (_mid>=0) this->setId(_mid);
	if (this->mediaContainer.size()>0)
		activeMedia=(*(mediaContainer.begin())).second;
}

int ACMediaDocument::addMedia(std::string stringKey, ACMedia* media){
	
	if (mediaContainer.find(stringKey)==mediaContainer.end()){
		mediaContainer[stringKey]=media;
	}
	else
		return 0;
	return 1;
}
void ACMediaDocument::defaultPreProcFeatureInit(void){
	std::map<std::string ,ACMedia* >::iterator it;

	for (it=mediaContainer.begin();it!=mediaContainer.end();it++)
		it->second->defaultPreProcFeatureInit();
}


void ACMediaDocument::init() {
	media_type = MEDIA_TYPE_MIXED;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;
}	

// copy-constructor, used for example to generate fragments
ACMediaDocument::ACMediaDocument(const ACMediaDocument& m, bool reduce) : ACMedia(m) {
	this->init();
	// ... XS TODO
}

ACMediaDocument::~ACMediaDocument() {
	activeMedia=0;
	this->deleteData();
	// the osg pointers should be automatically deleted (ref_ptr)
}

// used when we load from XML file
// so there is no need to compute a pointer to the data passed to the plugin
// in this case w and h can be read from XML file
bool ACMediaDocument::computeThumbnail(string _fname, int w, int h){
	bool ok = true;
	
	return ok;
}


bool ACMediaDocument::computeThumbnail(IplImage* img, int w, int h){
	bool ok = true;
	return ok;
}

bool ACMediaDocument::computeThumbnailSize(int w_, int h_){
	bool ok = true;
	
	return ok;
}	

void ACMediaDocument::extractData(string fname){
//	computeThumbnail(data, thumbnail_width , thumbnail_height);
}



int ACMediaDocument::setActiveSubMedia(string mediaName){
	if (mediaContainer.find(mediaName)==mediaContainer.end()){
		return 0;
	}
	else
		activeMedia=mediaContainer.find(mediaName)->second;
	return 1;
}

void ACMediaDocument::deleteMedia(){
	map<string ,ACMedia* > ::iterator iter;
	for (iter=mediaContainer.begin(); iter!=mediaContainer.end();iter++){
		delete (iter->second);
		iter->second=0;
	}
	mediaContainer.clear();
	
		
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