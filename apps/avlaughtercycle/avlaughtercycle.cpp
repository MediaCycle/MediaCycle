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

/*
 * AVLAUGHTERCYCLE - main file
 */
#include <stdlib.h>

#include "MediaCycle.h"

#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>

#define TI_MAX(x,y) ((x)>(y)?(x):(y))

//sleep()
#include <iostream>
#ifdef __MINGW32__
#include <windows.h>
#define sleep(n) Sleep(1000 * n)
#endif

using namespace std;

static void avlc_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData);
int processTcpMessageFromSSI(MediaCycle *that, char* buffer, int l, char **buffer_send, int *l_send);

int main(int argc, char** argv) {
	MediaCycle *mediacycle;

	cout << "new MediaCycle" << endl;
	mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO, "/home/alexis/Work/eNTERFACE/eNTERFACE09/JMUI/", "avlc-lib.acl");
	//mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/home/alexis/Work/eNTERFACE/eNTERFACE09/AVLC/test/","avlc-lib.acl");
	//mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/home/alexis/Work/eNTERFACE/eNTERFACE09/AVLC/","avlc-lib-20090806-1.acl");
	//mediacycle->addPluginLibrary("/home/alexis/Programmation/TiCore-app/Applications/Numediart/MediaCycle/src/Builds/linux-x86/plugins/eyesweb/mc_eyesweb.so");
	//mediacycle->addPluginLibrary("/home/alexis/Programmation/TiCore-app/Applications/Numediart/MediaCycle/src/Builds/linux-x86/plugins/greta/mc_greta.so");
	//mediacycle->addPluginLibrary("/home/alexis/Programmation/TiCore-app/Applications/Numediart/MediaCycle/src/Builds/linux-x86/plugins/audioanalysis/mc_audioanalysis.so");

	cout << "setCulsterN" << endl;
	mediacycle->getBrowser()->setClusterNumber(1);
	// XSCF251003 added this
	mediacycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
	// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
	

	//IMPORT DIRECTORY + SAVE IN LIBFILE
	/*cout<<"importDir"<<endl;
	mediacycle->importDirectory(mediacycle->getLocalDirectoryPath(),0);
	mediacycle->getLibrary()->saveAsLibrary(mediacycle->getLocalDirectoryPath() + "/" + mediacycle->getLibName());
	 */

	//IMPORT LIBFILE
	cout << "importLib" << endl;
	mediacycle->importACLLibrary(mediacycle->getLocalDirectoryPath() + mediacycle->getLibName()); // SD
	//mediacycle->importLibrary("/home/alexis/test-fscanf.acl");
	/*
	//TEST KNN WITH NEW FILE
	cout << "new media" << endl;
	ACPlugin *audioanalysis = mediacycle->getPluginManager()->getPlugin("AudioAnalysis");
	cout << "audioanalysis" << endl;
	 // XS TODO get data then calculate
	ACMediaFeatures *amf = audioanalysis->calculate("/home/alexis/Work/eNTERFACE/eNTERFACE09/AVLC/validation/alexis.wav");
	amf->dump();
	cout << "normalize" <<endl;
	for(int j=0; j<mediacycle->getLibrary()->getMeanFeatures().size(); j++) {
		for(int k=0; k<mediacycle->getLibrary()->getMeanFeatures()[j].size(); k++) {
			float old = amf->getFeature(k);
			cout << "(" << j << "," << k << ")" << old << " - " << mediacycle->getLibrary()->getMeanFeatures()[j][k] << "/" << mediacycle->getLibrary()->getStdevFeatures()[j][k] << endl;
			amf->setFeature (k, (old - mediacycle->getLibrary()->getMeanFeatures()[j][k]) / ( max(mediacycle->getLibrary()->getStdevFeatures()[j][k] , 0.00001)));
		}
	}
	amf->dump();
	cout << "creating media" << endl;
	ACMedia* local_media;
	local_media = ACMediaFactory::getInstance().create(MEDIA_TYPE_AUDIO);
	local_media->addFeatures(amf);
	cout << "done" << endl;
	cout<<"getKNN"<<endl;
	vector<ACMedia *> result;
	mediacycle->getKNN(local_media, result, 3);
	cout<<"done"<<endl;

	if (result.size() > 0) {
		ACPlugin *greta = mediacycle->getPluginManager()->getPlugin("Greta");
		//remove extension in greta
		if (greta) {
			greta->calculate(result[0]->getFileName());
			result[0]->getFeature(0)->dump();
		} else {
			for (int k=0;k<result.size();k++) {
				cout << result[k]->getFileName() << endl;
				result[k]->getFeature(0)->dump();
			}
		}
	}

	delete local_media;
	*/

	//test tcp-SSI-greta (all-in-one) = AVLaughterCycle
	//mediacycle->startTcpServer(12345,5,avlc_tcp_callback);

	//begin test greta
	/*
	ACPlugin *greta = mediacycle->getPluginManager()->getPlugin("Greta");

	FILE *f = fopen("/home/alexis/Work/eNTERFACE/eNTERFACE09/faplist.txt","r");

	char mystring [100];
	while(fgets(mystring,100,f)) {
		std::string filename = mystring;
		filename = filename.substr(0,filename.size()-1) + ".wav";
		cout << filename << endl;
		getchar();
		greta->calculate(filename);
	}
	fclose(f);
	//end test greta
	*/
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
                cout << "medias size : " << medias.size() << endl;
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

	/* Test KNN for JMUI */
	cout << "JMUI : get KNN" << endl;
	vector<ACMedia *> result;

	mediacycle->getBrowser()->setWeight(0, 1.0);
	mediacycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
	// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay

        std::vector<long> ids = mediacycle->getLibrary()->getAllMediaIds();
        for (int m=0;m<ids.size();m++) {
            mediacycle->getKNN(mediacycle->getLibrary()->getMedia(ids[m]), result, 11);
		if (result.size() > 0) {
			result[1]->getFeaturesVector(0)->dump();
                        cout << mediacycle->getLibrary()->getMedia(ids[m])->getFileName() << " ";
			for (int k=1;k<result.size();k++) {
				cout << result[k]->getFileName() << " ";
				//result[k]->getFeaturesVector(0)->dump();
			}
			cout << endl;

		} else {
			cout << "JMUI : no result from KNN" << endl;
		}
	}

	cout << endl;
	cout.flush();

	while (1) {
		sleep(30);
	}

	return (EXIT_SUCCESS);
}

