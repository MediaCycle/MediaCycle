/*
 *  textFile.cpp
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
#define TIXML_USE_STL
#include <tinyxml.h>

#include <boost/locale.hpp>

using namespace std;
using namespace boost::locale;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

string* textFileRead(string filePath){
    string ext=fs::extension(filePath);
    if (ext==string(".txt"))
        return txtFileRead(filePath);
    else
        if (ext==string(".xml")){
            return xmlFileRead(filePath);
            /*TiXmlDocument *testDoc=new TiXmlDocument(filePath);
            delete testDoc;
            archipelReader *doc=new archipelReader(filePath);
            if (!doc->isArchipel())
            {

                delete doc;
#if defined(SUPPORT_NAVIMED)
                navimedReader *doc2=new navimedReader(filePath);
                if (doc2->isNavimed()){
                    string *desc2=new string(doc2->getText());
                    delete doc2;
                    return desc2;
                }
                else
#endif //SUPPORT_NAVIMED
                    return 0;
            }
            string *desc2=new string(doc->getText());
            delete doc;
            return desc2;
            string desc;
            vector<string> data=doc->getGlossaire();
            for (vector<string>::iterator it=data.begin();it!=data.end();it++)
            {	desc+=(*it);
                desc+=string(" ");
            }
            data=doc->getIlot();
            for (vector<string>::iterator it=data.begin();it!=data.end();it++)
            {	desc+=(*it);
                desc+=string(" ");
            }
            string test=doc->getThumbPath();
            delete doc;
            return new string(desc);*/
        }
        else
            return 0;
    //TODO implement other text file type (pdf xml html...)
    return 0;
}

string labelFileRead(string filePath){
    string ext=fs::extension(filePath);
    //	if (ext==string(".txt")) {
    fs::path _path(filePath);
    string _label = _path.stem().string();
    return _label;
    /*	}
    else
        if (ext==string(".xml")){
            archipelReader *doc=new archipelReader(filePath);
            if (!doc->isArchipel()){
                delete doc;
#if defined(SUPPORT_NAVIMED)
                navimedReader *doc2=new navimedReader(filePath);
                if (doc2->isNavimed()){
                    string desc=doc2->getSubject()+string("/")+doc2->getDescription()+string("/")+doc2->getReference();
                    delete doc2;
                    return desc;
                }
                else{
                    delete doc2;
                    return string("");
                }
#else 
                return string("");
#endif
            }
            string desc=doc->getArtist()+string("/")+doc->getAlbumName();
            delete doc;
            return desc;
        }
        else
            return string("");
    //TODO implement other text file type (pdf xml html...)
    return string("");*/
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

void recursXmlParser(TiXmlNode* parent,string* out,string encoding);

string *xmlFileRead(string filePath){//default xml reader. Parse and extract all the strings in the file.


    TiXmlDocument *doc=new TiXmlDocument(filePath);
    string encodingType;
    bool loadOkay = doc->LoadFile();

    TiXmlDeclaration *xmlDecl=doc->FirstChild()->ToDeclaration();;

    string encoding =xmlDecl->Encoding();
    if (!loadOkay)
        return 0;

    string* lOut=new string;
    recursXmlParser(doc,lOut,encoding);
    cout<<*lOut<<endl;

    return lOut;

}

void recursXmlParser(TiXmlNode* parent,string* out,string encoding){
	int i=0;
	TiXmlNode* child = 0;
	while( child = parent->IterateChildren( child ) ){
		TiXmlText* tempText=child->ToText();
		if (tempText){
            std::string utf8_string ;
            if (encoding!=string("UTF-8"))
                 utf8_string = conv::to_utf<char>(tempText->ValueStr(),encoding);
            else
                utf8_string=tempText->ValueStr();
			(*out)+=utf8_string+string(" ");
		}
		recursXmlParser(child,out,encoding);
	}
}
