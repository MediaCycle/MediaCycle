/*
 *  ACAudioMakamClassifierPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 10/06/2012
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

#if defined(USE_OCTAVE) && defined(USE_MAKAM)

#include <iostream>
#include <vector>
#include <string>

#include "ACAudioMakamClassifierPlugin.h"

#include <embed.h>
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <armadillo>
#include "Armadillo-utils.h"

#ifdef __APPLE__
#include <sys/param.h>
#include <mach-o/dyld.h> /* _NSGetExecutablePath : must add -framework CoreFoundation to link line */
#define MAXPATHLENGTH 256
static std::string getExecutablePath(){
    char *given_path;
    std::string path("");
    given_path = new char[MAXPATHLENGTH * 2];
    if (!given_path) return path;
    unsigned int pathsize = MAXPATHLENGTH * 2;
    unsigned int result = _NSGetExecutablePath(given_path, &pathsize);
    if (result == 0){
        path = std::string (given_path);
        size_t current=0;
          while (current!=string::npos){
              current=path.find("./",2);
              if(current!=string::npos)
                   path.replace(current,2,"");
        }
    }
    free (given_path);
    return path;
}
#endif

ACAudioMakamClassifierPlugin::ACAudioMakamClassifierPlugin() : ACClusterMethodPlugin(){
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "AudioMakamClassifier";
    this->mDescription = "AudioMakamClassifier plugin";
    this->mId = "";

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

ACAudioMakamClassifierPlugin::~ACAudioMakamClassifierPlugin() {
}

void ACAudioMakamClassifierPlugin::updateClusters(ACMediaBrowser* mediaBrowser, bool needsCluster){

    // Add the path to the makam toolbox and yin mex files
    boost::filesystem::path s_path( __FILE__ );
    std::cout << "Main source path: " << s_path.parent_path().parent_path().parent_path() << std::endl;
    boost::filesystem::path b_path( boost::filesystem::current_path() );
    std::cout << "Main build path " << b_path.parent_path().parent_path() << std::endl;
    boost::filesystem::path i_path( boost::filesystem::initial_path() );
    std::cout << "Initial path " << i_path << std::endl;

    std::string source_path(""),build_path(""),slash("/");
#if defined(__APPLE__)
#if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
    boost::filesystem::path e_path( getExecutablePath() );
    source_path = e_path.parent_path().parent_path().string() + "/Resources/";
    build_path = e_path.parent_path().parent_path().string() + "/Resources/";
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
    std::string t_path = m_path + slash + "makamlarArel_short.txt";
    std::cout << "makam toolbox path " << m_path << std::endl;
    std::cout << "yin mex files path " << y_path << std::endl;
    std::cout << "template path " << t_path << std::endl;

    octave_value_list addpath_m_in = octave_value (m_path);
    octave_value_list addpath_m_out = feval ("addpath",addpath_m_in, 1);

    octave_value_list addpath_y_in = octave_value (y_path);
    octave_value_list addpath_y_out = feval ("addpath",addpath_y_in, 1);

    // Classify songs with the list of filenames as input

    std::list<std::string> lst;
    std::vector<ACMedia*> medias = mediaBrowser->getLibrary()->getAllMedia();
    for(std::vector<ACMedia*>::iterator media=medias.begin();media!=medias.end();media++)
        lst.push_back((*media)->getFileName());
    Cell cell(lst);
    octave_value_list in;
    in.resize (2);
    in(0) = cell;
    in(1) = octave_value(t_path);

    octave_value_list out = feval ("calculateMakamStrings",in, 1);

    // Convert as MediaCycle clusters
    if(out.length()<1){
        std::cerr << "ACAudioMakamClassifierPlugin::calculate: output of dimension 0" << std::endl;
        return;
    }

    std::cout << "ACAudioMakamClassifierPlugin::calculate: number of args returned " << out.length()<< std::endl;// << " type = " << check_value_type(out(0)) << std::endl;
    std::cout << "ACAudioMakamClassifierPlugin::calculate: type name " << out(0).type_name() << std::endl;

    std::cout << "ACAudioMakamClassifierPlugin::calculate: is_cellstr " << out(0).cell_value().is_cellstr() << std::endl;
    Array<std::string> clusters_names = out(0).cell_value().cellstr_value();
    std::cout << "ACAudioMakamClassifierPlugin::calculate: length " << clusters_names.length() << std::endl;

    int clusters_names_size = clusters_names.length();
    float radius = 0.2f;
    float angle = 0.0f;

    std::cout << "clusters_names.length() " << clusters_names.length() << std::endl;
    std::cout << "Cluster count " << mediaBrowser->getClusterCount() << std::endl;
    std::cout << "Label size " <<mediaBrowser->getLabelSize() << std::endl;

    std::list<std::string> makams;
    std::cout << "ACAudioMakamClassifierPlugin::calculate: " << clusters_names.length() << "existing makams: ";
    for (int name = 0; name<clusters_names.length() ; name++ ){
        std::cout << clusters_names(name) << ", ";
        makams.push_back(clusters_names(name));
    }
    std::cout << std::endl;

    mediaBrowser->setClusterNumber(clusters_names.length());
    mediaBrowser->initClusterCenters();

    if(mediaBrowser->getLabelSize() < clusters_names.length()){ // perfectible test
        for (int name = 0; name<clusters_names.length() ; name++ ){
            std::cout << "Adding label for makam " << clusters_names(name) << std::endl;
            // Label position shouldn't be set here, but in the position plugins -> implement ACMediaBrowser::addLabel(int i, string text)?
            //angle = (2*arma::math::pi() / (float) clusters_names.length()) * (float) name + (arma::math::pi()/2);
            //mediaBrowser->setLabel(name,clusters_names(name),ACPoint(radius*cos(angle),radius*sin(angle),0.0f));
            mediaBrowser->setLabel(name,clusters_names(name));
        }
        //mediaBrowser->displayAllLabels(false);
    }
    std::cout << "Label size " <<mediaBrowser->getLabelSize() << std::endl;

    std::cout << "Cluster count " << mediaBrowser->getClusterCount() << std::endl;

    Array<std::string> clustered_files = out(1).cell_value().cellstr_value();
    std::cout << "ACAudioMakamClassifierPlugin::calculate: clusters length " << clustered_files.length() << std::endl;

    int index = 0;
    //for (int index = 0; index<clustered_files.length() ; index++ )
    for (std::list<std::string>::iterator filename = lst.begin(); filename!=lst.end() && index < clustered_files.length();filename++){
        std::cout << "ACAudioMakamClassifierPlugin::calculate: cluster " << clustered_files(index) << " for file " << *filename << std::endl;
        std::list<std::string>::iterator makam  = std::find(makams.begin(),makams.end(), clustered_files(index));
        if(makam!=makams.end()){
            mediaBrowser->getMediaNode(index).setClusterId( std::distance(makams.begin(),makam) );
        }
        index++;
    }

}
#endif//defined(USE_OCTAVE) && defined(USE_MAKAM)
