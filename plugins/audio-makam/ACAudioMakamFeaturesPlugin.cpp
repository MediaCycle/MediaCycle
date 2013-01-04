/*
 *  ACAudioMakamFeaturesPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 18/05/2012
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include <iostream>
#include <vector>
#include <string>

#include "ACAudioMakamFeaturesPlugin.h"

#include <embed.h>
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

ACAudioMakamFeaturesPlugin::ACAudioMakamFeaturesPlugin() : ACFeaturesPlugin(){
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Makam Features";
    this->mDescription = "Audio Makam Features plugin";
    this->mId = "";
    this->mDescriptorsList.push_back("Makam Histogram");
    //this->mtf_file_name = "";

    // Initiate octave
    //char *sargv[0];
    //octave_init(0, sargv);
    string_vector argv (2);
    argv(0) = "-V";
    argv(1) = "-q";
    octave_main (2, argv.c_str_vec(), 1);

    #ifdef __APPLE__
    std::cout << "Executable path '" << getExecutablePath() << "'" << std::endl;
    boost::filesystem::path e_path( getExecutablePath() );
    std::string r_path = e_path.parent_path().parent_path().string() + "/Resources/";
    std::cout << "Resources path " << r_path << std::endl;
    #endif
}

ACAudioMakamFeaturesPlugin::~ACAudioMakamFeaturesPlugin() {
}

std::vector<ACMediaFeatures*> ACAudioMakamFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;
    ACMediaFeatures* feat;

    // Add the path to the makam toolbox and yin mex files
    boost::filesystem::path s_path( __FILE__ );
    //std::cout << "Main source path: " << s_path.parent_path().parent_path().parent_path() << std::endl;
    boost::filesystem::path b_path( boost::filesystem::current_path() );
    //std::cout << "Main build path " << b_path.parent_path().parent_path() << std::endl;
    std::string source_path(""),build_path(""),slash("/");
    #if defined(__APPLE__)
        #if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
	boost::filesystem::path e_path( getExecutablePath() );
    std::string r_path = e_path.parent_path().parent_path().string() + "/Resources/";
    std::cout << "Resources path " << r_path << std::endl;
    source_path = r_path;
    build_path = r_path;
        #else
        #if defined(XCODE)
            source_path = s_path.parent_path().parent_path().parent_path().parent_path().string() + "/3rdparty/";
            build_path = b_path.parent_path().parent_path().parent_path().string() + "/3rdparty/";
        #else
            source_path = s_path.parent_path().parent_path().parent_path().string() + "/3rdparty/";
            build_path = b_path.parent_path().parent_path().string() + "/3rdparty/";
        #endif
        #endif
    #elif defined (__WIN32__)
        source_path = s_path.parent_path().parent_path().parent_path().string() + "\\";
        build_path = b_path.parent_path().parent_path().string() + "\\";
        slash = "\\";
    #else // Linux
        #if not defined (USE_DEBUG) // needs "make package" to be ran to work
            source_path = "/usr/share/mediacycle/"; // or a prefix path from CMake?
            build_path = "/usr/share/mediacycle/"; // or a prefix path from CMake?
        #else
            source_path = s_path.parent_path().parent_path().parent_path().string() + "/3rdparty/";
            build_path = b_path.parent_path().parent_path().string() + "/3rdparty/";
        #endif
    #endif
    std::string m_path = source_path + "octave_makam";
    std::string y_path = build_path + "octave_yin";
    std::string t_path = m_path + slash + "makamTemplates.txt";
    std::cout << "makam toolbox path " << m_path << std::endl;
    std::cout << "yin mex files path " << y_path << std::endl;
    std::cout << "template path " << t_path << std::endl;

    octave_value_list addpath_m_in = octave_value (m_path);
    octave_value_list addpath_m_out = feval ("addpath",addpath_m_in, 1);
    std::cout << "addpath_m_out.length() " << addpath_m_out.length() << std::endl;
    //if(addpath_m_out.length()>=1) // sq_string
    //    std::cout << "Problem with makam path: " << addpath_m_out(0).type_name() << addpath_m_out(0).string_value() << std::endl;

    octave_value_list addpath_y_in = octave_value (y_path);
    octave_value_list addpath_y_out = feval ("addpath",addpath_y_in, 1);
    std::cout << "addpath_y_out.length() " << addpath_y_out.length() << std::endl;

    // Calculate the Makam histogram with filename and template file as input
    octave_value_list in;
    in.resize (2);
    in(0) = octave_value(theMedia->getFileName());;
    in(1) = octave_value(t_path);
    octave_value_list out = feval ("calculateMakamMultipliedHistogram",in, 1);

    // Convert as MediaCycle feature
    if(out.length()<1){
        std::cerr << "ACAudioMakamFeaturesPlugin::calculate: output of dimension 0" << std::endl;
        return desc;
    }

    std::cout << "ACAudioMakamFeaturesPlugin::calculate: number of args returned " << out.length()<< std::endl;// << " type = " << check_value_type(out(0)) << std::endl;
    std::cout << "ACAudioMakamFeaturesPlugin::calculate: type name " << out(0).type_name() << std::endl;
    std::cout << "ACAudioMakamFeaturesPlugin::calculate: size " << out(0).size() << std::endl;
    ColumnVector features = out(0).vector_value();
    std::cout << "ACAudioMakamFeaturesPlugin::calculate: features length " << features.length() << std::endl;

    feat = new ACMediaFeatures();
    feat->setName("Makam Histogram");
    for (int i = 0; i<features.length() ; i++ )
        feat->addFeatureElement(features(i));
    //feat->setNeedsNormalization(0);
    desc.push_back(feat);
    return desc;
}

// XS TODO !!!
// the plugin should know internally where it saved the mtf 
// thanks to mtf_file_name
//ACMediaTimedFeature* ACAudioMakamFeaturesPlugin::getTimedFeatures(){
//	if (mtf_file_names.size() == 0){
//        cout << "<ACAudioMakamFeaturesPlugin::getTimedFeatures> : missing file name "<<endl;
//		return 0;
//	}
//	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
//	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
//		return 0;
//	}
//	return ps_mtf;
//}
