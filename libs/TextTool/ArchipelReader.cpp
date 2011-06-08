/*
 *  ArchipelReader.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 6/06/11
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

#include "ArchipelReader.h"
#include <iostream>


using namespace std;

archipelReader::archipelReader(const string fileName){
	mFileName=fileName;
	mDoc=new TiXmlDocument(fileName);
	bool loadOkay = mDoc->LoadFile();
	
}

archipelReader::~archipelReader(void){
	
	delete mDoc;
	
	
}

vector<string> archipelReader::getIlot(void){
	vector<string> ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "keywords" ).FirstChild( "ilot" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	int i=0;
	while (1) {
		textChild=child.Child(i).FirstChild().ToText();
		if (textChild==0)
			break;
		ret.push_back(textChild->ValueStr());
		i++;
	}
	return ret;

}

vector<string> archipelReader::getGlossaire(void){
	vector<string> ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "keywords" ).FirstChild( "glossaire" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	int i=0;
	while (1) {
		textChild=child.Child(i).FirstChild().ToText();
		if (textChild==0)
			break;
		ret.push_back(textChild->ValueStr());
		i++;
	}
	return ret;
}

string archipelReader::getText(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "description" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	int i=0;
	while (1) {
		textChild=child.Child(i).FirstChild().ToText();
		if (textChild==0)
			break;
		ret+=textChild->ValueStr();
		ret+=string(" ");
		i++;
	}
	return ret;
}

string archipelReader::getArtist(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "info" ).FirstChild( "artist" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	ret=textChild->ValueStr();
	
	return ret;
}

string archipelReader::getAlbumName(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "info" ).FirstChild( "title" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	ret=textChild->ValueStr();
	return ret;
}

string archipelReader::getReference(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "info" ).FirstChild( "reference" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	ret=textChild->ValueStr();
	return ret;
	

}

vector<std::string> archipelReader::getTrackTitle(void){
	vector<string> ret;

	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "tracks" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	int i=0;
	while (1) {
		textChild=child.Child(i).FirstChild("title").FirstChild().ToText();
		if (textChild==0)
			break;
		ret.push_back(textChild->ValueStr());
		i++;
	}
	return ret;
}

std::vector<std::string> archipelReader::getTrackPath(void){
	
	size_t indTemp=mFileName.find_last_of("/"),lastTemp=mFileName.length();
	if (indTemp>lastTemp)
		indTemp=0;
	string rep=mFileName.substr(0,indTemp);
	
	vector<string> ret;

	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("archipel" ).FirstChild( "tracks" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	int i=0;
	while (1) {
		textChild=child.Child(i).FirstChild("path").FirstChild().ToText();
		if (textChild==0)
			break;
		ret.push_back(rep+textChild->ValueStr());
		i++;

	}
	return ret;
}