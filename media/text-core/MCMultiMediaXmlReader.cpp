/*
 *  MCMultiMediaXmlReader.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 30/06/11
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

#include "MCMultiMediaXmlReader.h"


#include <boost/filesystem.hpp>

using namespace std;

MCMultiMediaXmlReader::MCMultiMediaXmlReader(std::string filename){
	
	doc=new TiXmlDocument(filename);
	loadOkay = doc->LoadFile();
	if (loadOkay)
	{
		TiXmlHandle docHandle( doc );
		TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
		if (child.ToNode()==0)
		{
			loadOkay=false;
			delete doc;
			doc=0;
		}		
	}
}

MCMultiMediaXmlReader::~MCMultiMediaXmlReader(){
	if (doc!=0)
		delete doc;
}

bool MCMultiMediaXmlReader::isMCMultiMediaXml(){
	return loadOkay;
}

string MCMultiMediaXmlReader::getReference(){
	TiXmlHandle docHandle( doc );
	TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
	
	TiXmlElement* element=child.ToElement();
	if (element)
	{
	const string * testRef=element->Attribute(string("reference"));
	return string(*testRef);
	}
	else 
		return string("");
	
}

string MCMultiMediaXmlReader::getLabel(){
	TiXmlHandle docHandle( doc );
	TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
	
	TiXmlHandle childTemp=child.FirstChild("label").FirstChild();
	if (childTemp.ToText()!=0){
		return string(childTemp.ToText()->ValueStr());
	}
	else 
		return string("");
		
}

unsigned int MCMultiMediaXmlReader::getNumberOfMedia(){
	TiXmlHandle docHandle( doc );
	TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
	unsigned int i=0;
	while (1){
		TiXmlHandle mediaChild=child.FirstChild("medias").Child(i);
		if (mediaChild.ToElement()==0)
			break;
		i++;
	}
	return i;
}


string MCMultiMediaXmlReader::getMediaType(unsigned int index){
	TiXmlHandle docHandle( doc );
	TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
	TiXmlHandle mediaChild=child.FirstChild("medias").Child(index);
	if (mediaChild.ToElement()==0)
		return string("");
	const string * s_media_type=mediaChild.ToElement()->Attribute(string("type"));
	
	return string(*s_media_type);

}


string MCMultiMediaXmlReader::getMediaReference(unsigned int index){
	TiXmlHandle docHandle( doc );
	TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
	TiXmlHandle mediaChild=child.FirstChild("medias").Child(index);
	if (mediaChild.ToElement()==0)
		return string("");
	const string * label=mediaChild.ToElement()->Attribute(string("label"));
	if (label==0)
		return string("");
	return string(*label);	
}


string MCMultiMediaXmlReader::getMediaPath(unsigned int index){
	TiXmlHandle docHandle( doc );
	TiXmlHandle child= docHandle.FirstChild("MediaCycleMutiMediaDocument" );
	
	string path= boost::filesystem::path(filename).parent_path().string();
	TiXmlHandle mediaChild=child.FirstChild("medias").Child(index).FirstChild("path").FirstChild();
	if (mediaChild.ToText()==0)
		return string("");
	return mediaChild.ToText()->ValueStr();
}

