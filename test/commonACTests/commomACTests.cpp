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

	cout << "-------------------------------------------------------" << endl;
	cout << "Test Writing ACL file (old C version)" << endl;
	media_cycle->saveACLLibrary("_testold_"+file_name);		// SD ?

	cout << "-------------------------------------------------------" << endl;
	cout << "Test Writing ACL file (new CPP version): _testnew_"+file_name << endl;
	media_cycle->saveACLLibrary("_testnew_"+file_name);
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

void test_config_file(){
	MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	media_cycle->readConfigFile("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/mediacycle.config2");
//	media_cycle->dumpConfigFile();
	delete media_cycle;
}

int main(int, char **) {
	cout << "tests" << endl;
	//test_audio_library("/usr/local/share/mediacycle/audio/test.acl");
	test_config_file();
}