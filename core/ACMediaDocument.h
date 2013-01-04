/*
 *  ACMediaDocument.h
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

#ifndef ACMEDIADOCUMENT_H
#define ACMEDIADOCUMENT_H

#if defined (SUPPORT_MULTIMEDIA)
#include "ACMedia.h"
//#include "ACMediaDocumentData.h"
#include <string>
#include <iostream>

// -----------------------------------

typedef std::map<std::string ,ACMedia* > ACMediaContainer;

class ACMediaDocument: public ACMedia {
	// contains the *minimal* information about an image
public:
	ACMediaDocument();
	ACMediaDocument(const ACMediaDocument&, bool reduce = true);
    virtual ~ACMediaDocument();
    virtual std::string getIdentifier()=0;
	virtual ACMediaType getActiveSubMediaType();
	int setActiveSubMedia(std::string mediaName);
	virtual int import(std::string _path, int _mid=0, ACPluginManager *acpl=0, bool _save_timed_feat=false);
	void saveACLSpecific(std::ofstream &library_file);
	int loadACLSpecific(std::ifstream &library_file);
	void saveXMLSpecific(TiXmlElement* _media);
	int loadXMLSpecific(TiXmlElement* _pMediaNode);

	//void setThumbnail(osg::ref_ptr<osg::Image> _thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->width; thumbnail_height = _thumbnail->height; }
    //void setThumbnail(osg::ref_ptr<osg::Image> _thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->s(); thumbnail_height = _thumbnail->t(); }
	bool extractData(std::string fname);
	ACMediaContainer getContainer(){return mediaContainer;}

	std::vector<ACMediaFeatures*> &getAllFeaturesVectors() 
	{
		if (activeMedia!=0){
	
			return activeMedia->getAllFeaturesVectors();
		}
		else {
			return tempVect;
		}
	};
	ACMediaFeatures* getFeaturesVector(int i){
		if (activeMedia!=0){
			
			return activeMedia->getFeaturesVector(i);
		}
		else {
			return 0;		
		}
	};
	ACMediaFeatures* getFeaturesVector(std::string feature_name){
		if (activeMedia!=0){
			
			return activeMedia->getFeaturesVector(feature_name);
		}
		else {
			return 0;		
		}
	};
	int getNumberOfFeaturesVectors() {
		if (activeMedia!=0){
			return activeMedia->getNumberOfFeaturesVectors();
		}
		else {
			return 0;		
		}
	};
	std::vector<std::string> getListOfFeaturesPlugins(){
		if (activeMedia!=0){
			return activeMedia->getListOfFeaturesPlugins();
		}
		else {
			return tempString;		
		}
	};
	
	std::vector<ACMediaFeatures*> &getAllPreProcFeaturesVectors() { 
		if (activeMedia!=0){
			
		return activeMedia->getAllPreProcFeaturesVectors(); 
		}
		else {
			return tempVect;
		}
    }
	 ACMediaFeatures* getPreProcFeaturesVector(int i){ return activeMedia->getPreProcFeaturesVector(i); };
     virtual ACMediaFeatures* getPreProcFeaturesVector(std::string feature_name){ return activeMedia->getPreProcFeaturesVector(feature_name); };
     int getNumberOfPreProcFeaturesVectors(){
         return activeMedia->getNumberOfPreProcFeaturesVectors();
     }
    std::vector<std::string> getListOfPreProcFeaturesPlugins(){
        std::vector<std::string> preProcFeaturesPlugins;
        if(activeMedia)
            preProcFeaturesPlugins = activeMedia->getListOfPreProcFeaturesPlugins();
        else
            std::cerr << "ACMediaDocument::getListOfPreProcFeaturesPlugins: active media not set" << std::endl;
        return preProcFeaturesPlugins;
    }
	void defaultPreProcFeatureInit(void);
	void cleanPreProcFeaturesVector(void);

    virtual std::vector<std::string> getActivableMediaKeys()=0;
    ACMedia* getActiveMedia(void){return activeMedia;}
    std::string getActiveMediaKey(void){return activeMediaKey;}
	
private:
	void init();
	bool computeThumbnail(std::string _fname, int w=0, int h=0);
//	bool computeThumbnail(ACMediaDocumentData* data_ptr, int w=0, int h=0);
//	bool computeThumbnail(IplImage* img, int w=0, int h=0);
	bool computeThumbnailSize(int w_, int h_);
	void deleteMedia();
public:
    int addMedia(std::string key, ACMedia* media);
protected:
	int thumbnail_width, thumbnail_height; 
    //osg::ref_ptr<osg::Image> thumbnail;
    //osg::ref_ptr<osg::Texture2D> image_texture;
	//ACMediaDocumentData* data; No Need, data are stored in the respective media 
	ACMediaContainer mediaContainer;
	static	const int default_thumbnail_area ; // 128*128
	int mediaID;
	void incrementMediaID(){mediaID++ ;}
	int getMediaID(){return mediaID ;}
	std::vector<ACMediaFeatures*> tempVect;
	std::vector<std::string> tempString;
protected:
	ACMedia* activeMedia;
	std::string activeMediaKey;

};

#endif //defined (SUPPORT_MULTIMEDIA)
#endif // ACMEDIADOCUMENT_H
