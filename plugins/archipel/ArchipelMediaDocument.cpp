/*
 *  ArchipelMediaDocument.cpp
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

#include "ArchipelMediaDocument.h"

#include "ArchipelReader.h"

#include <stdio.h>
#include <stdlib.h>

ArchipelMediaDocument::ArchipelMediaDocument():ACMediaDocument(){
}

int ArchipelMediaDocument::import(std::string _filename, int _mid, ACPluginManager *acpl, bool _save_timed_feat){
	filename=_filename;	
	if (_mid>=0) this->setId(_mid);
	string extension = boost::filesystem::extension(filename);
	string path=boost::filesystem::path(_filename).parent_path().string()+string("/");
	if (extension!=string(".xml"))
		return 0;
	else{
		//read the xml file. We begin with the Mediacycle xml style
		archipelReader* xmlDoc=new archipelReader(filename);
		if (xmlDoc->isArchipel()){
			
			label=xmlDoc->getArtist()+string("/")+xmlDoc->getAlbumName();
			
			int nbMedia=0;
			ACMediaType mediaType;
			ACMedia *media ;
			string mediaFileName;
			string mediaExtension;
			ACMediaType fileMediaType;
			string mediaRef;
			int i=0;
			//archipelText
			mediaRef="main";
			mediaType=MEDIA_TYPE_TEXT;
			mediaFileName=filename;
			mediaExtension = boost::filesystem::extension(mediaFileName);
			fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
			if (fileMediaType!=mediaType){
				cout << "ArchipelMediaDocument::import other media type, skipping "  << endl;
				return 0;
			
			}
			media = ACMediaFactory::getInstance().create(mediaExtension);
			if (media->import(mediaFileName, this->getMediaID()+i+1, acpl, _save_timed_feat)){
				if (this->addMedia(mediaRef, media)){
					//this->incrementMediaID();
					if(mediaRef=="main")
						this->activeMedia = media;
				}	
			}
			else 
				return 0;
			nbMedia++;
			//Image
			mediaRef="covert";
			mediaType=MEDIA_TYPE_IMAGE;
			mediaFileName=path+xmlDoc->getThumbPath();
			mediaExtension = boost::filesystem::extension(mediaFileName);
			fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
			if (fileMediaType!=mediaType){
				cout << "ArchipelMediaDocument::import other media type, skipping "  << endl;
				return 0;
				
			}
			media = ACMediaFactory::getInstance().create(mediaExtension);
			if (media->import(mediaFileName, this->getMediaID()+i+1, acpl, _save_timed_feat)){
				if (this->addMedia(mediaRef, media)){
					//this->incrementMediaID();
					if(mediaRef=="main")
						this->activeMedia = media;
				}	
			}
			else 
				return 0;
			nbMedia++;
			//tracks
			vector<string> tracksPaths=xmlDoc->getTrackPath();
			unsigned int nbTracks=tracksPaths.size();
			for (int i=0;i<nbTracks;i++){
				std::stringstream out;
				out << "track"<<i;
				mediaRef=out.str() ;
				mediaType=MEDIA_TYPE_AUDIO;
				mediaFileName=path+tracksPaths[i];
				//mediaFileName=mediaFileName.substr(0, mediaFileName.size()-4)+string(".wav");
				mediaExtension = boost::filesystem::extension(mediaFileName);
				fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
				
				if (fileMediaType!=mediaType){
			
					cout << "ArchipelMediaDocument::impossible to timport:"<< mediaFileName << endl;
					continue;
					
				}
				media = ACMediaFactory::getInstance().create(mediaExtension);
				if (media->import(mediaFileName, this->getMediaID()+i+1, acpl, true)){
					if (this->addMedia(mediaRef, media)){
						//this->incrementMediaID();
						if(mediaRef=="main")
							this->activeMedia = media;
					}	
				}
				else 
					return 0;
				nbMedia++;
				
			}
			
			
			
			/*
			for (unsigned int i=0;i<nbMedia; i++){
				string s_media_type=xmlDoc->getMediaType(i);
				ACMediaType mediaType;
				stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(s_media_type);
				
				if( iterm == stringToMediaType.end() ) {
					cout << "ArchipelMediaDocument::import media type not found : " << s_media_type << endl;
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
				if (media->import(path+mediaFileName, this->getMediaID()+i+1, acpl, _save_timed_feat)){
					if (this->addMedia(mediaRef, media)){
						//this->incrementMediaID();
						if(mediaRef=="main")
							this->activeMedia = media;
					}	
				}
				else 
					delete media;
			}*/
			delete xmlDoc;
			return nbMedia;
		}
		else {
			return 0;
		}

	
		
	}
		
}