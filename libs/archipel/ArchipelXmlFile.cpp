/*
 *  ArchipelXmlFile.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 16/11/10
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

#include "ArchipelXmlFile.h"
#include <iostream>
#include <fstream>
#include "ArchipelReader.h"
using namespace std;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

string* ArchipelXmlFileRead(string filePath){
	string ext=fs::extension(filePath);
	if (ext==string(".xml")){
		archipelReader doc(filePath);
		return new string(doc.getText());			
	}
	else
		return NULL;
}

string ArchipelLabelFileRead(string filePath){
	string ext=fs::extension(filePath);
	if (ext==string(".xml")){
		archipelReader doc(filePath);
		return (doc.getArtist()+string("/")+doc.getAlbumName()) ;			
	}
	else
		return string("");
}

IplImage* ArchipelThumbFileRead(std::string filePath){
	IplImage* thumb = 0;
	string ext=fs::extension(filePath);
	if (ext==string(".xml")){
		archipelReader doc(filePath);
		std::cout << "Thumbnail: " << fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath() << std::endl;
		
		thumb = cvLoadImage((fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath()).c_str(), CV_LOAD_IMAGE_COLOR);
		try {
			if (!thumb) {
				cerr << "Check file name : " << fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath() << endl;
				throw(string(" <ArchipelThumbFileRead> CV_LOAD_IMAGE_COLOR : not a color image !"));
			}
		}
		catch (const string& not_image_file) {
			cerr << not_image_file << endl;
		}
		return thumb;			
	}
	else
		return 0;
}

string ArchipelGetThumbFileName(std::string filePath){
	string ext=fs::extension(filePath);
	if (ext==string(".xml")){
		archipelReader doc(filePath);
		return fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath();
	}
	else
		return "";
}