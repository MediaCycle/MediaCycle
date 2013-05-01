/**
 * @brief ACVisPlugin_PCA_constrained_t_Sne.cpp
 * @author Thierry Ravet
 * @date 01/05/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACVisPlugin_PCA_constrained_t_Sne.h"
#include "PCA_constrained_t_Sne.h"

using namespace arma;
using namespace std;
//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACVisPlugin_PCA_constrained_t_Sne::ACVisPlugin_PCA_constrained_t_Sne() : ACArmaVisPlugin(){
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle PCA constrained t_Sne";
    this->mDescription = "dimensionnality reduction resulting from t_Sne";
    this->mId = "";
    perplexity=20;
    this->addNumberParameter("perplexity",perplexity,10,100,10,"value of desired perplexity",boost::bind(&ACVisPlugin_PCA_constrained_t_Sne::perplexityValueChanged,this));
    itMax=1000;
    this->addNumberParameter("Iteration Number",itMax,10,2000,50,"Number of t-Sne iteration",boost::bind(&ACVisPlugin_PCA_constrained_t_Sne::itMaxValueChanged,this));
    lambda=0.95;
    this->addNumberParameter("Lambda",lambda,0.01,1.0,0.01,"Weight between tSne and PCA",boost::bind(&ACVisPlugin_PCA_constrained_t_Sne::lambdaValueChanged,this));
    vector<string> initializeValue;
    initializeValue.push_back("Yes");
    initializeValue.push_back("No");
    this->addStringParameter("Initialize","No",initializeValue,"Must we catch the current positions to initialize the t-Sne algorithm",boost::bind(&ACVisPlugin_PCA_constrained_t_Sne::initValueChanged,this));
    initFlag=false;
    
    //local vars
}

void ACVisPlugin_PCA_constrained_t_Sne::mediaCycleSet()
{
    // CF this will work only since/if we load feature plugins before this plugin...
    this->updateAvailableFeatures();
}
void ACVisPlugin_PCA_constrained_t_Sne::assignedFeaturesChanged(){
    if(!this->media_cycle) return;
    media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}

bool ACVisPlugin_PCA_constrained_t_Sne::updateAvailableFeatures(){
    if(this->media_cycle){
        if(this->media_cycle->getPluginManager()){
            std::vector<std::string> feature_names;
            std::vector<std::string> names = this->media_cycle->getListOfActivePlugins();
            std::vector<float> weight=this->media_cycle->getBrowser()->getWeightVector();
            //for (std::vector<std::string>::iterator name = names.begin();name != names.end();name++)
            if (weight.size()!=names.size())
                return false;
            for (int i=0;i<weight.size();i++){
                if (weight[i]>0)
                    feature_names.push_back(names[i] + " (dim 0)");
                //std::cout << "ACVisPlugin2Desc: feature: " << *feature << std::endl;
            }
            if(feature_names.size()>=1){
                if(this->hasStringParameterNamed("x"))
                    this->updateStringParameter("x",feature_names.front(),feature_names);
                else
                    this->addStringParameter("x",feature_names.front(),feature_names,"feature assigned to x axis",boost::bind(&ACVisPlugin_PCA_constrained_t_Sne::assignedFeaturesChanged,this));
                return true;
            }
        }
    }
    return false;
}


ACVisPlugin_PCA_constrained_t_Sne::~ACVisPlugin_PCA_constrained_t_Sne(){
}


void ACVisPlugin_PCA_constrained_t_Sne::perplexityValueChanged(void){
    if(!this->media_cycle) return;
    
    perplexity=this->getNumberParameterValue("perplexity");
    
}
void ACVisPlugin_PCA_constrained_t_Sne::itMaxValueChanged(void){
    if(!this->media_cycle) return;
    
    itMax=this->getNumberParameterValue("Iteration Number");
    
}
void ACVisPlugin_PCA_constrained_t_Sne::initValueChanged(void){
    if(!this->media_cycle) return;
    string initializeValue=this->getStringParameterValue("Initialize");
    if (initializeValue==string("Yes"))
        initFlag=true;
    else
        initFlag=false;
}
void ACVisPlugin_PCA_constrained_t_Sne::lambdaValueChanged(void){
    if(!this->media_cycle) return;
    
    lambda=this->getNumberParameterValue("Lambda");
    
}
void  ACVisPlugin_PCA_constrained_t_Sne::dimensionReduction(mat &posDisp_m,arma::mat desc_m,arma::umat tag){
    PCA_constrained_t_Sne t_Sne_algo;
    t_Sne_algo.setPerplexity(perplexity);
    t_Sne_algo.setIterMax(itMax);
    t_Sne_algo.setFeatureMatrix(desc_m);
    //t_Sne_algo.setXInit(desc_m.col(indX));
    mat tempMat;
    if (initFlag){
        arma::mat pos_m;
        this->catchCurrentPosition(media_cycle->getBrowser(), pos_m);
        tempMat=t_Sne_algo.compute(2,lambda,pos_m);
    }
    else
        tempMat=t_Sne_algo.compute(2,lambda);
   /* arma::mat tempVec=t_Sne_algo.getX();
    uword n=tempVec.n_rows;
    tempVec=tempVec-ones(n,1)*mean(tempVec);
    tempVec=tempVec/(ones(n,1)*stddev(tempVec));
    posDisp_m.resize(desc_m.n_rows,2);
    posDisp_m.col(0)=tempVec;
    posDisp_m.col(1)=tempMat.col(0);*/
    posDisp_m=tempMat;

}

void ACVisPlugin_PCA_constrained_t_Sne::updateNextPositions(ACMediaBrowser* browser){
    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;
    
    if(!(this->hasStringParameterNamed("x"))){
        if(!(this->updateAvailableFeatures())){
            std::cout << "ACVisPlugin2Desc::updateNextPositions: available features not set" << std::endl;
            media_cycle->setNeedsDisplay(true);
            return;
        }
    }
    std::string x = this->getStringParameterValue("x");
    size_t x_pos = x.find(" (dim");
    if(x_pos != std::string::npos)
        x = x.substr(0,x_pos);

    ACMedias medias = browser->getLibrary()->getAllMedia();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = browser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=browser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACArmaVisPlugin::extractDescMatrix weight vector size incompatibility"<<endl;
    for(int f=0; f< nbFeature; f++){
        if (weight[f]>0.f){
            if (x==(browser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName())){
                indX=totalDim;
            }
                
            featDim = browser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
            for(int d=0; d < featDim; d++){
                totalDim++;
            }
            
        }
    }
    

    ACArmaVisPlugin::updateNextPositions(browser);

};


