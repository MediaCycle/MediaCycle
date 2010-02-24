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

using namespace std;

int main(int argc, char** argv) {
    MediaCycle *mediacycle;
    //string libpath("/home/alexis/NetBeansProjects/MediaCycle/lib/Caltech101-a.acl");

    cout<<"new MediaCycle"<<endl;
    mediacycle = new MediaCycle(MEDIA_TYPE_VIDEO,"/home/alexis/NetBeansProjects/MediaCycle/lib/","mainlib.acl");
    //mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/home/alexis/NetBeansProjects/MediaCycle/lib/","mainlib.acl");
    //    mediacycle->addPlugin("/home/alexis/Programmation/TiCore-app/Applications/Numediart/MediaCycle/src/Builds/linux-x86/plugins/eyesweb/mc_eyesweb.so");
    mediacycle->addPlugin ("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-xcode/plugins/eyesweb/Debug/mc_eyesweb.dylib");
	//	("/Users/dtardieu/src/Numediart/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/eyesweb/Debug/mc_eyesweb.dylib");
    //    mediacycle->importDirectory("/Users/xavier/numediart/Project7.3-DancersCycle/Recordings_Raffinerie_0709/FrontShots/H264_subset/",0);
	//	("/Users/dtardieu/data/DANCERS/Video/Front/",0);
	
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
