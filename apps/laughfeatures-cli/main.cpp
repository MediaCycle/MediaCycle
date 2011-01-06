/**
 * @brief main.cpp
 * @author Jerome Urbain
 * @date 06/01/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include <stdio.h>
#include <sndfile.h>
#include <string>
#include <iostream>
#include "ACAudioFeatures.h"
#include "ACMediaTimedFeature.h"
#include <unistd.h>
#include <sys/stat.h>
#include "ACAudio.h"
#include "ACMediaData.h"
#include "ACAudioSegmentationPlugin.h"

void usage(string myname){        
	cerr << endl;
	cerr << "A utility for feature extraction from audio files." << endl;
	//	cerr << "Damien Tardieu -  TCTS/UMONS, Mons, Belgium" << endl;
	cerr << "Copyright 2010-2011 University of Mons." << endl;
	cerr << endl;
	cerr << "  Usage: " << myname
			 << " [OPTION] audiofile" << endl;
	cerr << endl;
	cerr << "  -L       Size of the analysis window, must be a power of two [default: 256]."<< endl;
	cerr << "  -c       Number of MFCC channels [default: 16]." << endl;
	cerr << "  -m       Number of MFCC [default: 13]." << endl;
	cerr << "  -o       Directory for storing feature files [default \"audiofile directory/audiofile.desc/\"]" << std::endl;
	cerr << "  -e       Extend sound limits for computation" << std::endl;
	cerr << endl;
	cerr << endl;
	
}

std::string descAbbreviation(std::string descName){
	std::string abbrev;
	if (descName == "Spectral Centroid"){
		abbrev = "sc";
	}	
	if (descName == "Spectral Spread"){
		abbrev = "ss";
	}	
	if (descName == "Spectral Flatness"){
		abbrev = "sfm";
	}	
	if (descName == "Spectral Variation"){
		abbrev = "sv";
	}	
	if (descName == "Spectral Flux"){
		abbrev = "sf";
	}	
	if (descName == "Zero Crossing Rate"){
		abbrev = "zcr";
	}	
	if (descName == "MFCC"){
		abbrev = "mfcc";
	}	
	if (descName == "Delta MFCC"){
		abbrev = "dmfcc";
	}	
	if (descName == "Delta Delta MFCC"){
		abbrev = "ddmfcc";
	}	
	if (descName == "Spectral Decrease"){
		abbrev = "sd";
	}	
	if (descName == "Energy"){
		abbrev = "ener";
	}	
	if (descName == "Loudness"){
		abbrev = "loud";
	}	
	if (descName == "Energy Modulation Amplitude"){
		abbrev = "ema";
	}
	if (descName == "Energy Modulation Frequency"){
		abbrev = "emf";
	}
        if (descName == "Burst Segmentation"){
		abbrev = "bs";
	}
	return abbrev;
}

int main(int argc, char** argv){
	std::string filename;
	std::string outDir;
	SF_INFO sfinfo;
	SNDFILE* testFile;
	float* data;

	int bflag, ch, fd;
	int mfccNbChannels = 32;
	int mfccNb = 13;
	int windowSize = 512; 	
	bool extendSoundLimits = false;
	
	string myname(argv[0]);

	bflag = 0;
	while ((ch = getopt(argc, argv, "heL:c:m:o:")) != -1) {
		switch (ch) {
		case 'e' :
			std::cout << "Extend sound limits" << std::endl;
			extendSoundLimits = true;
			break;	
		case 'L':
			windowSize = atoi(optarg);
			break;
		case 'c': 
			mfccNbChannels = atoi(optarg);
			break;
		case 'm':
			mfccNb = atoi(optarg);
			break;
		case 'o':
			outDir += optarg;
			break;
		case 'h':
			usage(myname);
			exit(1); 
			break;
		case '?':
			usage(myname);
			exit(1); 
			break;
		default:
			usage(myname);
			exit(1);
			break;
		}
	}
	
// 	for (int index = optind; index < argc; index++)
// 		printf ("Non-option argument %s\n", argv[index]);
	if (optind >= argc) {
		fprintf(stderr, "Expected filename after options\n");
		usage(myname);
		exit(1);
	}
	filename += argv[optind];
// 	filename = "/Users/dtardieu/data/Sol-v2-Damien/Sounds/Bb-Clarinet/flatterzunge/samples/BbCl-flatt-A#3-ff.aif";
// 	//	filename = "/Users/dtardieu/data/Sol-v2-Damien/Sounds/Violin/pizzicato-secco/samples/Vn-pizz-sec-A#3-ff-4c.aif";
// 	filename = "/Users/dtardieu/data/Sol-v2-Damien/Sounds/Bb-Clarinet/ordinario/samples/BbCl-ord-A#3-ff.aif";
	
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "Sound file : " << filename << std::endl;
	std::cout << "Output directory : " << outDir << std::endl;
	std::cout << "Window size : " << windowSize << std::endl;
	std::cout << "Number of MFCC channels : " << mfccNbChannels << std::endl;
	std::cout << "Number of MFCC : " << mfccNb << std::endl;
	
	if (! (testFile = sf_open (filename.c_str(), SFM_READ, &sfinfo))){  
		/* Open failed so print an error message. */
		printf ("Not able to open input file %s\n", filename.c_str()) ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
		return  1 ;
	}
	
	std::vector<ACMediaTimedFeature*> desc;
	std::cout << "Length : " << sfinfo.frames << std::endl;
	std::cout << "Sampling Rate : " << sfinfo.samplerate << std::endl;
	std::cout << "Channels : " << sfinfo.channels << std::endl;
	data = new float[(long) sfinfo.frames*sfinfo.channels];
	std::cout << "Read " << sf_read_float(testFile, data, sfinfo.frames*sfinfo.channels) << " frames" << std::endl;
	desc = computeFeatures(data, sfinfo.samplerate, sfinfo.channels, sfinfo.frames, mfccNbChannels, mfccNb, windowSize, 	extendSoundLimits);

        /*ACMediaData ACMD;
        ACMD.setAudioData(data);
        ACAudio ACM;
        ACM.import(filename);
        ACAudioSegmentationPlugin r;
        ACAudioSegmentationPlugin ACASP;
        ACASP.segment(&ACMD, &ACM);*/



	std::string descFileName;
	int posSep = filename.find_last_of("/\\");
	int posDot = filename.find_last_of(".");
	
	std::string rootFileName;
	std::string soundDir;
	if (posSep == -1){
		rootFileName = filename.substr(0, posDot);
		soundDir = ".";
	}
	else{
		rootFileName = filename.substr(posSep+1, posDot-posSep-1);
		soundDir = filename.substr(0, posSep);
	}

	std::string descDir;
	if (outDir.size()==0){
		descDir = soundDir + "/" + rootFileName + ".desc" + "/";
	}
	else{
		descDir = outDir + rootFileName + "/";
	}

	std::cout << "Features directory : " << descDir << std::endl;
	std::cout << "-----------------------------------------" << std::endl;

	struct stat st;
	
	if(!stat(descDir.c_str(),&st) == 0)
		mkdir(descDir.c_str(), 01777);

	for (int i=0; i<desc.size(); i++){
		descFileName = descDir + rootFileName + "." + descAbbreviation(desc[i]->getName()) + ".txt";
		std::cout << "Saving " << descFileName << std::endl;
		desc[i]->saveAsTxt(descFileName);
	}

	for (int i=0; i<desc.size(); i++){
		delete desc[i];
	}

        

	delete [] data;
	sf_close(testFile);
}


