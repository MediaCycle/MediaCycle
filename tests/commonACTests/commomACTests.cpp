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

bool test_audio_library(string file_name){
	bool ok = true;
	
	cout << "------------  Testing Audio Library  ------------------" << endl;
	cout << "... creating mediacycle Audio" << endl;
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	
	cout << "... importing XML Audio Library" << endl;
	media_cycle->importXMLLibrary(file_name);
	int library_size = media_cycle->getLibrarySize();
	if (library_size == 0) ok = false;
	
	string test_file = "testnew_"+ file_name ;
	cout << "... writing XML file " << test_file << endl;

	media_cycle->saveXMLLibrary(test_file);

	cout << "... cleaning everything" << endl;
	media_cycle->clean();

	cout << "... importing new XML Audio Library file" << endl;
	media_cycle->importXMLLibrary(test_file);
	int library_size_new = media_cycle->getLibrarySize();
	if (library_size_new == 0) ok = false;

	ok = (library_size == library_size_new);
	return ok;
}

bool test_features_visualisation_plugins(){
	bool ok = true;
	cout << "------------  Testing Plugins Library  ----------------" << endl;
	cout << "... creating mediacycle Audio" << endl;
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	std::string build_type = "Debug";
	
	cout << "... importing audio features plugins Library" << endl;
	media_cycle->addPluginLibrary("../../../plugins/audio/" + build_type + "/mc_audiofeatures.dylib");
	media_cycle->dumpPluginsList();
	
	cout << "... removing audio features plugins Library" << endl;
	media_cycle->removePluginLibrary("../../../plugins/audio/" + build_type + "/mc_audiofeatures.dylib");
	if (media_cycle->getPluginManager()->getSize() != 0) ok = false;

	cout << "... importing visualisation plugins Library" << endl;
	media_cycle->addPluginLibrary("../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
	media_cycle->dumpPluginsList();
	
	cout << "... removing visualisation plugins Library" << endl;
	media_cycle->removePluginLibrary("../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
	if (media_cycle->getPluginManager()->getSize() != 0) ok = false;
	delete media_cycle;
	return ok;
}

void test_image_config_file(string xml_file_name){
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	media_cycle->readXMLConfigFile(xml_file_name);
	//	media_cycle->dumpConfigFile();
	delete media_cycle;
}

void test_multiple_XML(){
	MediaCycle *mediacycle;
	
    std::string build_type ("Release");
#ifdef USE_DEBUG
    build_type = "Debug";
#endif
    
    mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO, "/tmp/", "mediacycle.acl");
    mediacycle->addPluginLibrary("../../../plugins/audio/" + build_type + "/mc_audio.dylib");
	
    mediacycle->addPluginLibrary("../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib");
    mediacycle->addPluginLibrary("../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
	
    mediacycle->importDirectory("/usr/local/share/mediacycle/data/audio_all/zero-g-pro-pack_b/Live Bass Grooves/Cool Grooves/Cool 100 1-E.wav", 0);
    mediacycle->saveXMLLibrary("/Users/xavier/tmp/afac.xml");
    mediacycle->importDirectory("/usr/local/share/mediacycle/data/audio_all/zero-g-pro-pack_b/Live Bass Grooves/Cool Grooves/Cool 100 3-G.wav", 0);
    mediacycle->saveXMLLibrary("/Users/xavier/tmp/afac.xml");
	
}

int main(int, char **) {
	cout << "tests" << endl;
	//test_audio_library("/usr/local/share/mediacycle/data/audio/zero-g-pro-pack-small-mc.xml");
	test_multiple_XML();

}
