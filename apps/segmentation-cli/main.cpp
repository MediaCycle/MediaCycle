/**
 * @brief main.cpp
 * @author Cecile Picard
 * @date 22/06/2012
 * @copyright (c) 2012 – UMONS - Numediart
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
#include <stdlib.h>
#include <sndfile.h>
#include <string>
#include <iostream>
#include "ACAudioFeatures.h"
#include "ACMediaTimedFeature.h"
#include <unistd.h>
#include <sys/stat.h>

// For listing files in directory
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <boost/lexical_cast.hpp>
namespace fs = boost::filesystem;

#include "MediaCycle.h"
#include "ACBicSegmentationPlugin.h"
#include "Armadillo-utils.h"

using namespace std;

void usage(string myname){        
	cerr << endl;
	cerr << "Testing segmentation for MoDyVA" << endl;
	
	cerr << "Copyright 2011-2012 University of Mons." << endl;
	cerr << endl;
	cerr << "  Usage: " << myname
			 << " [OPTION] audiofile" << endl;
	cerr << endl;
	cerr << endl;
	cerr << endl;
	
}

string int2string(const int& number)
{
	ostringstream oss;
	oss << number;
	return oss.str();
}


int main(int argc, char** argv){
	std::string filename;
	std::string outDir;
	
	int bflag, ch, fd;
	int mfccNbChannels = 32;
	int mfccNb = 13;
	int windowSize = 512; 	
	bool extendSoundLimits = false;
	
	string myname(argv[0]);
	
	
		 
	//default values
	float _lambda = 1;
    int _sampling_rate = 1;
    int _Wmin = 1000;
    float _bic_thresh = 0.8;
    int _jump_width = 5;
    int _discard_borders_ = 5;
	
	
	
	//boost
	fs::path p(fs::current_path());
	
	bflag = 0;
	while ((ch = getopt(argc, argv, "l:s:W:t:j:b:")) != -1) {
		switch (ch) {
		case 'l' :
			_lambda = atoi(optarg);
			break;	
		case 's':
			_sampling_rate = atoi(optarg);
			break;
		case 'W': 
			_Wmin = atoi(optarg);
			break;
		case 't':
			_bic_thresh = atoi(optarg);
			break;
		case 'j':
			_jump_width = atoi(optarg);
			break;
		case 'b':
			_discard_borders_ = atoi(optarg);
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
	
	/*for (int index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);*/
	if (optind >= argc) {
		fprintf(stderr, "Expected directory after options\n");
		//usage(myname);
		exit(1);
	}
	
	if (argc > 1)
		p = fs::system_complete(argv[optind]);
	else
		std::cout << "\nusage:   simple_ls [path]" << std::endl;
	
	unsigned long file_count = 0;
	unsigned long dir_count = 0;
	unsigned long other_count = 0;
	unsigned long err_count = 0;
	
	////////////// Mediacycle //////////////
	MediaCycle *media_cycle;
	
	ACMediaType _media_type = MEDIA_TYPE_AUDIO;
	
	media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");
	
 	string smedia = "none";
	switch (_media_type) {
		case MEDIA_TYPE_3DMODEL:
#if defined (SUPPORT_3DMODEL)
			smedia="3Dmodel";
#endif //defined (SUPPORT_3DMODEL)
			break;	
		case MEDIA_TYPE_AUDIO:
#if defined (SUPPORT_AUDIO)
			smedia="audio";
#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE:
#if defined (SUPPORT_IMAGE)
			smedia="image";
#endif //defined (SUPPORT_IMAGE)
			break;
		case MEDIA_TYPE_VIDEO:
#if defined (SUPPORT_VIDEO)
			smedia="video";
#endif //defined (SUPPORT_VIDEO)
			break;
		default:
			break;
	}
	
	// -- media-specific features plugin + generic segmentation and visualisation plugins--
	std::string f_plugin, s_plugin;//, v_plugin;
	
	char c_path[2048];
	// use the function to get the path
	getcwd(c_path, 2048);
	std::string s_path = c_path;
	
	std::string build_type ("Release");
#ifdef USE_DEBUG
	build_type = "Debug";
#endif //USE_DEBUG
	
#if defined(__APPLE__)
#if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
	f_plugin = "@executable_path/../MacOS/mc_" + smedia +".dylib";
	s_plugin = "@executable_path/../MacOS/mc_segmentation.dylib";
#else
	f_plugin = s_path + "/../../../plugins/"+ smedia + "/" + build_type + "/mc_" + smedia +".dylib";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib";
#endif
	// common to all media, but only for mac...
