/*
 *  ACMediaLibrary.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 21/04/09
 *
 *  - XS 26/06/09 : removed size_library (= media_library.size())
 *
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

#include "ACPluginManager.h"

#include "ACMediaLibrary.h"
#include "ACMediaFactory.h"

// XS for sorting:
#include <algorithm>

#include <Common/TiMath.h> // for Timax ...

using namespace std;

ACMediaLibrary::ACMediaLibrary() {
	index_last_normalized = -1;
        media_library.resize(0);
        media_type = NONE;
}

ACMediaLibrary::ACMediaLibrary(ACMediaType aMediaType) {
	index_last_normalized = -1;
        media_library.resize(0);
        media_type = aMediaType;
}

int ACMediaLibrary::importDirectory(std::string _path, int _recursive, int id, ACPluginManager *acpl) {
// XS : return value convention: -1 = error ; otherwise returns number of files added

	unsigned long file_count = 0;
	unsigned long dir_count = 0;
	unsigned long other_count = 0;
	
	string filename;
	string extension;
	
	fs::path full_path( fs::initial_path<fs::path>() );
	
	full_path = fs::system_complete( fs::path( _path, fs::native ) );
	
	if ( !fs::exists( full_path ) )
	{
		printf("File or directory not found: %s\n", full_path.native_file_string().c_str());
		return -1;
	}
	
	//ACMediaFactory factory;
	
	if ( fs::is_directory( full_path ) )
	{
		fs::directory_iterator end_iter;
		for ( fs::directory_iterator dir_itr( full_path );
			 dir_itr != end_iter;
			 ++dir_itr )
		{
			if ( _recursive && fs::is_directory( dir_itr->path() ) )
			{
				++dir_count;
				file_count += importDirectory((dir_itr->path()).native_file_string(), _recursive); 
			}
			else if ( fs::is_regular( dir_itr->path() ) )
			{
				file_count += importDirectory((dir_itr->path()).native_file_string(), 0);										
			}
			else 
			{
				++other_count;
			}
		}
	}
	else
	{
		filename = _path;
		extension = fs::extension(_path);
		cout << "extension:" << extension << endl; 
		
		ACMedia* media = ACMediaFactory::create(extension); // XS TODO : import ?
		
		if (media == NULL) {
			cout << "extension unknown, skipping " << filename << " ... " << endl;
			++other_count; // XS-SD TODO : OK ?
		}
		else {
			media->import(filename); // or try 2d way to make a factory
			if (id>=0) {
				media->setId(id);
			}
                        //compute features with available plugins
                        if (acpl) {
                            for (int i=0;i<acpl->getSize();i++) {
                                for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
                                    if (acpl->getPluginLibrary(i)->getPlugin(j)->getType() == media->getType()) {
                                        //TODO move this in media ?
                                        ACMediaFeatures *af = acpl->getPluginLibrary(i)->getPlugin(j)->calculate(media->getFileName());
                                        //another option :
                                        //ACMediaFeatures *af = acpl->getPluginLibrary(i)->calculate(j,media->getFileName());
                                        media->addFeatures(af);
                                    }
                                }
                            }
                        }
			media_library.push_back(media);
			++file_count; // XS-SD TODO : here ?
		}
	}
	return file_count;

}
	
int ACMediaLibrary::openLibrary(std::string _path, bool aInitLib){
	// this does not re-initialize the media_library
	// but appends new media to it.
        // except if aInitLib is set to true
	int ret, file_count=0;
	
	FILE *library_file = fopen(_path.c_str(),"r");
	
	//ACMediaFactory factory;
	ACMedia* local_media;
	// --TODO-- ???  how does it know which type of media ?
	// have to be set up  at some point using setMediaType()
        if (aInitLib) {
            cleanLibrary();
        }
	do {
		local_media = ACMediaFactory::create(media_type);
		ret = local_media->load(library_file);
		if (ret) {
			media_library.push_back(local_media);
                        file_count++;
		}
	}
	while (ret>0);
	
	fclose(library_file);

        return file_count;
}

void ACMediaLibrary::saveAsLibrary(string _path) {
	
	int n_loops = media_library.size();
	
	FILE *library_file = fopen(_path.c_str(),"w");
	//ACMediaFactory factory;
	
	for(unsigned int i=0; i<n_loops; i++) {
		//ACMedia* local_media = factory.create(media_type);
		// --TODO-- ???  how does it know which type of media ?
		// have to be set up  at some point using setMediaType()
		ACMedia* local_media = media_library[i];
		local_media->save(library_file);
	}	
	
	fclose(library_file);
}

void ACMediaLibrary::cleanLibrary() {
	cleanStats();
	media_library.clear();
}

void ACMediaLibrary::cleanStats() {
	cout << "cleaning features mean and stdev"<< endl;
	mean_features.clear();
	stdev_features.clear();
}

ACMedia* ACMediaLibrary::getItem(int i){
	if (i < media_library.size()){
		return media_library[i];
	}
	else return NULL;
}

std::string ACMediaLibrary::getThumbnail(int id) {
	int i;
	for (i=0;i<media_library.size();i++) {
		if (id==media_library[i]->getId()) {
			return media_library[i]->getThumbnail();
		}
	}
	return "";
}

bool ACMediaLibrary::isEmpty() {
	int n = media_library.size() ;
	if (n <= 0) {
		cout << "empty library" << endl;
		return true;
	}
	
	// XS assumes each item has same number of features
	// and each feature has the same number of floats in its vector
	
	int number_of_features = media_library[0]->getNumberOfFeatures();
	if (number_of_features <= 0) {
		cout << "no features to normalize" << endl;
		return true;
	}
	return false;
}

void ACMediaLibrary::calculateStats() {
	if ( isEmpty() ) return;
	int n = media_library.size() ;
	int number_of_features = media_library[0]->getNumberOfFeatures();

	// initialize to zero
	int i,j,k;
	for (i=0; i< number_of_features; i++) {
		vector<double> tmp_vect;
		for (j=0; j< media_library[0]->getFeatures(i)->size(); j++) {
			tmp_vect.push_back(0.0);
		}
		mean_features.push_back(tmp_vect);
		stdev_features.push_back(tmp_vect);
	}
	
	// computing sums
	for(i=0; i<n; i++) {
		ACMedia* item = media_library[i];		
		for(j=0; j<mean_features.size(); j++){
			for(k=0; k<mean_features[j].size(); k++){
				double val = item->getFeatures(j)->getFeature(k);
				//XS debug
				cout << "val:" << val << endl;
				
				mean_features[j][k] += val;
				stdev_features[j][k] += val * val;

                                cout << "stdev_features:" << stdev_features[j][k] << endl;
			}
		}
	}
	
	// divide by N --> biased variance estimator
	for(j=0; j<mean_features.size(); j++) {
		printf("feature %d\n", j);
		for(k=0; k<mean_features[j].size(); k++) {
			mean_features[j][k] /= n;
			stdev_features[j][k] /= n;
                        double tmp = stdev_features[j][k] - mean_features[j][k] * mean_features[j][k];
                        if ( tmp < 0 )
                            stdev_features[j][k] = 0;
                        else {
                            stdev_features[j][k] = sqrt( tmp);
                        }
			printf("\t[%d] mean_features = %f, stddev = %f\n", k, mean_features[j][k], stdev_features[j][k]);
		}
	}
	
}

void ACMediaLibrary::normalizeFeatures() {
	cout << "normalizing features" << endl;
	if ( isEmpty() )  return;
	
	if (index_last_normalized < 0) {
		// *first* normalization
		calculateStats();
	}
	else {
		// *subsequent* normalization
		denormalizeFeatures();
		calculateStats();
	}
		
	int i,j,k;
	
	int n = media_library.size() ;
	
	for(i=0; i<n; i++){
		ACMedia* item = media_library[i];
		for(j=0; j<mean_features.size(); j++) {
			for(k=0; k<mean_features[j].size(); k++) {
				float old = item->getFeatures(j)->getFeature(k);
				item->getFeatures(j)->setFeature (k, (old - mean_features[j][k]) / ( TI_MAX(stdev_features[j][k] , 0.00001)));
			}
		}
	}
	index_last_normalized = n-1;
}

void ACMediaLibrary::denormalizeFeatures() {
	cout << "denormalizing features" << endl;
	if ( isEmpty() ) return;	

	int i,j,k;
	
	// XS denormalize only those that have been normalized (duh),
	// so n is NOT the full library size !
	
	for(i=0; i<= index_last_normalized; i++){
		ACMedia* item = media_library[i];
		for(j=0; j<mean_features.size(); j++) {
			for(k=0; k<mean_features[j].size(); k++) {
				float old = item->getFeatures(j)->getFeature(k);
				item->getFeatures(j)->setFeature (k, old * stdev_features[j][k] + mean_features[j][k]);
			}
		}
	}
	index_last_normalized = -1;
	cleanStats();
}



// XS custom for Thomas Isreal videos : features sorted, along with filename
void ACMediaLibrary::saveSorted(string output_file){
	//XS dirty, supposes you save feature number 0 -- should be generalized !
	ofstream out(output_file.c_str()); 
	
	const int nfeat=1;
	
	std::vector<std::pair<float, int> > f[nfeat];
	
	for (int i=0; i< media_library.size() ;i++) {
		out << i << " : " << media_library[i]->getFileName() << endl;
		for (int j=0; j< nfeat ;j++) {
			out << media_library[i]->getFeatures(0)->getFeature(j) << endl;
			f[j].push_back (std::pair<float, int> (media_library[i]->getFeatures(0)->getFeature(j), i));
		}
	}
	for (int j=0; j< nfeat ;j++) {
		sort (f[j].begin(),f[j].end());
	}
	std::vector<std::pair<float, int> >::const_iterator itr;
	//	for (int j=0; j< nfeat ;j++) {
	////		cout << " -- " << j <<   " -- " << endl;
	////		for(itr = f[j].begin(); itr != f[j].end(); ++itr){
	////			std::cout << "Value = " << (*itr).first << ", original index = " << (*itr).second << endl;
	////		}
	//		for(itr = f[j].begin(); itr != f[j].end(); ++itr){
	//			out  << (*itr).first << "\t" << (*itr).second+1  << endl;		}
	//		out << endl;
	//	}
	for (int i=0; i<media_library.size() ;i++) {
		out << f[0][i].first << "\t" << f[0][i].second << "\t" << 
//		f[1][i].first << "\t" << f[1][i].second << "\t" <<
//		f[2][i].first << "\t" << f[2][i].second << "\t" << 
//		f[3][i].first << "\t" << f[3][i].second << "\t" << 
//		f[4][i].first << "\t" << f[4][i].second << "\t" << 
//		f[5][i].first << "\t" << f[5][i].second << "\t" <<  
		endl;
	}
	out.close();
}
