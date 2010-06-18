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
		
		mediacycle->addPlugin("../../plugins/audio/" + build_type + "/mc_audio.dylib");
		mediacycle->addPlugin("../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib");
	
		//		mediacycle->importDirectory("/Users/dtardieu/data/AudioCycleProPackTest/zero-g-pro-pack_b/Super Funk/Funkmachine-E/", 1);
		//mediacycle->importDirectory("/Users/dtardieu/data/test/testEnv/",1);
		//		mediacycle->importDirectory("/Users/dtardieu/data/rire-audiocycle/",1);
		//		mediacycle->importDirectory("/Users/dtardieu/data/footsteps/", 1);
		mediacycle->importDirectory("/Users/dtardieu/data/AudioGarden/test123",1);
		//mediacycle->saveMCSLLibrary("/Users/dtardieu/data/AudioCycleProPackTest/zero-g-pro-pack_b/Super Funk/Funkmachine-E.acl");
		//mediacycle->saveACLLibrary("/Users/dtardieu/data/test/testEnv/testEnv.acl");

		//mediacycle->importMCSLLibrary("/Users/dtardieu/data/AudioCycleProPackTest/zero-g-pro-pack_b/Super Funk/Funkmachine-E.acl");

		//		mediacycle->saveMCSLLibrary("/Users/dtardieu/data/AudioCycleProPackTest/zero-g-pro-pack_b/Super Funk/Funkmachine-E.mcsl");
		//		mediacycle->saveMCSLLibrary("/Users/dtardieu/data/rire-audiocycle/rire-audiocycle.mcsl");
		//		mediacycle->saveMCSLLibrary("/Users/dtardieu/data/footsteps/footsteps.mcsl");
		mediacycle->saveMCSLLibrary("/Users/dtardieu/data/AudioGarden/test123/test123.mcsl");

		for (int i = 0; i < mediacycle->getLibrary()->getSize(); i++){
			std::cout << i << " : " << mediacycle->getLibrary()->getMedia(i)->getFileName() << "   " << ((ACAudio*)mediacycle->getLibrary()->getMedia(i))->getNFrames() << std::endl;
		}

		vector<long> grainIds;
		for (int i=31; i<84; i++){
			//for (int i=1; i<20; i++){
			grainIds.push_back(i);
		}
		float* syn_v;
		long length;
		AGSynthesis(mediacycle->getLibrary(), 0, grainIds, &syn_v, length);
		
		SF_INFO sfinfo;
		SNDFILE* testFile;
		sfinfo.samplerate = 44100;
		sfinfo.channels = 1;
		sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		
		if (! (testFile = sf_open ("synthesis.wav", SFM_WRITE, &sfinfo))){  
			printf ("Not able to open input file %s.\n", "synthesis.wav") ;
			puts (sf_strerror (NULL)) ;
			return 1;
		}
		
		float tt[1];
		tt[0] =1.0;
		sf_writef_float  (testFile, syn_v, length);
		sf_close(testFile);

// 	cout<<"setCulsterN"<<endl;
//     mediacycle->getBrowser()->setClusterNumber(1);
//     cout<<"importLib"<<endl;
//     mediacycle->importLibrary("/home/alexis/NetBeansProjects/MediaCycle/lib/LClib.acl");
//     cout<<"done"<<endl;

//     mediacycle->startTcpServer(12345,5);

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

    vector<ACMedia*> loops = mediacycle->getLibrary()->getAllMedia();
    if (loops.size() > 0) {
        cout << "loopssize : " << loops.size() << endl;
        cout << "filename : " << loops[1]->getFileName() << endl;
        for(int y=0; y<loops.size(); y++)
        {
            for (k=0;k<ids.size();k++) {
                if (loops[y]->getId() == ids[k]) {
                    cout << "filename : " << loops[y]->getFileName() << endl;
                }
            }
            //Should output 1 4 8
        }
    }

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
    vector<ACMedia*> loops = mediacycle->getLibrary()->getAllMedia();
    mediacycle->getBrowser()->getKNN(loops[NN]->getId(),similar,2);

    cout << "similar : " << similar[0] << " & " << similar[1] << endl;
    cout.flush();
    cout << "filename : " << loops[NN]->getFileName() << endl;
    for(int y=0; y<loops.size(); y++)
    {
        if (loops[y]->getId() == similar[0] || loops[y]->getId() == similar[1])
        cout << "filename : " << loops[y]->getFileName() << endl;
        //Should output 1 4 8
    }


    mediacycle->getLibrary()->saveAsLibrary("test.mcl");
/*    sleep(30);
    network_socket->stop();
    delete network_socket;
*/
    cout << endl;
    cout.flush();
  
    while(1) {
        sleep(30);
    }
    
    return (EXIT_SUCCESS);
}
