/*
 *  TextTest.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 24/05/11
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

#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <ctime>

#include "ACText.h"

#include "ACTextFeaturesPlugin.h"
#include "ACIndexModifier.h"

#include "ACArchipelReader.h"
#include<iostream>

#include <QtGui>
using namespace std;

#ifdef _CL_HAVE_NAMESPACES
using namespace lucene::index;
#endif

#include "MediaCycle.h"
/*
#include <ctime>
const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;
	
	return &pINDENT[ LENGTH-n ];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;
	
	return &pINDENT[ LENGTH-n ];
}

int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;
	
	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
		
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;	
}

void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 )
{
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;
	
	switch ( t )
	{
		case TiXmlNode::TINYXML_DOCUMENT:
			printf( "Document" );
			break;
			
		case TiXmlNode::TINYXML_ELEMENT:
			printf( "Element [%s]", pParent->Value() );
			num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
			switch(num)
		{
			case 0:  printf( " (No attributes)"); break;
			case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
			default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
			break;
			
		case TiXmlNode::TINYXML_COMMENT:
			printf( "Comment: [%s]", pParent->Value());
			break;
			
		case TiXmlNode::TINYXML_UNKNOWN:
			printf( "Unknown" );
			break;
			
		case TiXmlNode::TINYXML_TEXT:
			pText = pParent->ToText();
			printf( "Text: [%s]", pText->Value() );
			break;
			
		case TiXmlNode::TINYXML_DECLARATION:
			printf( "Declaration" );
			break;
		default:
			break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

// load the named file and dump its structure to STDOUT
void dump_to_stdout(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		printf("\n%s:\n", pFilename);
		dump_to_stdout( &doc ); // defined later in the tutorial
	}
	else
	{
		printf("Failed to load file \"%s\"\n", pFilename);
	}
}

vector<ACMediaType> mediaExtract2(ACMediaType lMediaType){
	unsigned int cpt=lMediaType;
	unsigned int currMedia=1;
	vector<ACMediaType> desc;
	for (cpt=lMediaType;cpt>0;cpt>>=1,currMedia<<=1){
		if (lMediaType&currMedia)
		{
			desc.push_back(currMedia);
		}
	}
	return desc;
}
*/
int main(void){return 0;}
/*	
	ACSparseVector testSVec1,testSVec2,testSVec3;
	
	testSVec1.resize(3);	
	vector<float> *tempVect=new vector<float>;
	tempVect->push_back(10);
	testSVec1[0]=tempVect;
	tempVect=new vector<float>;
	tempVect->push_back(2);
	tempVect->push_back(4);
	testSVec1[1]=tempVect;
	tempVect= new vector<float>;
	tempVect->push_back(24);
	tempVect->push_back(10.5);
	testSVec1[2]=tempVect;

	testSVec2.resize(3);
	tempVect=new vector<float>;
	tempVect->push_back(10);
	testSVec2[0]=tempVect;
	tempVect=new vector<float>;
	tempVect->push_back(2);
	tempVect->push_back(5);
	tempVect->push_back(9);
	testSVec2[1]=tempVect;
	tempVect= new vector<float>;
	tempVect->push_back(24);
	tempVect->push_back(40.5);
	tempVect->push_back(70.97);
	testSVec2[2]=tempVect;
	
	testSVec3=SparseMatrixOperator::addVectors(*(testSVec1[0]),*(testSVec1[1]),*(testSVec1[2]),*(testSVec2[0]),*(testSVec2[1]),*(testSVec2[2]) );
	float test=1,test1=0.f;
	while (1) {
		 test=(float)clock();///CLOCKS_PER_SEC;
		
		//usleep(1);
		cout<<test<<endl;
		test1=test;
		
	}
	
	vector<ACMediaType> testVect=mediaExtract2(MEDIA_TYPE_AUDIO|MEDIA_TYPE_VIDEO|MEDIA_TYPE_TEXT);
	MediaCycle *testMC=new MediaCycle(MEDIA_TYPE_MIXED);
	testMC->getLibrary()->importFile(string("/Users/ravet/Desktop/MediaCycleData/testMediaDocument/testMediaDocument.xml"), testMC->getPluginManager(), false, false);
	return 0;
	/*TiXmlDocument doc("EA0236.xml");
		
	
	archipelReader testDoc("EA0236.xml");

	vector<string>::iterator it;
	std::vector<std::string> testvect=testDoc.getIlot();
	for (it=testvect.begin();it!=testvect.end();it++)
		cout << (*it) <<endl;
	
	
	testvect= testDoc.getGlossaire();
	for (it=testvect.begin();it!=testvect.end();it++)
		cout << (*it) <<endl;
	
	
	std::string strTest=testDoc.getText();
	cout << strTest <<endl;
	
	strTest=testDoc.getArtist();
	cout << strTest <<endl;

	strTest=testDoc.getAlbumName();
	cout << strTest <<endl;

	strTest=testDoc.getReference();
	cout << strTest <<endl;

	testvect=testDoc.getTrackTitle();
	for (it=testvect.begin();it!=testvect.end();it++)
		cout << (*it) <<endl;
	
	testvect=testDoc.getTrackPath();
	for (it=testvect.begin();it!=testvect.end();it++)
		cout << (*it) <<endl;
	
	
	
	return 0;


	//	testXml.LoadFile(<#FILE *#>, <#TiXmlEncoding encoding#>);
	std::string f_plugin, s_plugin, v_plugin;
	std::string smedia="text",build_type="Debug";
	
	for (int i=0;i<100;i++){
		MediaCycle *testMC=new MediaCycle(MEDIA_TYPE_TEXT);
        if(testMC->loadPluginLibraryFromBasename(smedia) == -1){
			return 0;
		}
        if(testMC->loadPluginLibraryFromBasename("visualisation") == -1){
			return 0;
		}
        if(testMC->loadPluginLibraryFromBasename("segmentation") == -1){
			
			return 0;
		}
		testMC->importDirectory("/Users/ravet/Desktop/MediaCycleData/extraitsArticles/",1);
		testMC->setClustersMethodPlugin("ACCosKMeans");
		testMC->setClustersPositionsPlugin("ACCosClustPosition");
		
		delete testMC;
		
		
	}
	
	
	string 	pathIndex=string("../../../../apps/textcycle-osg-qt/temp");
	//lucene::analysis::SimpleAnalyzer an;
	//lucene::analysis::StopAnalyzer an("/Users/ravet/Desktop/navimed/TMG_5.0R6/common_words - fr.txt");
	
	ACTextFeaturesPlugin* testPlugin=new ACTextFeaturesPlugin();
	for (int i=0;i<100;i++){
		vector<ACMedia*> testLib;
		ACMedia * testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 2.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 3.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 4.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 5.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		
		preProcessInfo testInfo=testPlugin->update(testLib);
		std:vector<ACMedia*>::iterator it;
		for (it=testLib.begin();it!=testLib.end();it++){
			(*it)->getAllPreProcFeaturesVectors()=testPlugin->apply(testInfo,(*it));
		}
		for (it=testLib.begin();it!=testLib.end();it++){
			cout <<(*it)->getFileName() <<endl;
	//		std::vector<float>::iterator it2;
	//		for (it2=(*it)->getAllPreProcFeaturesVectors()[0]->getFeaturesVector()->begin();it2!=(*it)->getAllPreProcFeaturesVectors()[0]->getFeaturesVector()->end();it2++)
	//			cout<<(*it2)<<endl;
		//	for (int i=0;i<(*it)->getAllPreProcFeaturesVectors()[0]->getFeaturesVector()->size();i++)
		//		cout << (*((*it)->getAllPreProcFeaturesVectors()[0]->getFeaturesVector()))[i]<<endl;;
		}
	//	std::vector<ACMediaFeatures*> testFeat=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		
		//delete testMedia;
		for (it=testLib.begin();it!=testLib.end();it++){
			delete (*it);
			(*it)=NULL;
		}
	//	sleep(1);
		
		std::cout<<i<<endl;
	}
	_lucene_shutdown(); //clears all static memory
	
	return 0;
	
	

}*/
