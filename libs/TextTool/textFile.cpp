/*
 *  fileText.cpp
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

#include "textFile.h"
#include <iostream>
#include <fstream>
#include <ArchipelReader.h>
using namespace std;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

string* textFileRead(string filePath){
	string ext=fs::extension(filePath);
	if (ext==string(".txt")) 
		return txtFileRead(filePath);
	else
		if (ext==string(".xml")){
			archipelReader doc(filePath);
			if (!doc.isArchipel())
				return 0;
			return new string(doc.getText());
			vector<string> data=doc.getGlossaire();
			string desc;
			for (vector<string>::iterator it=data.begin();it!=data.end();it++)
			{	desc+=(*it);
				desc+=string(" ");
			}
			data=doc.getIlot();
			for (vector<string>::iterator it=data.begin();it!=data.end();it++)
			{	desc+=(*it);
				desc+=string(" ");
			}
			string test=doc.getThumbPath();
			return new string(desc);
		}
		else
			return 0;
	//TODO implement other text file type (pdf xml html...)
	return 0;
}

string labelFileRead(string filePath){
	string ext=fs::extension(filePath);
	if (ext==string(".txt")) {
		size_t indTemp=filePath.find_last_of("/"),lastTemp=filePath.length();
		return filePath.substr(indTemp,lastTemp-indTemp);
	}
	else
		if (ext==string(".xml")){
			archipelReader doc(filePath);
			return (doc.getArtist()+string("/")+doc.getAlbumName()) ;			
		}
		else
			return string("");
	//TODO implement other text file type (pdf xml html...)
	return string("");
}

string *txtFileRead(string filePath){
	fstream inFile(filePath.c_str(), fstream::in);
	if (!inFile.is_open())
		return 0;
	string* lOut=new string;
	do {
		string stringTemp;
		inFile>>stringTemp;
		if (stringTemp.size()>0){
			(*lOut)+=' ';
			(*lOut)+=stringTemp;
		}			
	}while (!inFile.eof());
	
	//cout << (*lOut)<< "\n";
	return lOut;
}