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
#include <math.h>
//#include "Armadillo-utils.h"
//#include "fftsg_h.c"
#include <time.h>
#include "tinyxml.h"

using namespace std;

static void dancers_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData); 
int processTcpMessageFromInstallation(MediaCycle *that, char *buffer, int l, char **buffer_send, int *l_send); 
void saveLibraryAsXml(MediaCycle *mediacycle, string _path);
void readLibraryXml(MediaCycle *mediacycle, std::string filename);
std::string generateID(std::string filename);

int main(int argc, char** argv) {
  string path = "/Users/dtardieu/Desktop/dancers-dt-4.acl";
  string xmlpath = "/Users/dtardieu/Desktop/dancers-dt-4.xml";
  cout<<"new MediaCycle"<<endl;
  MediaCycle* mediacycle;
  mediacycle = new MediaCycle(MEDIA_TYPE_VIDEO);
  mediacycle->addPlugin ("/Users/dtardieu/src/Numediart/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/eyesweb/Debug/mc_eyesweb.dylib");
  mediacycle->importDirectory("/Users/dtardieu/data/DANCERS/Video/FrontTest/", 0);
  //   mediacycle->importLibrary(path);
  //   mediacycle->getBrowser()->setClusterNumber(1);
  //  mediacycle->startTcpServer(12345,5,dancers_tcp_callback);
  //readLibraryXml(mediacycle, "/Users/dtardieu/Desktop/dancers-exemple.xml");
  saveLibraryAsXml(mediacycle, xmlpath);
  mediacycle->saveAsLibrary("/Users/dtardieu/Desktop/dancers-exemple.acl");
  return (EXIT_SUCCESS);
}
static void dancers_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData) {
  MediaCycle *that = (MediaCycle*)userData;
  processTcpMessageFromInstallation(that, buffer, l, buffer_send, l_send);
}

int processTcpMessageFromInstallation(MediaCycle *that, char *buffer, int l, char **buffer_send, int *l_send) {
    std::string file_name;

    unsigned long pos = 0;
    cout << "Processing TCP message of length" <<  l  << endl;

    unsigned int tot_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);
    cout << "Actual length : " << tot_size << endl;

    unsigned int type_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);

    cout << "type_size : " << type_size << endl;

    std::string type_name(buffer+pos,type_size);
    pos += type_size;

    cout << "type_name : " << type_name << endl;

    //if a wave file is sent, we save it locally
    if ( type_name == "addwavf" ) {
        cout << "SSI : add wave file" << endl;
        unsigned int name_size = *reinterpret_cast<int*>(buffer+pos);
        //extract filename
        pos += sizeof(int);

        cout << "name_size : " << name_size << endl;
        std::string tmp(buffer+pos,name_size);
        file_name = tmp;
        pos += name_size;

        cout << "file_name : " << file_name << endl;

        //extract wavefile
        unsigned int wav_size = *reinterpret_cast<int*>(buffer+pos);
        pos += sizeof(int);

        FILE *local_file;
        time_t timer;
        time(&timer);
        tmp = that->getLocalDirectoryPath()+"/"+file_name;
        local_file = fopen(tmp.c_str(),"wb");
        fwrite((void*)(buffer+pos), 1, wav_size, local_file);
        fclose(local_file);

        pos += wav_size;
    } else if ( type_name == "request" ) {
        cout << "SSI : request" << endl;
        //nothing specific to do
    } else {
        //not a valid request
        return -1;
    }

    unsigned int burst_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);

    cout << "burst_size : " << burst_size << endl;

    std::string burst_labels(buffer+pos,burst_size);
    pos += burst_size;

    cout << "burst_labels : " << burst_labels << endl;

    unsigned int stat_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);

    cout << "stat_size : " << stat_size << endl;

    int nfeats = stat_size/sizeof(float);

    //float *ssi_features = reinterpret_cast<float*>(buffer+pos);
    float *ssi_features = new float[nfeats];

    for (int k=0;k<nfeats;k++) {
        ssi_features[k] = *reinterpret_cast<float*>(buffer+pos);
        //cout << "ssi_features (" << k << ") :" << ssi_features[k] << endl;
        pos += sizeof(float);
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
    for (int i=0; i<nfeats; i++)
        mediaFeatures->setFeature(i,ssi_features[i]);
    mediaFeatures->setComputed();
    cout << "done" << endl;

    cout << "creating media" << endl;
    ACMedia* local_media;
    local_media = ACMediaFactory::create(MEDIA_TYPE_AUDIO);
    local_media->addFeatures(mediaFeatures);
    cout << "done" << endl;

    if ( type_name == "addwavf" ) {
        cout << "addwavf - name" << file_name << " - " << file_name.size() << endl;
        local_media->setFileName(file_name);
        cout << "addwavf - addmedia" << endl;
        that->getLibrary()->addMedia(local_media);
        cout << "addwavf - savelib" << endl;
        that->getLibrary()->saveAsLibrary(that->getLocalDirectoryPath() + "/" + that->getLibName());
        cout << "done" << endl;
    } else if (type_name == "request") {
        vector<ACMedia *> result;
        that->getKNN(local_media,result,1);

        if (result.size() > 0) {
            ACPlugin *greta = that->getPluginManager()->getPlugin("Greta");
            
            if (greta) {
                //filename extension is removed in greta
                cout << "Sent to Greta : " << result[0]->getFileName() << endl;
                greta->calculate(result[0]->getFileName());
            } else {
                cout << "Greta plugin not found, displaying results here ..." << endl;
                cout << "*** REQUEST DUMP : " << endl;
                mediaFeatures->dump();
                
                for (int k=0;k<result.size();k++) {
                    cout << "result (" << k << ") : " << result[k]->getFileName() << endl;
                    result[k]->getFeatures(0)->dump();
                }
            }
        }

        //delete only in "request". In "addwavf", local_media is added to the library and therefore shouyld not be deleted
        delete local_media; 
    } else {
        //not valid (should not happen since already checked before)
        return -1;
    }

    //cout << "BUFFER : " << buffer << endl << endl;
    return 0;
}

