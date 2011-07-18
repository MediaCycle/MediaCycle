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

#if defined (SUPPORT_PDF)

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
	if (image_stream) image_stream->quit();	
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
	// option 1 : use openCV
	//CvCapture* capture = data_ptr->getData();
		
	// take thumbnail in the middle of the PDF...
	/*int nframes = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, nframes/2); 	
	
	if(!cvGrabFrame(capture)){
		cerr << "<ACPDF::computeThumbnail> Could not find frame..." << endl;
		return -1;
	}
	
	//XS use cloneImage, otherwise thumbnail gets destroyed along with cvCapture
	IplImage* tmp = cvRetrieveFrame(capture);
	thumbnail = cvCloneImage(tmp);*/
	
	//CF we should compute the following in a separate thread
/*	
	// option 2 : Loading the movie with OSG
	std::cout << boost::filesystem::extension(filename);
	/// prerequisites for loading OSG media files, 2 alternatives
	/// 1) standard procedure: checking for a plugin that can open the format of the media file
	osgDB::ReaderWriter* readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(filename).substr(1));
	/// 2) hack: forcing the use of the ffmpeg plugin by checking the plugin that can open the ffmpeg format (most probably the ffmpeg plugin)
	//osgDB::ReaderWriter* readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");
	if (!readerWriter){
		cerr << "<ACPDF::computeThumbnail> problem loading file, no OSG plugin available" << endl;
		return false;
	}

	osg::ref_ptr<osg::Image> thumbnail = osgDB::readImageFile(filename);
	// XS TODO : needs rescaling !!
	//thumbnail->scaleImage(thumbnail_width,thumbnail_height,1);
	//thumbnail->setAllocationMode(osg::Image::NO_DELETE);
	
	if (!thumbnail){
		cerr << "<ACPDF::computeThumbnail> problem creating thumbnail" << endl;
		return false;
	}
	
	// Saving the PDF as texture for transmission
	image_texture = new osg::Texture2D;
	image_texture->setImage(thumbnail);
	
	// Converting the PDF as preloaded stream to transmit the same instance to multiple recipient with unified playback controls
	image_stream = dynamic_cast<osg::ImageStream*>(thumbnail.get());
	image_stream->setLoopingMode(osg::ImageStream::LOOPING);
	
	// Hack to display a first valid frame, quite long!
	//while (thumbnail->isImageTranslucent())
	//	image_stream->play();
	//image_stream->pause();
	//image_stream->rewind();
*/	
	return ok;
}

PoDoFo::PdfDocument* ACPDF::getData()
{
	if (data == 0) {
		data = new ACPDFData(filename);
	}	
	return static_cast<PoDoFo::PdfDocument*> (data->getData());
}

//ACMediaData* ACPDF::extractData(string _fname){
bool ACPDF::extractData(string _fname){
	// XS todo : store the default header (16 below) size somewhere...
	this->filename = _fname;
	
	// From the podofopdfinfo tool
	PoDoFo::PdfDocument* pdf_doc = this->getData();
	page_count = pdf_doc->GetPageCount();
	start = 0;
	end = page_count;
	
	if( !pdf_doc->GetInfo() )
        std::cerr << "No info dictionary in this PDF file!" << std::endl;
    else
    {
        author = pdf_doc->GetInfo()->GetAuthor().GetStringUtf8();
        creator = pdf_doc->GetInfo()->GetCreator().GetStringUtf8();
        subject = pdf_doc->GetInfo()->GetSubject().GetStringUtf8();
        title = pdf_doc->GetInfo()->GetTitle().GetStringUtf8();
        keywords = pdf_doc->GetInfo()->GetKeywords().GetStringUtf8();
    }
	
	typedef std::pair<double,double> Format;
	
	PoDoFo::PdfPage*  curPage;
	int	pgCount = pdf_doc->GetPageCount();
	std::map<  Format , int > sizes;
	std::map<  Format , int >::iterator sIt;
	PoDoFo::PdfRect  rect;
	for ( int pg=0; pg<pgCount; pg++ ) 
	{
		curPage = pdf_doc->GetPage( pg );
		rect = curPage->GetMediaBox();
		Format s( rect.GetWidth() - rect.GetLeft(), rect.GetHeight() - rect.GetBottom());
		sIt = sizes.find(s);
		if(sIt == sizes.end())
			sizes.insert(std::pair<Format,int>(s,1));
		else
			++(sIt->second);
	}
	
	Format format;
	std::stringstream ss;
	if(sizes.size() == 1)
	{
		format = sizes.begin()->first;
		ss << format.first << " x " << format.second << " pts"  ;
		format_unit = "pts";
	}
	else
	{
		// WeÕre looking for the most represented format
		int max=0;
		for(sIt = sizes.begin();sIt != sizes.end(); ++sIt)
		{
			if(sIt->second > max)
			{
				max = sIt->second;
				format = sIt->first;
			}
		}
		ss << format.first << " x " << format.second << " pts "<<std::string(sizes.size(), '*');
		format_unit = std::string(sizes.size(), '*');
	}
	width = format.first;
	height = format.second;
	
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
	
	// XS TODO : do this here ??
	computeThumbnail(16, 16);
}

void ACPDF::deleteData(){
	if (data)
		delete data;
	data=0;
}

void ACPDF::setData(PoDoFo::PdfMemDocument* _data){
	if (data == 0)
		data = new ACPDFData();	
	data->setData(_data);

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

#endif //defined (SUPPORT_PDF)