static void avlc_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData) {
	MediaCycle *that = (MediaCycle*) userData;
	processTcpMessageFromSSI(that, buffer, l, buffer_send, l_send);
}

/*
 * data structure sent by SSI (AVLaughterCycle) :
 *
 *   <uint32> tot_size
 *   <uint32> type_size
 *   <char> x type_size --> type_name
 *
 *   if type_name == 'addwavf'
 *     <uint32> wav_size
 *     <byte> x wav_size --> content of wave file
 *   elif type_name == 'request'
 *     no wave file is sent
 *   fi
 *
 *   <uint32> burst_size
 *   <char> x burst_size --> sequence of burst labels
 *   <uint32> stat_size
 *   <float>  stat_size --> sequence of statistics
 */
int processTcpMessageFromSSI(MediaCycle *that, char *buffer, int l, char **buffer_send, int *l_send) {
	//AM : TODO rewrite Tcp in C++ to get something better than a char* buffer containing binary data ?
	// (and then rewrite code below)
	std::string file_name;

	unsigned long pos = 0;
	cout << "Processing TCP message of length" << l << endl;

	unsigned int tot_size = *reinterpret_cast<int*> (buffer + pos);
	pos += sizeof (int);
	cout << "Actual length : " << tot_size << endl;

	unsigned int type_size = *reinterpret_cast<int*> (buffer + pos);
	pos += sizeof (int);

	cout << "type_size : " << type_size << endl;

	std::string type_name(buffer + pos, type_size);
	pos += type_size;

	cout << "type_name : " << type_name << endl;

	//if a wave file is sent, we save it locally
	if (type_name == "addwavf") {
		cout << "SSI : add wave file" << endl;
		unsigned int name_size = *reinterpret_cast<int*> (buffer + pos);
		//extract filename
		pos += sizeof (int);

		cout << "name_size : " << name_size << endl;
		std::string tmp(buffer + pos, name_size);
		file_name = tmp;
		pos += name_size;

		cout << "file_name : " << file_name << endl;

		//extract wavefile
		unsigned int wav_size = *reinterpret_cast<int*> (buffer + pos);
		pos += sizeof (int);

		FILE *local_file;
		time_t timer;
		time(&timer);
		tmp = that->getLocalDirectoryPath() + "/" + file_name;
		local_file = fopen(tmp.c_str(), "wb");
		fwrite((void*) (buffer + pos), 1, wav_size, local_file);
		fclose(local_file);

		pos += wav_size;
	} else if (type_name == "request") {
		cout << "SSI : request" << endl;
		//nothing specific to do
	} else {
		//not a valid request
		return -1;
	}

	unsigned int burst_size = *reinterpret_cast<int*> (buffer + pos);
	pos += sizeof (int);

	cout << "burst_size : " << burst_size << endl;

	std::string burst_labels(buffer + pos, burst_size);
	pos += burst_size;

	cout << "burst_labels : " << burst_labels << endl;

	unsigned int stat_size = *reinterpret_cast<int*> (buffer + pos);
	pos += sizeof (int);

	cout << "stat_size : " << stat_size << endl;

	int nfeats = stat_size / sizeof (float);

	//float *ssi_features = reinterpret_cast<float*>(buffer+pos);
	float *ssi_features = new float[nfeats];

	for (int k=0; k<nfeats; k++) {
		ssi_features[k] = *reinterpret_cast<float*> (buffer + pos);
		//cout << "ssi_features (" << k << ") :" << ssi_features[k] << endl;
		pos += sizeof (float);
	}

	/*for (int k=0;k<nfeats;k++) {
		cout << dec << "ssi_features (" << k << ") :" << ssi_features[k] << endl;
		for (int j=0;j<4;j++) {
			int tmp = (int) *(buffer+pos+k*4+j);
			cout << tmp << " ";
		}
		cout << endl;
	}*/

	//TODO feature normalization if done when loading library
	cout << "creating features" << endl;
	ACMediaFeatures *mediaFeatures = new ACMediaFeatures();
	mediaFeatures->resize(nfeats);
	for (int i=0; i<nfeats; i++) {
		stringstream tmpstr;
		tmpstr << "ssi" << i;
		mediaFeatures->setName(tmpstr.str());
		mediaFeatures->setFeatureElement(i,ssi_features[i]);
	}
	mediaFeatures->setComputed();
	cout << "done" << endl;

	cout << "creating media" << endl;
	ACMedia* local_media;
	local_media = ACMediaFactory::getInstance().create(MEDIA_TYPE_AUDIO);
	local_media->addFeaturesVector(mediaFeatures);
	cout << "done" << endl;

	if (type_name == "addwavf") {
		cout << "addwavf - name : " << file_name << " - " << file_name.size() << endl;
		local_media->setFileName(file_name);
		cout << "addwavf - addmedia" << endl;
		that->getLibrary()->addMedia(local_media);
		cout << "addwavf - savelib" << endl;
		that->getLibrary()->saveACLLibrary(that->getLocalDirectoryPath() + "/" + that->getLibName());
		cout << "done" << endl;
	} else if (type_name == "request") {
		cout << "normalize" << endl;
		cout << "before" << endl;
		mediaFeatures->dump();
		for (int j=0; j<that->getLibrary()->getMeanFeatures().size(); j++) {
			for (int k=0; k<that->getLibrary()->getMeanFeatures()[j].size(); k++) {
				float old = mediaFeatures->getFeatureElement(k);
				//cout << "(" << j << "," << k << ")" << old << " - " << that->getLibrary()->getMeanFeatures()[j][k] << "/" << that->getLibrary()->getStdevFeatures()[j][k] << endl;
				mediaFeatures->setFeatureElement(k, (old - that->getLibrary()->getMeanFeatures()[j][k]) / (max(that->getLibrary()->getStdevFeatures()[j][k], 0.00001)));
			}
		}
		/*cout << endl << "after" << endl;
		local_media->getFeaturesVector(0)->dump(); // XS ou getFeatures ?
		cout << endl;*/

		vector<ACMedia *> result;
		that->getKNN(local_media, result, 1);

		if (result.size() > 0) {
			ACFeaturesPlugin *greta = dynamic_cast<ACFeaturesPlugin*>( that->getPluginManager()->getPlugin("Greta"));
			if (greta) {
				//filename extension is removed in greta
				cout << "Sent to Greta : " << result[0]->getFileName() << endl;
				//XS TODO getData
				//greta->calculate(result[0]->getFileName());
			} else {
				cout << "Greta plugin not found, displaying results here ..." << endl;
				cout << "*** REQUEST DUMP : " << endl;
				mediaFeatures->dump();

				for (int k=0; k < result.size(); k++) {
					cout << "result (" << k << ") : " << result[k]->getFileName() << endl;
					result[k]->getFeaturesVector(0)->dump();
				}
			}
		}
		cout << endl << endl;
		//delete only in "request". In "addwavf", local_media is added to the library and therefore shouyld not be deleted
		delete local_media;
	} else {
		//not valid (should not happen since already checked before)
		return -1;
	}

	//cout << "BUFFER : " << buffer << endl << endl;
	return 0;
}
