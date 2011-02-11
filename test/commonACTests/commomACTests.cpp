/*
 *  commomACTests.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 16/03/10
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

#include "MediaCycle.h"
#include "tinyxml.h"

#include <iostream>
#include <string>
using namespace std;

void test_audio_library(string file_name){
	
	cout << "-------------------------------------------------------" << endl;
	cout << "Testing creating mediacycle AUDIO" << endl;
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");

	cout << "-------------------------------------------------------" << endl;
	cout << "Test Import Audio Library" << endl;
	media_cycle->importACLLibrary(file_name);
	cout << "Library Size = " << media_cycle->getLibrarySize() << endl;

//	cout << "-------------------------------------------------------" << endl;
//	cout << "Test Writing ACL file (new CPP version): _testnew_"+file_name << endl;
//	media_cycle->saveACLLibrary("_testnew_"+file_name);
//	cout << "-------------------------------------------------------" << endl;

	//XS TODO : this is tmp testing
	cout << "-------------------------------------------------------" << endl;
	cout << "Test Writing XML file /Users/xavier/tmp/xmlTest.xml" << endl;
	media_cycle->getLibrary()->saveXMLLibrary("/Users/xavier/tmp/xmlTest.xml");
	cout << "-------------------------------------------------------" << endl;
	cout << "Test Loading XML file /Users/xavier/tmp/xmlTest.xml" << endl;
	media_cycle->getLibrary()->openXMLLibrary("/Users/xavier/tmp/xmlTest.xml");
	cout << "-------------------------------------------------------" << endl;
}

void test_audio_visualisation_plugins(){
	cout << "Testing creating mediacycle AUDIO" << endl;
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");

	cout << "-------------------------------------------------------" << endl;
	cout << "Test Plugins" << endl;
	std::string build_type ("Release");
	build_type = "Debug";
	media_cycle->addPlugin("../../../plugins/audio/" + build_type + "/mc_audiofeatures.dylib");	
	media_cycle->addPlugin("../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
	media_cycle->dumpPluginsList();

	delete media_cycle;
}

void test_audio_config_file(){
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	media_cycle->readConfigFile("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/mediacycle.config2");
//	media_cycle->dumpConfigFile();
	delete media_cycle;
}

void test_image_config_file(){
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	media_cycle->readConfigFile("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/mediacycle.config2");
	//	media_cycle->dumpConfigFile();
	delete media_cycle;
}

void test_XML(){
	const char* demoStart =
	"<?xml version=\"1.0\"  standalone='no' >\n"
	"<MediaCycle>\n"
    "<NumberOfMedia>2</NumberOfMedia>\n"
    "<Medias>\n"
	"<Media MediaType=\"2\" FileName=\"/usr/local/share/mediacycle/data/images/obj100__0100.png\" MediaID=\"1\" NumberOfFeatures=\"3\">\n"
	"<Feature FeatureName=\"Color\" NeedsNormalization=\"1\" NumberOfFeatureElements=\"12\">78.6384 17.3848 6.02063 58.8089 14.0491 7.52038 0.226132 1.26305 5.54078 2.05873 4.33935 42.2579 </Feature>\n"
	"<Feature FeatureName=\"Shape\" NeedsNormalization=\"1\" NumberOfFeatureElements=\"7\">-2.22995 -7.32801 -8.38812 -8.20365 -16.5016 -12.4563 -17.5089 </Feature>\n"
	"<Feature FeatureName=\"Texture\" NeedsNormalization=\"1\" NumberOfFeatureElements=\"70\">0.548651 0.557809 0.947574 1.25489 1.61763 2.28575 2.53847 2.65199 3.37621 2.43304 0.442217 0.560494 0.688245 0.869465 1.0184 1.45404 1.27505 1.01416 1.74571 1.25372 0.943836 1.1016 0.793647 1.26042 1.15282 2.02965 1.10004 1.17679 1.41099 1.05961 0.808844 1.27964 1.02299 1.42824 1.21304 1.25937 1.32587 1.13106 1.78762 1.08561 1.65357 2.673 2.18582 2.40749 2.74802 2.67446 3.81013 3.20498 4.63338 3.00419 0.878644 2.02471 1.01617 1.92956 0.9284 1.04338 1.00564 0.935684 1.60673 1.01675 1.02893 1.19235 0.804547 1.27018 1.22951 1.75586 1.3749 1.32093 2.28436 1.98965 </Feature>\n"
	"</Media>\n"
	"<Media MediaType=\"3\" FileName=\"/usr/local/share/mediacycle/data/images/obj100__0100.mov\" MediaID=\"2\" NumberOfFeatures=\"3\">\n"
	"<Feature FeatureName=\"Color\" NeedsNormalization=\"1\" NumberOfFeatureElements=\"12\">78.6384 17.3848 6.02063 58.8089 14.0491 7.52038 0.226132 1.26305 5.54078 2.05873 4.33935 42.2579 </Feature>\n"
	"<Feature FeatureName=\"Shape\" NeedsNormalization=\"1\" NumberOfFeatureElements=\"7\">-2.22995 -7.32801 -8.38812 -8.20365 -16.5016 -12.4563 -17.5089 </Feature>\n"
	"<Feature FeatureName=\"Texture\" NeedsNormalization=\"1\" NumberOfFeatureElements=\"70\">0.548651 0.557809 0.947574 1.25489 1.61763 2.28575 2.53847 2.65199 3.37621 2.43304 0.442217 0.560494 0.688245 0.869465 1.0184 1.45404 1.27505 1.01416 1.74571 1.25372 0.943836 1.1016 0.793647 1.26042 1.15282 2.02965 1.10004 1.17679 1.41099 1.05961 0.808844 1.27964 1.02299 1.42824 1.21304 1.25937 1.32587 1.13106 1.78762 1.08561 1.65357 2.673 2.18582 2.40749 2.74802 2.67446 3.81013 3.20498 4.63338 3.00419 0.878644 2.02471 1.01617 1.92956 0.9284 1.04338 1.00564 0.935684 1.60673 1.01675 1.02893 1.19235 0.804547 1.27018 1.22951 1.75586 1.3749 1.32093 2.28436 1.98965 </Feature>\n"
	"</Media>\n"
	"</Medias>\n"
	"</MediaCycle>\n";
	
	string s(demoStart);
	cout << s << endl;
	
	TiXmlDocument doc( "/Users/xavier/tmp/demotest.xml" );
	doc.Parse( demoStart );
	
	if ( doc.Error() )
	{
		printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
		exit( 1 );
	}
	doc.SaveFile();
	doc.Print( stdout );
	
	TiXmlNode* rootNode = 0;
	TiXmlNode* node = 0;
	TiXmlElement* mediaElement = 0;
	TiXmlElement* itemElement = 0;
	
	
	// --------------------------------------------------------
	// An example of changing existing attributes, and removing
	// an element from the document.
	// --------------------------------------------------------
	
	// Get the "ToDo" element.
	// It is a child of the document, and can be selected by name.
	rootNode = doc.FirstChild( "MediaCycle" );
	assert( rootNode );
	mediaElement = rootNode->ToElement();
	assert( mediaElement  );
//	
//	// Going to the toy store is now our second priority...
//	// So set the "priority" attribute of the first item in the list.
	node = mediaElement->FirstChild("Medias");	
	assert( node );
	itemElement = node->ToElement();
	assert( itemElement  );
	
	node = itemElement->FirstChild("Media");
	assert( node );
	mediaElement = node->ToElement();
	assert( mediaElement  );	
	mediaElement->SetAttribute( "priority", 2 );
	
	// --------------------------------------------------------
	// An example of using handles
	// --------------------------------------------------------
	
	TiXmlHandle docHandle( &doc );
	TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );
	TiXmlText* nMediaText=rootHandle.FirstChild( "NumberOfMedia" ).FirstChild().Text();
	string nof = nMediaText->ValueStr();
	std::stringstream tmp;
	int n_loops;
	tmp << nof;
	tmp >> n_loops;
	
	TiXmlElement* pMediaNode=rootHandle.FirstChild( "Medias" ).FirstChild().Element();
	for( pMediaNode; pMediaNode; pMediaNode=pMediaNode->NextSiblingElement()) {
		ACMediaType typ;
		int typi=0;
		pMediaNode->QueryIntAttribute("MediaType", &typi);
		typ = (ACMediaType) typi;
		ACMedia* local_media = ACMediaFactory::create(typ);
		local_media->loadXML(pMediaNode);
	}

	
//	
//	// Change the distance to "doing bills" from
//	// "none" to "here". It's the next sibling element.
//	itemElement = itemElement->NextSiblingElement();
//	assert( itemElement );
//	itemElement->SetAttribute( "distance", "here" );
//	
//	// Remove the "Look for Evil Dinosaurs!" item.
//	// It is 1 more sibling away. We ask the parent to remove
//	// a particular child.
//	itemElement = itemElement->NextSiblingElement();
//	todoElement->RemoveChild( itemElement );
//	
//	itemElement = 0;
//	
//	// --------------------------------------------------------
//	// What follows is an example of created elements and text
//	// nodes and adding them to the document.
//	// --------------------------------------------------------
//	
//	// Add some meetings.
//	TiXmlElement item( "Item" );
//	item.SetAttribute( "priority", "1" );
//	item.SetAttribute( "distance", "far" );
//	
//	TiXmlText text( "Talk to:" );
//	
//	TiXmlElement meeting1( "Meeting" );
//	meeting1.SetAttribute( "where", "School" );
//	
//	TiXmlElement meeting2( "Meeting" );
//	meeting2.SetAttribute( "where", "Lunch" );
//	
//	TiXmlElement attendee1( "Attendee" );
//	attendee1.SetAttribute( "name", "Marple" );
//	attendee1.SetAttribute( "position", "teacher" );
//	
//	TiXmlElement attendee2( "Attendee" );
//	attendee2.SetAttribute( "name", "Voel" );
//	attendee2.SetAttribute( "position", "counselor" );
//	
//	// Assemble the nodes we've created:
//	meeting1.InsertEndChild( attendee1 );
//	meeting1.InsertEndChild( attendee2 );
//	
//	item.InsertEndChild( text );
//	item.InsertEndChild( meeting1 );
//	item.InsertEndChild( meeting2 );
//	
//	// And add the node to the existing list after the first child.
//	node = todoElement->FirstChild( "Item" );
//	assert( node );
//	itemElement = node->ToElement();
//	assert( itemElement );
//	
//	todoElement->InsertAfterChild( itemElement, item );
//	
	printf( "\n** Demo doc processed: ** \n\n" );
	doc.Print( stdout );	
}

int main(int, char **) {
	cout << "tests" << endl;
//	test_audio_library("/usr/local/share/mediacycle/data/audio/zero-g-pro-pack-small-mc.acl");
	test_XML();
}