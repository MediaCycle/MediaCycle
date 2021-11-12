/*
 *  TextTest.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 24/05/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include <stdlib.h>
#include <string>
#include <cstring>
#include<iostream>
#include<fstream>
#include<vector>
#define TIXML_USE_STL
#include <tinyxml.h>

using namespace std;
//Script to encode a xml mediacycle file from xml navimed files
int main(int argc, char *argv[])

{
	cout<<argc<<endl;
	int cpt=0;
    cout<<(cpt++)<<" "<<(cpt++)<<" "<<(cpt++);
	for (int i=0;i<argc;i++)
		cout<<argv[i]<<endl;
	if (argc>1)
	{
		string tempFileName=string(argv[1]);
		fstream* fileStr=new fstream(tempFileName.c_str(),fstream::in);
		string absPath=tempFileName.substr(0,tempFileName.find_last_of("/"))+string("/");
		if (fileStr->is_open())
		{
			cout<<"file_open"<<endl;
			string test;
			vector<vector<float> > v;
			vector<string> content,patient,episode,filenames;
			
            
            char line[1024];
			fileStr->getline( line,1024);
            cout<<line<<endl;
            while(fileStr->getline( line,1024))
            {
                std::stringstream linestream(line);	
                vector<float> tempF;
				string tempStr,tempStr1,tempStr2,tempStr3,tempStr4;
				linestream>>tempStr1;
				if (linestream.eof())
					break;
				linestream>>tempStr2;
				if (linestream.eof())
					break;
				linestream>>tempStr3;
				if (linestream.eof())
					break;
				linestream>>tempStr4;
				if (linestream.eof())
					break;
                
				cout<<tempStr1<<"\t";
				cout<<tempStr2<<"\t";
				cout<<tempStr3<<"\t";
				cout<<tempStr4+string(".xml")<<"\t";
				do {
                    linestream>>tempStr;
                    tempF.push_back(atof(tempStr.c_str()));
                    cout<<tempF[tempF.size()-1]<<"\t";
                } while (!linestream.eof());
                
				cout<<endl;
                v.push_back(tempF);
                content.push_back(tempStr1);
				patient.push_back(tempStr2);
				episode.push_back(tempStr3);
				filenames.push_back(tempStr4+string(".xml"));
				
            

			
			}
			delete fileStr;
			string _fname=tempFileName.substr(0,tempFileName.length()-4)+string(".xml");
			cout<<_fname<<endl;
			
			TiXmlDocument MC_doc;
			TiXmlDeclaration* MC_decl = new TiXmlDeclaration( "1.0", "", "" );
			MC_doc.LinkEndChild( MC_decl );
			
			TiXmlElement* MC_e_root = new TiXmlElement( "MediaCycle" );
			MC_doc.LinkEndChild( MC_e_root );
			
			// "header"
			TiXmlElement* MC_e_browser_mode = new TiXmlElement( "BrowserMode" );
			MC_e_root->LinkEndChild( MC_e_browser_mode );
			// XS  TODO get it as text e.g. this->getBrowserModeAsString()
			std::stringstream tmp_bm;
			tmp_bm << 1;
			TiXmlText* MC_t_bm = new TiXmlText( tmp_bm.str() );
			MC_e_browser_mode->LinkEndChild( MC_t_bm );
			
			TiXmlElement* MC_e_media_type = new TiXmlElement( "MediaType" );
			MC_e_root->LinkEndChild( MC_e_media_type );
			// XS  TODO get it as text e.g. this->getMediaTypeAsString()
			std::stringstream tmp_mt;
			tmp_mt << 0x0040;
			TiXmlText* MC_t_mt = new TiXmlText( tmp_mt.str() );
			MC_e_media_type->LinkEndChild( MC_t_mt );
			
			// "medias and features"
			TiXmlElement* MC_e_features_weights = new TiXmlElement("FeaturesWeights");
			MC_e_root->LinkEndChild( MC_e_features_weights );
			vector<float> features_weights;
			features_weights.push_back(1.f);
			features_weights.push_back(1.f);
			
			MC_e_features_weights->SetAttribute("NumberOfFeatures", features_weights.size());
			
			// concatenate feature weights separated by a " "
			std::string sfw;
			std::stringstream tmp;
			for (unsigned int j=0; j<features_weights.size(); j++) {
				tmp << features_weights[j]<< " " ;
			}
			sfw = tmp.str();
			TiXmlText* MC_t_features_weights = new TiXmlText(sfw.c_str());
			MC_e_features_weights->LinkEndChild( MC_t_features_weights );
			
			std::string media_identifier = "Medias";
			TiXmlElement* MC_e_medias = new TiXmlElement(media_identifier);
			
			//this->mediaLibrary->saveCoreXMLLibrary(MC_e_root, MC_e_medias);
			TiXmlElement* MC_e_number_of_medias = new TiXmlElement( "NumberOfMedia" );
			MC_e_root->LinkEndChild( MC_e_number_of_medias );
			int n_loops = v.size();
			std::string s_loops;
			std::stringstream tmp2;
			tmp2 << n_loops;
			s_loops = tmp2.str();
			
			int a=0;
			
			TiXmlText* MC_t_nm = new TiXmlText( s_loops );
			MC_e_number_of_medias->LinkEndChild( MC_t_nm );
			
			// XS TODO iterator
			std::vector<TiXmlElement*> medias;
			
			for(int i=0; i<n_loops; i++) {
				TiXmlElement* media = new TiXmlElement( "Media" );
				MC_e_medias->LinkEndChild( media );
				//media_library[i]->saveXML( media);
				//**********************************
			{
					//if ( features_saved_xml || _medias == NULL) return; 
					/*if (_medias == NULL) return;
					 TiXmlElement* media;
					 #ifdef SUPPORT_MULTIMEDIA
					 if(this->media_type == MEDIA_TYPE_MIXED)
					 media = new TiXmlElement( "MediaDocument" );
					 else
					 #endif
					 media = new TiXmlElement( "Media" );
					 _medias->LinkEndChild( media );*/
					
					//if (media == NULL) return;
					
					media->SetAttribute("MediaID", i);
					media->SetAttribute("MediaType", 0x0040);
					
					media->SetAttribute("FileName", absPath+filenames[i]);
				
					media->SetAttribute("Label", filenames[i].substr(0,filenames[i].length()-4)+string(" ")+content[i]);
					
					int n_features =1;
					TiXmlElement* features = new TiXmlElement( "Features" );  
					features->SetAttribute("NumberOfFeatures", n_features);
                media->LinkEndChild( features );  
                int n_features_elements =  v[i].size() ;
                int nn = 0;
                TiXmlElement* mediaf = new TiXmlElement( "Feature" );  
                features->LinkEndChild( mediaf );
                    mediaf->SetAttribute("FeatureName", "V");
                mediaf->SetAttribute("NeedsNormalization", nn);
                mediaf->SetAttribute("NumberOfFeatureElements",n_features_elements);
                // keep feature elements separated by a " "
                std::string s;
                std::stringstream tmp;
					
					for (int j=0; j<n_features_elements; j++) {
						
                        tmp<< v[i][j]<<" "; 
						
					}
                
                s = tmp.str();
                TiXmlText* mediafe = new TiXmlText(s.c_str());
                mediaf->LinkEndChild( mediafe );  		
					TiXmlElement* segments = new TiXmlElement( "Segments" );  
					media->LinkEndChild( segments );  
					segments->SetAttribute("NumberOfSegments", 0);
					
				}
				//*********************************
			}
			
			
			MC_e_root->LinkEndChild( MC_e_medias );
			
			// "plugins"
			// XS TODO put this in a method getPluginsNames(blabla)
		/*	if (pluginManager) {
				TiXmlElement* MC_e_features_plugin_manager = new TiXmlElement( "PluginsManager" );
				MC_e_root->LinkEndChild( MC_e_features_plugin_manager );
				int n_librarires = pluginManager->getSize();
				MC_e_features_plugin_manager->SetAttribute("NumberOfPluginsLibraries", n_librarires);
				for (int i=0; i<n_librarires; i++) {
					TiXmlElement* MC_e_features_plugin_library = new TiXmlElement( "PluginLibrary" );
					MC_e_features_plugin_manager->LinkEndChild( MC_e_features_plugin_library );
					int n_plugins = pluginManager->getPluginLibrary(i)->getSize();
					MC_e_features_plugin_library->SetAttribute("NumberOfPlugins", n_plugins);
					MC_e_features_plugin_library->SetAttribute("LibraryPath", pluginManager->getPluginLibrary(i)->getLibraryPath());
					for (int j=0; j<n_plugins; j++) {
						TiXmlElement* MC_e_features_plugin = new TiXmlElement( "FeaturesPlugin" );
						MC_e_features_plugin_library->LinkEndChild( MC_e_features_plugin );
						std::stringstream tmp_p;
						tmp_p << pluginManager->getPluginLibrary(i)->getPlugin(j)->getName() ;
						TiXmlText* MC_t_pm = new TiXmlText( tmp_p.str() );
						MC_e_features_plugin->LinkEndChild( MC_t_pm );
					}
				}
			}*/
			
			MC_doc.SaveFile(_fname.c_str());
			
			
		}
		

	}
		

	
	return 0;
	
	

}
