/*
 *  NavimedMediaDocument.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 27/07/11
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
 *
 */

#include "NavimedMediaDocument.h"
#include "NavimedReader.h"

#include "NavimedMediaFactory.h"
#include <stdio.h>
#include <stdlib.h>
#include "NavimedSensor.h"

NavimedMediaDocument::NavimedMediaDocument():ACMediaDocument(){
}

int NavimedMediaDocument::import(std::string _filename, int _mid, ACPluginManager *acpl, bool _save_timed_feat){
	activableMediaKey.clear();
	filename=_filename;	
			
	int nbMedia=0;
	if (_mid>=0) this->setId(_mid);
	string extension = boost::filesystem::extension(filename);
	string path=boost::filesystem::path(_filename).parent_path().string()+string("/");
	if (extension!=string(".xml"))
		return 0;
	else{
		//read the xml file. We begin with the Mediacycle xml style
		navimedReader* xmlDoc=new navimedReader(filename);
		if (xmlDoc->isNavimed()){
			
			label=boost::filesystem::path(_filename).filename().string();
			if (xmlDoc->isNavimedRadiography()){
				ACMediaType mediaType;
				ACMedia *media ;
				string mediaFileName;
				string mediaExtension;
				ACMediaType fileMediaType;
				string mediaKey;
				
				//navimedText
				mediaKey="Description (Text)";
				activableMediaKey.push_back(mediaKey);
				
				mediaType=MEDIA_TYPE_TEXT;
				mediaFileName=filename;
				mediaExtension = boost::filesystem::extension(mediaFileName);
				fileMediaType = NavimedMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
				if (fileMediaType!=mediaType){
					cout << "NavimedMediaDocument::import other media type, skipping "  << endl;
					return 0;
					
				}
				media = NavimedMediaFactory::getInstance().create(mediaExtension);
				if (media->import(mediaFileName, this->getId()+nbMedia+1, acpl, _save_timed_feat)){
					if (this->addMedia(mediaKey, media)){
						this->setActiveSubMedia(mediaKey);
					}	
				}
				else {
                    delete xmlDoc;
                    delete media;
					return 0;
                }
				nbMedia++;
			}
			if (xmlDoc->isNavimedBiology()){
				ACMediaType mediaType;
				ACMedia *media ;
				string mediaFileName;
				string mediaExtension;
				ACMediaType fileMediaType;
				string mediaKey;
				
				//navimedText
				mediaKey="Biological parameter (Sensor)";
				activableMediaKey.push_back(mediaKey);
				
				mediaType=MEDIA_TYPE_SENSOR;
				mediaFileName=filename;
				media = new NavimedSensor();
				if (media->import(mediaFileName, this->getId()+nbMedia+1, acpl, _save_timed_feat)){
					if (this->addMedia(mediaKey, media)){
						this->setActiveSubMedia(mediaKey);
					}	
				}
				else 
					return 0;
				nbMedia++;
				label+=media->getLabel();
			}
			
			delete xmlDoc;
			return nbMedia;
		}
		else {
			return 0;
		}
	}
}
