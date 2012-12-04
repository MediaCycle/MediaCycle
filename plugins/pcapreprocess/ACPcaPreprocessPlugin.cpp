/*
 *  ACPcaPreprocessPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 22/05/12
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

#include "ACPcaPreprocessPlugin.h"
#include <vector>
#include <string>
#include "ACPcaPreprocessInfo.h"

using namespace arma;
/*
 std::string mName;
 std::string mId;
 std::string mDescription;
 ACPluginType mPluginType;
 ACMediaType mMediaType;
 */

ACPcaPreprocessPlugin::ACPcaPreprocessPlugin() : ACPreProcessPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL^MEDIA_TYPE_TEXT;
    //this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "PCA preprocess";
    this->mDescription = "Principal component analysis preprocess plugin";
    this->mId = "";
	mLambda=0.95f;
}

ACPcaPreprocessPlugin::~ACPcaPreprocessPlugin(){
}

preProcessInfo ACPcaPreprocessPlugin::update(ACMedias media_library){
	ACPcaPreprocessInfo *pcaInfo=new ACPcaPreprocessInfo;
	
	
        ACMedias::iterator it=media_library.begin();
	int k=0;	
	{
                ACMedia* currMedia=it->second;

		for (int i=0;i<currMedia->getNumberOfFeaturesVectors();i++){
			k+=currMedia->getFeaturesVector(i)->getSize();
		}
	}
		
	
	arma::fmat X(media_library.size(),k);
	int j=0;
	//copy the ACMediaLibrary Features in a Arma::fmat 
	for (it=media_library.begin();it!=media_library.end();it++){
                ACMedia* currMedia=it->second;
		k=0;
		for (int i=0;i<currMedia->getNumberOfFeaturesVectors();i++){
			ACMediaFeatures* currFeat=currMedia->getFeaturesVector(i);
			for (int l=0;l<currFeat->getSize();l++){
				X(j,k)=currFeat->getFeatureElement(l);
				k++;
			}
		}
		j++;
	}
	arma::fmat Z;
	arma::fvec eigValues;
	arma::fmat ZTest,Xred;
	pcaInfo->mean=arma::mean(X,0);
	pcaInfo->stdDev=arma::stddev(X,0);
	for (int i=0;i<pcaInfo->stdDev.n_cols;i++)
		if (pcaInfo->stdDev(0,i)==0.f) {
			pcaInfo->stdDev(0,i)=1.f;
		}
	Xred=(X-ones<fmat>(media_library.size(),1)*pcaInfo->mean)/(ones<fmat>(media_library.size(),1)*pcaInfo->stdDev);
	
	//princomp(pcaInfo->coef,Z,eigValues,X);
	cout<<"coef"<<endl;
	pcaInfo->coef.print();
	cout<<"stat:"<<endl;
	pcaInfo->stdDev.print();
	cout<<"Xred"<<endl;
	Xred.print();
	
	princomp(pcaInfo->coef,Z,eigValues,Xred);
	cout<<"coef"<<endl;
	pcaInfo->coef.print();
	
	arma::fmat varSum=arma::cumsum(eigValues,0)/(arma::sum(arma::sum(eigValues,0)));
	cout<<"eigen cumSum"<<endl;
	
	varSum.print();
	
	int nDim=0;
	while ((nDim<eigValues.n_rows)&&(varSum(nDim,0)<mLambda)){
		nDim++;
	}
	if (nDim==0)
		nDim=1;
	//if (nDim>2)
	//	nDim=2;
	pcaInfo->coef.shed_cols( nDim, pcaInfo->coef.n_cols-1 );
		
	cout<<"eigValues"<<endl;
	eigValues.print();
	
	//cout<<"Z"<<endl;
	//Z.print();
	//tempMat=arma::ones<mat>(media_library.size(),1)*arma::mean(X,0);
	//pcaInfo->mean=arma::mean(X,0);
	//Xred=X-ones<fmat>(media_library.size(),1)*pcaInfo->mean;
	//ZTest=Xred *pcaInfo->coef;
	//cout<<"ZTest"<<endl;
	//ZTest.print();
	//float testMax=arma::max(arma::max(ZTest,0));
	return (preProcessInfo)pcaInfo;
}

std::vector<ACMediaFeatures*> ACPcaPreprocessPlugin::apply(preProcessInfo info,ACMedia* theMedia){
	std::vector<ACMediaFeatures*> desc;
	if (theMedia==0||info==0){
		cerr << "ACPcaPreprocessPlugin::apply : wrong number of feature in the media: " << theMedia->getId() << endl;
		return desc;
	}
	ACPcaPreprocessInfo *pcaInfo=(ACPcaPreprocessInfo*)info;
	ACMedia* currMedia=theMedia;
	arma::fmat X(1,pcaInfo->coef.n_rows);
	int k=0;
	for (int i=0;i<currMedia->getNumberOfFeaturesVectors();i++){
		ACMediaFeatures* currFeat=currMedia->getFeaturesVector(i);
		for (int l=0;l<currFeat->getSize();l++){
			X(0,k)=currFeat->getFeatureElement(l);
			if (k>=pcaInfo->coef.n_rows){
				cerr << "ACPcaPreprocessPlugin::apply : wrong number of feature in the media: " << theMedia->getId() << endl;
				return desc;
			}
			k++;
		}
	}
	if (k<pcaInfo->coef.n_rows){
		cerr << "ACPcaPreprocessPlugin::apply : wrong number of feature in the media: " << theMedia->getId() << endl;
		return desc;
	}
	arma::fmat Xred,Z;
	Xred=(X-pcaInfo->mean)/pcaInfo->stdDev;
	Z=Xred*pcaInfo->coef;
	cout<<"ACPcaPreprocessPlugin::Apply Media Id:"<<theMedia->getId()<<endl;
	Z.print();
	FeaturesVector descVect;
	for (int i=0;i<Z.n_cols;i++)
		descVect.push_back(Z(0,i));
	ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,"Acp");
	desc.push_back(descFeat);
	
	FeaturesVector descVect2;
	descVect2.push_back(0.f);
	int nFeat=theMedia->getNumberOfFeaturesVectors();
	for (int i=1;i<nFeat;i++){
		ACMediaFeatures* descFeat=new ACMediaFeatures(descVect2,"temp");//TODO TR change the weight system. Preprocesses sould be applied only for the features with a weight==1
		desc.push_back(descFeat);
	}
	return desc;
}

void ACPcaPreprocessPlugin::freePreProcessInfo(preProcessInfo &info){
	ACPcaPreprocessInfo* pcaInfo=(ACPcaPreprocessInfo*)info;
	delete pcaInfo;
	pcaInfo=0;
}