void saveLibraryAsXml(MediaCycle* mediacycle, string _path) {
  ACMediaLibrary* media_library;
  media_library = mediacycle->getLibrary();
  ACMedia* local_media;
  int n_loops = media_library->getSize();  
  int featureSize;
  float featureValue;
  std::string featureName;
  std::string ID;
  FILE *library_file = fopen(_path.c_str(),"w");
  fprintf(library_file, "%s\n", "<?xml version=\"1.0\"\?>");
  fprintf(library_file, "%s\n", "<dancers>");

  /// HEADER ///
  fprintf(library_file, "%s\n", "<head>");

  fprintf(library_file, "<feature size=\"6\" >ID</feature>\n");
  
  for (int i=0; i < media_library->getItem(0)->getNumberOfFeatures(); i++){
    featureSize = media_library->getItem(0)->getFeatures(i)->size();
    featureName = media_library->getItem(0)->getFeatures(i)->getName();
    
    if (featureSize > 1){
	std::cout << "Warning : Multidimensional feature, won't be exported" << std::endl;
    }
    else{
      fprintf(library_file, "<feature size=\"1\">");
      fprintf(library_file, "%s",  media_library->getItem(0)->getFeatures(i)->getName().c_str());
      fprintf(library_file, "</feature>\n");
    }
  }
  fprintf(library_file, "%s\n", "</head>");
  
  /// ITEMS //
  fprintf(library_file, "%s\n", "<items>");
  for(int i=0; i<n_loops; i++) {
    fprintf(library_file, "<v>");
    local_media = media_library->getItem(i);    
    
    // printing ID
    ID = generateID(local_media->getFileName());
    fprintf(library_file, "%s", ID.c_str());

    for (int j=0; j < media_library->getItem(i)->getNumberOfFeatures(); j++){
      featureSize = media_library->getItem(i)->getFeatures(j)->size();
      featureName = media_library->getItem(i)->getFeatures(j)->getName();
      
      if (featureSize == 1){
	featureValue = media_library->getItem(i)->getFeatures(j)->getDiscretizedFeature(1,1);
	fprintf(library_file, "%d", (int) featureValue);
      }
      else{
	std::cout << "Warning : Multidimensional feature, won't be exported" << std::endl;
      }
    }
    fprintf(library_file, "</v>\n");
  }
  fprintf(library_file, "%s\n", "</items>");  
  fprintf(library_file, "%s\n", "</dancers>");
  fclose(library_file);
}

void readLibraryXml(MediaCycle* mediacycle, std::string filename){
  std::vector<string> descNames;
  std::vector<int> descDims;
  TiXmlDocument doc( filename.c_str() );
  doc.LoadFile();
  TiXmlNode* node = 0;
  TiXmlNode* dancers = 0;
  TiXmlNode* items = 0;
  TiXmlElement* head = 0;
  TiXmlElement* feature=0;
  
  int dim;
  dancers = doc.FirstChild("dancers");
  assert(dancers);
  head = dancers->FirstChild("head")->ToElement();
  assert(head);
  for( node = head->FirstChild("feature"); node; node = node->NextSibling() ){
    feature = node->ToElement();
    feature->QueryIntAttribute("size", &dim);
    descDims.push_back(dim);
    descNames.push_back(feature->FirstChild()->Value());
  }
  
  items = dancers->FirstChild("items");
  string tmpFeature;
  for( node = items->FirstChild("v"); node; node = node->NextSibling() ){
    int currIdx = 0;
    ACMedia* local_media = ACMediaFactory::create(MEDIA_TYPE_VIDEO);
    tmpFeature = node->FirstChild()->Value();
    for (int i=0; i < descDims.size(); i++) {
      ACMediaFeatures* mediaFeatures = new ACMediaFeatures();
      mediaFeatures->resize(1);
      mediaFeatures->setComputed();
      mediaFeatures->setName(descNames[i]);
      std::cout << tmpFeature.substr(currIdx, descDims[i]).c_str() << std::endl;
      mediaFeatures->setFeature(0, atof(tmpFeature.substr(currIdx, descDims[i]).c_str()));
      currIdx = currIdx+descDims[i];
      local_media->addFeatures(mediaFeatures);
    }
    mediacycle->getLibrary()->addMedia(local_media);
  }
}

std::string generateID(std::string filename){
  const int nbCities=2;
  const std::string cityNames[nbCities] = {"Bru", "Par"};
  const std::string cityID[nbCities] = {"00", "11"};
  std::string IDs, numDancer, numTry, city;
  int posCity;
  int posSep = filename.find_last_of("/\\");
  int posDot = filename.find_last_of(".");
  city = filename.substr(posSep+1, 3);
  numDancer = filename.substr(posSep+5, 3);
  numTry = filename[posSep+9];
  for (int i=0; i<nbCities; i++){
    if (!city.compare(cityNames[i]))
      posCity = i;
  }
  IDs.assign(cityID[posCity]);
  IDs += numDancer;
  IDs += numTry;
  return IDs;
}
