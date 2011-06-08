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

#ifndef ACMEDIADOCUMENT_H
#define ACMEDIADOCUMENT_H

#if defined (SUPPORT_MULTIMEDIA) 
#include "ACOpenCVInclude.h"
#include "ACMedia.h"
#include "ACMediaDocumentData.h"
#include <string>
#include <iostream>

#include <osg/Texture2D>

// -----------------------------------


class ACMediaDocument: public ACMedia {
	// contains the *minimal* information about an image
public:
	ACMediaDocument();
	~ACMediaDocument();
	ACMediaDocument(const ACMediaDocument&, bool reduce = true);

	
	void saveACLSpecific(std::ofstream &library_file);
	int loadACLSpecific(std::ifstream &library_file);
	void saveXMLSpecific(TiXmlElement* _media);
	int loadXMLSpecific(TiXmlElement* _pMediaNode);

	//void setThumbnail(osg::ref_ptr<osg::Image> _thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->width; thumbnail_height = _thumbnail->height; }
	void setThumbnail(osg::ref_ptr<osg::Image> _thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->s(); thumbnail_height = _thumbnail->t(); }
	int getThumbnailWidth() {return thumbnail_width;}
	int getThumbnailHeight() {return thumbnail_height;}
	void* getThumbnailPtr() { return (void*)image_texture; }
	
	ACMediaDocumentData* getData(){return static_cast<ACMediaDocumentData*>(data->getData());}
	void setData(ACMediaDocumentData* _data);
	virtual ACMediaData* getMediaData(){return data;} // XS TODO : needs dynamic_cast<ACMediaData*> (data) ??
	void extractData(std::string fname);
	virtual void deleteData();

private:
	void init();
	bool computeThumbnail(std::string _fname, int w=0, int h=0);
	bool computeThumbnail(ACMediaDocumentData* data_ptr, int w=0, int h=0);
	bool computeThumbnail(IplImage* img, int w=0, int h=0);
	bool computeThumbnailSize(int w_, int h_);
	void deleteMedia();

private:
	int thumbnail_width, thumbnail_height; 
	osg::ref_ptr<osg::Image> thumbnail;
	osg::ref_ptr<osg::Texture2D> image_texture;
	ACMediaDocumentData* data;
	std::map<ACMediaType ,ACMedia*>	mediaContainer;
	
	
};

#endif //defined (SUPPORT_MULTIMEDIA)
#endif // ACMEDIADOCUMENT_H