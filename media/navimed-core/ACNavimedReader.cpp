/*
 *  ACNavimedReader.cpp
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

#include "ACNavimedReader.h"
#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp> 
#include <boost/iostreams/code_converter.hpp>

#include <boost/locale.hpp>

using namespace std;
using namespace boost::locale;

ACNavimedReader::ACNavimedReader(const string fileName){
	mFileName=fileName;
	mDoc=new TiXmlDocument(fileName);
	bool loadOkay = mDoc->LoadFile();
	TiXmlDeclaration *xmlDecl=mDoc->FirstChild()->ToDeclaration();
	mEncoding =xmlDecl->Encoding();	
}



ACNavimedReader::~ACNavimedReader(void){
	delete mDoc;
}

bool ACNavimedReader::isNavimed(){
	TiXmlHandle docHandle( mDoc );
	
	
	TiXmlHandle child= docHandle.FirstChild("XMLResult" );
	if (child.ToNode()==0)
		return false;
	else 
		return true;
}


bool ACNavimedReader::isNavimedBiology(){
	return (this->getSubject()==string("Biologie"));
}

bool ACNavimedReader::isNavimedRadiography(){
	return (this->getSubject()==string("RADIOLOGIE"));
}

string ACNavimedReader::getText(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("XMLResult").FirstChild( "XMLTextValue" );//.FirstChild( "content" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	int i=0;
	while (1) 
	{
		textChild=child.Child(i).FirstChild().ToText();
		//ret=string(textChild->Parse(0,0,TIXML_ENCODING_UTF8));
		if (textChild==0)
			break;
		ret+=textChild->ValueStr();
		ret+=string(" ");
		i++;
	}
	std::string utf8_string = conv::to_utf<char>(ret,mEncoding);
	return utf8_string;
}

string ACNavimedReader::getSubject(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("XMLResult" ).FirstChild( "subject" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	if (textChild==0)
		return string("");
	ret=textChild->ValueStr();
	
	return ret;
}

string ACNavimedReader::getDescription(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("XMLResult" ).FirstChild( "description" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	if (textChild==0)
		return string("");
	ret=textChild->ValueStr();
	return ret;
}

string ACNavimedReader::getReference(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("XMLResult" ).FirstChild( "info" ).FirstChild( "reference" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	if (textChild==0)
		return string("");
	ret=textChild->ValueStr();
	return ret;
}


bool ACNavimedReader::getBioParam(std::string paramName,float &paramValue){
	string ret;
	TiXmlNode* child= mDoc->FirstChild("XMLResult" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	for( child = child->FirstChild(string("XMLEncodedValue")); child; child = child->NextSibling(string("XMLEncodedValue")) ){
		if (child->FirstChild(string("libelle"))==0)
			continue;
		if (child->FirstChild(string("libelle"))->FirstChild()==0)
			continue;
		if (child->FirstChild(string("libelle"))->FirstChild()->ToText()==0)
			continue;
		
		std::string utf8_string = conv::to_utf<char>(child->FirstChild(string("libelle"))->FirstChild()->ToText()->ValueStr(),mEncoding);
		//cout<<utf8_string<<endl;
		if (utf8_string==paramName){
			if (child->FirstChild(string("indice"))!=0){
				paramValue=this->convertValue(child->FirstChild(string("indice"))->FirstChild()->ToText()->ValueStr());
			}
			else{
				paramValue=0.f;
			}
			return true;
		}
	}
	if (textChild==0)
		return false;
}

map<string,float> *ACNavimedReader::getBioParam(void){
	map<std::string,float> *desc=new map<std::string,float>;
	string ret;
	TiXmlNode* child= mDoc->FirstChild("XMLResult" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	for( child = child->FirstChild(string("XMLEncodedValue")); child; child = child->NextSibling(string("XMLEncodedValue")) ){
		if (child->FirstChild(string("libelle"))==0)
			continue;
		if (child->FirstChild(string("libelle"))->FirstChild()==0)
			continue;
		if (child->FirstChild(string("libelle"))->FirstChild()->ToText()==0)
			continue;
		float paramValue;
		std::string utf8_string = conv::to_utf<char>(child->FirstChild(string("libelle"))->FirstChild()->ToText()->ValueStr(),mEncoding);
		if (child->FirstChild(string("indice"))!=0){
			paramValue=this->convertValue(child->FirstChild(string("indice"))->FirstChild()->ToText()->ValueStr());
		}
		else{
			paramValue=0.f;
		}
		(*desc)[utf8_string]=paramValue;
	}
	return desc;
}

float ACNavimedReader::convertValue(string valStr){
	if (valStr=="--")
		return -2.f;
	if (valStr=="-")
		return -1.f;
	if (valStr=="+")
		return 1.f;
	if (valStr=="++")
		return 2.f;
	return 0.f;
};



vector<std::string> ACNavimedReader::getRadiosName(void){
	vector<string> ret;

	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("XMLResult" ).FirstChild( "tracks" );//.Element();//.Child( "Child", 1 ).ToElement();
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

std::string ACNavimedReader::getMetaData(){
    if (this->isNavimedBiology()){
        string ret;
        TiXmlNode* child= mDoc->FirstChild("XMLResult" );
        TiXmlText* textChild;
        
        for( child = child->FirstChild(string("XMLEncodedValue")); child; child = child->NextSibling(string("XMLEncodedValue")) ){
            if (child->FirstChild(string("libelle"))==0)
                continue;
            if (child->FirstChild(string("libelle"))->FirstChild()==0)
                continue;
            if (child->FirstChild(string("libelle"))->FirstChild()->ToText()==0)
                continue;
            float paramValue;
            ret+=std::string("\t");
            ret+= conv::to_utf<char>(child->FirstChild(string("libelle"))->FirstChild()->ToText()->ValueStr(),mEncoding);
            ret+=std::string(" ");
            if (child->FirstChild(string("indice"))!=0){
                ret+=child->FirstChild(string("indice"))->FirstChild()->ToText()->ValueStr();
            }
            else{
                ret+=std::string("0");
            }
            ret+=std::string("\n");
        }
        return ret;
        
    }
    else{
        if (this->isNavimedRadiography()){
            string ret;
            ret=this->getDescription();
            ret+=std::string("\n");
            ret+=this->getText();
            
            
            return ret;
            
        }
        else{
            return std::string("");
        }
    }
    
}

std::string ACNavimedReader::getThumbPath(void){
	string ret;
	TiXmlHandle docHandle( mDoc );
	TiXmlHandle child= docHandle.FirstChild("XMLResult" ).FirstChild( "info" ).FirstChild( "thumb" );//.Element();//.Child( "Child", 1 ).ToElement();
	TiXmlText* textChild;
	
	textChild=child.FirstChild().ToText();
	ret=textChild->ValueStr();
	return ret;
	
}
