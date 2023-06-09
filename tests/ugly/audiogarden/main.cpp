/* 
 * File:   main.cpp
 * Author: Alexis Moinet
 *
 * @date 15 juillet 2009
 * @copyright (c) 2009 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#include <stdlib.h>

#include "MediaCycle.h"

#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include "ACAudio.h"
#include "AGSynthesis.h"
#include "sndfile.h"

using namespace std;

int main(int argc, char** argv) {

    MediaCycle *mediacycle;
    //string libpath("/home/alexis/NetBeansProjects/MediaCycle/lib/Caltech101-a.acl");

    cout<<"new MediaCycle"<<endl;
		mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");

		std::string build_type ("Release");
#ifdef USE_DEBUG
		build_type = "Debug";
#endif
		
		mediacycle->addPluginLibrary("../../../plugins/audio/" + build_type + "/mc_audio.dylib");
		mediacycle->addPluginLibrary("../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib");
	
		//mediacycle->importDirectory("/Users/dtardieu/data/AudioCycleProPackTest/zero-g-pro-pack_b/Super Funk/Funkmachine-E/", 1);
		//mediacycle->importDirectory("/Users/dtardieu/data/test/testEnv/",1);
		//		mediacycle->importDirectory("/Users/dtardieu/data/rire-audiocycle/",1);
		//		mediacycle->importDirectory("/Users/dtardieu/data/footsteps/", 1);
		//mediacycle->importDirectory("/Users/dtardieu/data/AudioGarden/audiogarden/sounds2",1);
		//mediacycle->saveMCSLLibrary("/Users/dtardieu/data/AudioCycleProPackTest/zero-g-pro-pack_b/Super Funk/Funkmachine-E.acl");
		//mediacycle->saveACLLibrary("/Users/dtardieu/data/test/testEnv/testEnv.acl");

		//mediacycle->importMCSLLibrary("/Users/ccl/Dropbox/Numediart/audiogarden/soundsExt.mcsl");

		mediacycle->importMCSLLibrary("/Users/dtardieu/data/AudioGarden/potpourri.mcsl");
		//		mediacycle->saveMCSLLibrary("/Users/dtardieu/data/rire-audiocycle/rire-audiocycle.mcsl");
		//		mediacycle->saveMCSLLibrary("/Users/dtardieu/data/footsteps/footsteps.mcsl");
		//mediacycle->saveMCSLLibrary("/Users/dtardieu/data/AudioGarden/audiogarden/audiogarden.mcsl");

                std::vector<long> ids = mediacycle->getLibrary()->getAllMediaIds();
                for (int i = 0; i < ids.size(); i++){
                        std::cout << i << " : " << mediacycle->getLibrary()->getMedia(ids[i])->getFileName() << "   " << ((ACAudio*)mediacycle->getLibrary()->getMedia(ids[i]))->getNFrames() << std::endl;
		}

		vector<long> grainIds;
		for (int i=38; i<39; i++){
			grainIds.push_back(i);
		}

		AGSynthesis* synth = new AGSynthesis();
		synth->setMediaCycle(mediacycle);
		synth->compute(37, grainIds);
		synth->saveAsWav("./synthesis.wav");
	
// 	cout<<"setCulsterN"<<endl;
//     mediacycle->getBrowser()->setClusterNumber(1);
//     cout<<"importLib"<<endl;
//     mediacycle->importLibrary("/home/alexis/NetBeansProjects/MediaCycle/lib/LClib.acl");
//     cout<<"done"<<endl;

//     mediacycle->startTcpServer(12345,5);

	// XS note: if you uncomment this, remove media id from  call (= last argument)
    /*mediacycle->importDirectory("/home/alexis/NetBeansProjects/MediaCycle/lib/b50aac6a76bf5d5b660dd822273fe58af8791131.wav",0,1);
    mediacycle->importDirectory("/home/alexis/NetBeansProjects/MediaCycle/lib/18f038431e4db3c83c7227f47966cbbe7d6e467d.wav",0,2);
    mediacycle->importDirectory("/home/alexis/NetBeansProjects/MediaCycle/lib/d2dd27046e2a241e06d48a22fd1bc4183e7fa990.wav",0,3);
    mediacycle->importDirectory("/home/alexis/NetBeansProjects/MediaCycle/lib/c2ea562d07ce786935d278e0bd59cdb2b1948c6d.wav",0,4);
    mediacycle->importDirectory("/home/alexis/NetBeansProjects/MediaCycle/lib/2a28aa910897bc86e243f6f18920b5cc8faa2249.wav",0,5);
*/
    /*
    vector<int> ids;
    ids.resize(0);

    mediacycle->getKNN(1, ids, 2);
    cout << "size : " << ids.size() << endl;
    cout.flush();

    int k;
    for (k=0;k<ids.size();k++)
        cout << "similar : " << ids[k] << endl;
    cout.flush();

    vector<ACMedia*> medias = mediacycle->getLibrary()->getAllMedia();
    if (medias.size() > 0) {
        cout << "mediassize : " << medias.size() << endl;
        cout << "filename : " << medias[1]->getFileName() << endl;
        for(int y=0; y<medias.size(); y++)
        {
            for (k=0;k<ids.size();k++) {
                if (medias[y]->getId() == ids[k]) {
                    cout << "filename : " << medias[y]->getFileName() << endl;
                }
            }
            //Should output 1 4 8
        }
    }
*/
/*
    ACMediaLibrary *medialib;
    ACMediaBrowser *mediabrowser;

    std::string libpath("/home/alexis/NetBeansProjects/MediaCycle/lib/Caltech101-a.acl");

    int ret;
    char *retc;
    fpos_t pos;

    int width, height, n_features;

    medialib = new ACMediaLibrary();
    medialib->setMediaType(IMAGE);   
    medialib->openLibrary(libpath);

    mediabrowser = new ACMediaBrowser();
    mediabrowser->libraryContentChanged(); 
    mediabrowser->setClusterNumber(4);
	// XSCF251003 added this
	 mediacycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
	 // XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
	//
    mediabrowser->setLibrary(medialib);
    mediabrowser->libraryContentChanged(); 
*/
/*    int NN = 500;
    int similar[2];
    vector<ACMedia*> medias = mediacycle->getLibrary()->getAllMedia();
    mediacycle->getBrowser()->getKNN(medias[NN]->getId(),similar,2);

    cout << "similar : " << similar[0] << " & " << similar[1] << endl;
    cout.flush();
    cout << "filename : " << medias[NN]->getFileName() << endl;
    for(int y=0; y<medias.size(); y++)
    {
        if (medias[y]->getId() == similar[0] || medias[y]->getId() == similar[1])
        cout << "filename : " << medias[y]->getFileName() << endl;
        //Should output 1 4 8
    }


    mediacycle->getLibrary()->saveAsLibrary("test.mcl");
*/
/*    sleep(30);
    network_socket->stop();
    delete network_socket;
*/
    cout << endl;
    cout.flush();
  
    
    return (EXIT_SUCCESS);
}
