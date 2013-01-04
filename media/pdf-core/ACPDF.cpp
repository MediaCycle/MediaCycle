/*
 *  ACPDF.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 27/05/2011
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

#include "ACPDF.h"

#include <osg/ImageUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include "boost/filesystem.hpp"

#include <iostream>
using namespace std;

// ----------- class constants
const int ACPDF:: default_thumbnail_area = 4096; // 64 * 64

ACPDF::ACPDF() : ACMedia()
{
	this->init();
}	

void ACPDF::init()
{
	media_type = MEDIA_TYPE_PDF;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;
	data=0;
	author = "";
	creator = "";
	subject = "";
	title = "";
	keywords = "";
	format_unit = "";
	page_count = 0;	
}	

ACPDF::~ACPDF() {
}

ACPDF::ACPDF(const ACPDF& m) : ACMedia(m) {
	this->init();	
	thumbnail = m.thumbnail;
	thumbnail_width = m.thumbnail_width;
	thumbnail_height = m.thumbnail_height;
	// Should I copy the thumbnail ?
	author = m.author;
	creator = m.creator;
	subject = m.subject;
	title = m.title;
	keywords = m.keywords;
	format_unit = m.format_unit;
	page_count = m.page_count;
}	

bool ACPDF::computeThumbnail(int w, int h){
	if (!computeThumbnailSize(w, h)) return false;
	bool ok = true;
	return ok;
}

//PoDoFo::PdfDocument* ACPDF::getData()
//{
//	if (data == 0) {
//		data = new ACPDFData(filename);
//	}
//	return static_cast<PoDoFo::PdfDocument*> (data->getData());
//}

//ACMediaData* ACPDF::extractData(string _fname){
bool ACPDF::extractData(string _fname){

    ACPDFData* pdf_data = dynamic_cast<ACPDFData*>(this->getMediaData());
    if(!pdf_data){
        std::cerr << "ACPDF::extractData: incorrect pdf data" << std::endl;
        return false;
    }

    this->page_count = pdf_data->getNumberOfPages();
    this->start = 0;
    this->end = page_count;
	
    this->author = pdf_data->getAuthor();
    this->creator = pdf_data->getCreator();
    this->subject = pdf_data->getSubject();
    this->title = pdf_data->getTitle();
    this->keywords = pdf_data->getKeywords();
    this->width = pdf_data->getWidth();
    this->height = pdf_data->getHeight();
	
	#ifdef USE_DEBUG
	std::cout << "Extracted PDF file '" << _fname << "'";
	std::cout << " with " << page_count << " pages";
	std::cout << " of size " << width << " X " << height << " ";
	std::cout << " - author(" << author << ")";
	std::cout << " - creator(" << creator << ")";
	std::cout << " - title(" << title << ")";
	std::cout << " - keywords(" << keywords << ")";
	std::cout << std::endl;
	#endif
	
	return computeThumbnail(16, 16);
}

// obsolete + confusing with thumbnail width/height
void ACPDF::saveACLSpecific(ofstream &library_file) {
	library_file << filename_thumbnail << endl;
	library_file << this->getDuration() << endl;
	library_file << width << endl;
	library_file << height << endl;
}

// obsolete + confusing with thumbnail width/height
int ACPDF::loadACLSpecific(ifstream &library_file) {
	std::string ghost;//CF somebody please explain me why this is required!
	getline(library_file, ghost, '\n');//CF somebody please explain me why this is required!
	getline(library_file, filename_thumbnail, '\n');
	library_file >> end;
	library_file >> width;
	library_file >> height;
	//int n_features = 0;//CF done in ACMedia
	//library_file >> n_features;//CF done in ACMedia
	return 1;
}

void ACPDF::saveXMLSpecific(TiXmlElement* _media){
	_media->SetAttribute("thumbnailFileName", filename_thumbnail);
	_media->SetAttribute("Width", width);
	_media->SetAttribute("Height", height);
	_media->SetAttribute("ThumbnailWidth", thumbnail_width);
	_media->SetAttribute("ThumbnailHeight", thumbnail_height);
	_media->SetAttribute("Author", author);
	_media->SetAttribute("Creator", creator);
	_media->SetAttribute("Subject", subject);
	_media->SetAttribute("Title", title);
	_media->SetAttribute("Keywords", keywords);
	_media->SetAttribute("Page Count", page_count);
}

int ACPDF::loadXMLSpecific(TiXmlElement* _pMediaNode){
	int w=-1;
	int h=-1;
	int t_w=-1;
	int t_h=-1;
	int pdf_p = 0;

	// XS TODO is this one necessary ?
	filename_thumbnail = _pMediaNode->Attribute("thumbnailFileName");
	
	double t=0;

	_pMediaNode->QueryIntAttribute("Width", &w);
	if (w < 0)
		throw runtime_error("corrupted XML file, wrong image width");
	else
		this->width = w;
	
	_pMediaNode->QueryIntAttribute("Height", &h);
	if (h < 0)
		throw runtime_error("corrupted XML file, wrong image height");
	else
		this->height = h;
	
	_pMediaNode->QueryIntAttribute("ThumbnailWidth", &t_w);
	_pMediaNode->QueryIntAttribute("ThumbnailHeight", &t_h);
	
	if (computeThumbnail (t_w , t_h) != 1)
		throw runtime_error("<ACPDF::loadXMLSpecific> : problem computing thumbnail");

	this->author = _pMediaNode->Attribute("Author");
	this->creator = _pMediaNode->Attribute("Creator");
	this->subject = _pMediaNode->Attribute("Subject");
	this->title = _pMediaNode->Attribute("Title");
	this->keywords = _pMediaNode->Attribute("Keywords");	
	
	_pMediaNode->QueryIntAttribute("Page Count", &pdf_p);
	if (pdf_p == 0)
		throw runtime_error("corrupted XML file, wrong page count");
	else
		this->author = pdf_p;
	
	
	// not necessary
	//	data = new ACPDFData(filename);
	
	return 1;	
}

// cut and paste from ACImage
// XS TODO should put this in ACSpatialMedia
bool ACPDF::computeThumbnailSize(int w_, int h_){
	// we really want a specific (positive) thumbnail size
	bool ok = true;
	if ((w_ > 0) && (h_ > 0)) {
		thumbnail_width = w_;
		thumbnail_height = h_;
	}
	// we just scale the original width and height to the default thumbnail area
	if ((width !=0) && (height!=0)){
		float scale = sqrt((float)default_thumbnail_area/((float)width*(float)height));
		thumbnail_width = (int)(width*scale);
		thumbnail_height = (int)(height*scale);
	}
	else {
		std::cerr << "PDF dimensions not set." << std::endl;
		ok = false;
	}
	return ok;
}