#elif defined (__WIN32__)
	f_plugin = s_path + "\..\..\..\plugins\\" + smedia + "\mc_"+smedia+".dll";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dll";
#else
#if not defined (USE_DEBUG) // needs "make package" to be ran to work
	f_plugin = "/usr/lib/mc_"+smedia+".so";
	s_plugin = "/usr/lib/mc_segmentation.so";
#else
	f_plugin = s_path + "/../../plugins/"+smedia+"/mc_"+smedia+".so";
	s_plugin = s_path + "/../../plugins/segmentation/mc_segmentation.so";
#endif
#endif
	
	media_cycle->addPluginLibrary(f_plugin);
	media_cycle->addPluginLibrary(s_plugin);
	//std::cout<<media_cycle->addPluginLibrary(s_plugin)<<endl;
	
	/*ACMedia* ACM;
	//ACMediaTimedFeature* descM; 
	ACM = ACMediaFactory::getInstance().create(MEDIA_TYPE_AUDIO);*/
	ACPluginManager *acpl;
	acpl = media_cycle->getPluginManager();
	int i_o;
	// BICsegmentation
	ACBicSegmentationPlugin* P = dynamic_cast<ACBicSegmentationPlugin*> (acpl->getPlugin("BICSegmentation")); //CPL needed for using setParameters()
	
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "BIC parameters" << std::endl;
	std::cout << "lambda : " << _lambda << std::endl;
	std::cout << "sampling rate : " << _sampling_rate << std::endl;
	std::cout << "Wmin : " << _Wmin << std::endl;
	std::cout << "bic_thresh : " << _bic_thresh << std::endl;
	std::cout << "jump_width : " << _jump_width << std::endl;
	std::cout << "discard_borders : " << _discard_borders_ << std::endl;
	
	//std::cout << "Set parameters " << std::endl;
	P->setParameters(_lambda, _sampling_rate, _Wmin, _bic_thresh, _jump_width, _discard_borders_);
	
	// Saving results
	char name[] = "Segmentation.txt";
	std::string soundDir;
	std::string fileRes;
	/*ofstream examplefile(name); //open the file already!!
	if (examplefile.bad())
	{
		cout << "Cannot create file!" << endl;
	}*/
	
	////////////// Extract files from directory path  //////////////
	std::cout << "Extracting files from directory " << std::endl;
	if (!fs::exists(p))
	{
		std::cout << "\nNot found: " << p << std::endl;
		return 1;
	}
	
	if (fs::is_directory(p))
	{
		std::cout << "\nIn directory: " << p << "\n\n";
		soundDir = p.c_str();
		string sStr = boost::lexical_cast<string>(_sampling_rate);
		string WminStr = boost::lexical_cast<string>(_Wmin); // to put the size of the window in the file name
		string bicStr = boost::lexical_cast<string>(_bic_thresh);
		string jStr = boost::lexical_cast<string>(_jump_width);
		fileRes = soundDir + "/SheetMetal_segTest_s" + sStr + "_W" + WminStr + "_t" + bicStr + "_j" + jStr + ".txt"; 
		ofstream examplefile(fileRes.c_str()); //open the file already!!
		if (examplefile.bad())
		{
			cout << "Cannot create file!" << endl;
		}
		
		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) //loop in the directory
		{
			try
			{
				if (fs::is_directory(dir_itr->status()))
				{
					++dir_count;
					//std::cout << dir_itr->path().filename() << " [directory]\n";
				}
				else if (fs::is_regular_file(dir_itr->status()))
				{
					if (dir_itr->path().extension() == ".wav") {
						++file_count;
						std::cout << "-----------------------------------------" << std::endl;
						std::cout << "Sound file : " << dir_itr->path().filename() << "\n";
						filename = dir_itr->path().string();
						
						std::string numStr;
						int posSep = filename.find_last_of("/\\");
						int posDot = filename.find_last_of(".");
						int posUnd = filename.find_last_of("_");
						if (posSep == -1){
							numStr = filename.substr(posUnd+1, posDot-posSep-1);
						}
						else{
							numStr = filename.substr(posUnd+1, posDot-posSep-1);
						}
						int value = atoi(numStr.c_str());
						
						////////////// do segmentation and save the result //////////////
						ACMedia* ACM;
						ACMediaTimedFeature* descM; 
						ACM = ACMediaFactory::getInstance().create(MEDIA_TYPE_AUDIO);
						i_o = ACM->import(filename, 0, acpl, true); // for segmentation, we must save the timedFeatures
						cerr <<  "features computed" << endl;
						descM = ACM->getTimedFeatures();
						
						std::vector<ACMedia*> seg = P->segment(descM, ACM);
						int Nseg = seg.size();
						cerr << "NUMBER SEGs: " << Nseg << endl; 	
						
											
						std::vector<int> segments_limits;
						segments_limits.resize(Nseg+1); //needed before writing!
						double fractpart, intpart;
						for (int i = 0; i < Nseg; i++){
							//cout << "Segment n° " << i+1 << endl;
							fractpart = modf (seg[i]->getStart() , &intpart);
							if (fractpart>0.5) {
								segments_limits[i] = intpart + 1;
							} else {
								segments_limits[i] = intpart;
							}
							//cout << seg[i]->getStart() << " " << segments_limits[i] << " " << segments_limits[i] + value << endl;
						}
						
						for (int i =0; i < Nseg; i++){
							if (i<Nseg-1) {
								cout << segments_limits[i] + value << " " << segments_limits[i+1] + value << endl;
								examplefile << segments_limits[i] + value << endl;//" " << segments_limits[i+1] + value << endl;
							} else {
								int endValue;
								fractpart = modf (seg[i]->getEnd() , &intpart);
								if (fractpart>0.5) {
									endValue = intpart + 1;
								} else {
									endValue = intpart;
								}
								cout << segments_limits[i] + value << " " << endValue + value << endl;
								examplefile << segments_limits[i] + value << endl;// " " << endValue + value << endl;
							}
						}
						cerr << "delete! " << endl;
						delete ACM, descM;	
						
					}
				}
				else
				{
					++other_count;
					std::cout << dir_itr->path().filename() << " [other]\n";
				}
				
			}
			catch (const std::exception & ex)
			{
				++err_count;
				std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
			}
		}
		std::cout << "\n" << file_count << " files\n"
		<< dir_count << " directories\n"
		<< other_count << " others\n"
		<< err_count << " errors\n";
		
		examplefile.close();
		
	} else {// must be a file
	
		std::cout << p << "is not a directory!\n";    
	}
	
	

	
	
	/*
	 
	filename += argv[optind];
	//filename = "/Users/ccl/data/Projects/numediart/MoDyVa_DameBlanche/DB/Wav_MoDyVA_CAT/Events_adjust/selection/Event9_44100.wav";

	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "Sound file : " << filename << std::endl;
	std::cout << "Output directory : " << outDir << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "BIC parameters" << std::endl;
	std::cout << "lambda : " << _lambda << std::endl;
	std::cout << "sampling rate : " << _sampling_rate << std::endl;
	std::cout << "Wmin : " << _Wmin << std::endl;
	std::cout << "bic_thresh : " << _bic_thresh << std::endl;
	std::cout << "jump_width : " << _jump_width << std::endl;
	std::cout << "discard_borders : " << _discard_borders_ << std::endl;
		
	// CPL: do the usual routine when segmenting
	MediaCycle *media_cycle;
	
	ACMediaType _media_type = MEDIA_TYPE_AUDIO;
	
	media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");
	
 	string smedia = "none";
	switch (_media_type) {
		case MEDIA_TYPE_3DMODEL:
#if defined (SUPPORT_3DMODEL)
			smedia="3Dmodel";
#endif //defined (SUPPORT_3DMODEL)
			break;	
		case MEDIA_TYPE_AUDIO:
#if defined (SUPPORT_AUDIO)
			smedia="audio";
#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE:
#if defined (SUPPORT_IMAGE)
			smedia="image";
#endif //defined (SUPPORT_IMAGE)
			break;
		case MEDIA_TYPE_VIDEO:
#if defined (SUPPORT_VIDEO)
			smedia="video";
#endif //defined (SUPPORT_VIDEO)
			break;
		default:
			break;
	}
	
	
	// -- media-specific features plugin + generic segmentation and visualisation plugins--
	std::string f_plugin, s_plugin;//, v_plugin;
	
	char c_path[2048];
	// use the function to get the path
	getcwd(c_path, 2048);
	std::string s_path = c_path;
	
	std::string build_type ("Release");
#ifdef USE_DEBUG
	build_type = "Debug";
#endif //USE_DEBUG
	
#if defined(__APPLE__)
#if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
	f_plugin = "@executable_path/../MacOS/mc_" + smedia +".dylib";
	s_plugin = "@executable_path/../MacOS/mc_segmentation.dylib";
#else
	f_plugin = s_path + "/../../../plugins/"+ smedia + "/" + build_type + "/mc_" + smedia +".dylib";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib";
#endif
	// common to all media, but only for mac...
#elif defined (__WIN32__)
	f_plugin = s_path + "\..\..\..\plugins\\" + smedia + "\mc_"+smedia+".dll";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dll";
#else
#if not defined (USE_DEBUG) // needs "make package" to be ran to work
	f_plugin = "/usr/lib/mc_"+smedia+".so";
	s_plugin = "/usr/lib/mc_segmentation.so";
#else
	f_plugin = s_path + "/../../plugins/"+smedia+"/mc_"+smedia+".so";
	s_plugin = s_path + "/../../plugins/segmentation/mc_segmentation.so";
#endif
#endif
	
	media_cycle->addPluginLibrary(f_plugin);
	media_cycle->addPluginLibrary(s_plugin);
	
	
	ACMedia* ACM;
	ACMediaTimedFeature* descM; 
	ACPluginManager *acpl;
	ACM = ACMediaFactory::getInstance().create(MEDIA_TYPE_AUDIO);
	acpl = media_cycle->getPluginManager();
	int i_o = ACM->import(filename, 0, acpl, true); // for segmentation, we must save the timedFeatures
	descM = ACM->getTimedFeatures();
	
	// BICsegmentation
	ACBicSegmentationPlugin* P = dynamic_cast<ACBicSegmentationPlugin*> (acpl->getPlugin("BicSegmentation")); //CPL needed for using setParameters()
	P->setParameters(_lambda, _sampling_rate, _Wmin, _bic_thresh, _jump_width, _discard_borders_);
	std::vector<ACMedia*> seg = P->segment(descM, ACM);
	int Nseg = seg.size();
	//cerr << "NUMBER SEGs: " << Nseg << endl; 
	
	std::string descFileName;
	int posSep = filename.find_last_of("/\\");
	int posDot = filename.find_last_of(".");
	int posUnd = filename.find_last_of("_");
	
	std::string rootFileName;
	std::string soundDir;
	std::string numStr;
	if (posSep == -1){
		numStr = filename.substr(posUnd+1, posDot-posSep-1);
		rootFileName = filename.substr(0, posDot);
		soundDir = ".";
	}
	else{
		numStr = filename.substr(posUnd+1, posDot-posSep-1);
		rootFileName = filename.substr(posSep+1, posDot-posSep-1);
		soundDir = filename.substr(0, posSep);
	}

	std::string descDir;
	if (outDir.size()==0){
		descDir = soundDir + "/" + rootFileName + ".bic" + "/";
	}
	else{
		descDir = outDir + rootFileName + "/";
	}
	int value = atoi(numStr.c_str());
	//cout << "beginning of the file: " << value << endl;
	
	struct stat st;
	//if(!stat(descDir.c_str(),&st) == 0)
	//	mkdir(descDir.c_str(), 01777);
	cout << "rootFileName: " << rootFileName << endl;
	descFileName = descDir + rootFileName + ".bic_" + int2string(_lambda) + int2string(_sampling_rate) + int2string(_Wmin) + int2string(_bic_thresh) + int2string(_jump_width) + int2string(_discard_borders_) + ".txt";
	char name[] = "bibi.txt";
	
	//output_file.c_str()??
	ofstream examplefile(name); //open the file already!!
	if (examplefile.bad())
	{
        cout << "Cannot create file!" << endl;
	}
	
	std::vector<int> segments_limits;
	segments_limits.resize(Nseg+1); //needed before writing!
	double fractpart, intpart;
	for (int i = 0; i < Nseg; i++){
		//cout << "Segment n° " << i+1 << endl;
		fractpart = modf (seg[i]->getStart() , &intpart);
		if (fractpart>0.5) {
			segments_limits[i] = intpart + 1;
		} else {
			segments_limits[i] = intpart;
		}
		//cout << seg[i]->getStart() << " " << segments_limits[i] << " " << segments_limits[i] + value << endl;
	}
	
	for (int i =0; i < Nseg; i++){
		if (i<Nseg-1) {
			cout << segments_limits[i] + value << " " << segments_limits[i+1] + value << endl;
			examplefile << segments_limits[i] + value << " " << segments_limits[i+1] + value << endl;
		} else {
			int endValue;
			fractpart = modf (seg[i]->getEnd() , &intpart);
			if (fractpart>0.5) {
				endValue = intpart + 1;
			} else {
				endValue = intpart;
			}
			cout << segments_limits[i] + value << " " << endValue + value << endl;
			examplefile << segments_limits[i] + value << " " << endValue + value << endl;
		}
	}
	
	//examplefile.flush();
	examplefile.close();
	//for (int i=0; i<desc.size(); i++){
	//	descFileName = descDir + rootFileName + "." + descAbbreviation(desc[i]->getName()) + ".txt";
	//	std::cout << "Saving " << descFileName << std::endl;
	//	desc[i]->saveAsTxt(descFileName);
	//}


	delete descM;	
	 */
}


