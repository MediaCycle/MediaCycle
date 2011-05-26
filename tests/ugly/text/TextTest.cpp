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
#include <iostream>
#include <signal.h>
#include <ctime>

#include "ACText.h"

#include "ACTextFeaturesPlugin.h"
#include "ACIndexModifier.h"

#include<iostream>

using namespace std;
using namespace lucene::index;

int main(void){
	string 	pathIndex=string("../../../../apps/textcycle-osg-qt/temp");
	lucene::analysis::SimpleAnalyzer an;
	ACTextFeaturesPlugin* testPlugin=new ACTextFeaturesPlugin();
	for (int i=0;i<100;i++){
		vector<ACMedia*> testLib;
		ACMedia * testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 2.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 3.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 4.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		testMedia=new ACText;
		testMedia->extractData("/Users/ravet/Desktop/MediaCycleData/textTest/A - copie 5.txt");
		testMedia->getAllFeaturesVectors()=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		testLib.push_back(testMedia);
		
		preProcessInfo testInfo=testPlugin->update(testLib);
		std:vector<ACMedia*>::iterator it;
		for (it=testLib.begin();it!=testLib.end();it++){
			(*it)->getAllPreProcFeaturesVectors()=testPlugin->apply(testInfo,(*it));
		}
		for (it=testLib.begin();it!=testLib.end();it++){
			cout <<(*it)->getFileName() <<endl;
			std::vector<float>::iterator it2;
			for (it2=(*it)->getAllPreProcFeaturesVectors()[0]->getFeaturesVector()->begin();it2!=(*it)->getAllPreProcFeaturesVectors()[0]->getFeaturesVector()->end();it2++)
				cout<<(*it2)<<endl;
		}
	//	std::vector<ACMediaFeatures*> testFeat=testPlugin->calculate(((ACText*)testMedia)->getMediaData(),testMedia );
		
		//delete testMedia;
		for (it=testLib.begin();it!=testLib.end();it++){
			delete (*it);
			(*it)=NULL;
		}
	//	sleep(1);
		
		std::cout<<i<<endl;
	}
	_lucene_shutdown(); //clears all static memory
	
	return 0;
	
	

}
