/**
 * @brief ACVisPlugin_t_Sne.cpp
 * @author Christian Frisson
 * @date 31/10/2013
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
#include "ACVisPlugin_t_Sne.h"
#include "t_Sne.h"

using namespace arma;
using namespace std;
//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACVisPlugin_t_Sne::ACVisPlugin_t_Sne() : ACArmaVisPlugin(){
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle t_Sne";
    this->mDescription = "dimensionnality reduction resulting from t_Sne";
    this->mId = "";
    perplexity=10;
    this->addNumberParameter("perplexity",perplexity,1,10000,10,"value of desired perplexity",boost::bind(&ACVisPlugin_t_Sne::perplexityValueChanged,this));
    itMax=2000;
    this->addNumberParameter("Iteration Number",itMax,10,2000,50,"Number of t-Sne iteration",boost::bind(&ACVisPlugin_t_Sne::itMaxValueChanged,this));
    vector<string> initializeValue;
    initializeValue.push_back("Yes");
    initializeValue.push_back("No");
    this->addStringParameter("Initialize","No",initializeValue,"Must we catch the current positions to initialize the t-Sne algorithm",boost::bind(&ACVisPlugin_t_Sne::initValueChanged,this));
    initFlag=false;
    
    //local vars
}

ACVisPlugin_t_Sne::~ACVisPlugin_t_Sne(){
}


void ACVisPlugin_t_Sne::perplexityValueChanged(void){
    if(!this->media_cycle) return;
    
    perplexity=this->getNumberParameterValue("perplexity");
    
}
void ACVisPlugin_t_Sne::itMaxValueChanged(void){
    if(!this->media_cycle) return;
    
    itMax=this->getNumberParameterValue("Iteration Number");
    
}
void ACVisPlugin_t_Sne::initValueChanged(void){
    if(!this->media_cycle) return;
    string initializeValue=this->getStringParameterValue("Initialize");
    if (initializeValue==string("Yes"))
        initFlag=true;
    else
        initFlag=false;
}

void  ACVisPlugin_t_Sne::dimensionReduction(mat &posDisp_m,arma::mat desc_m,arma::umat tag){
    t_Sne t_Sne_algo;
    t_Sne_algo.setPerplexity(perplexity);
    t_Sne_algo.setIterMax(itMax);
    t_Sne_algo.setFeatureMatrix(desc_m);
    if (initFlag){
        arma::mat pos_m;
        this->catchCurrentPosition(media_cycle->getBrowser(), pos_m);
        posDisp_m=t_Sne_algo.compute(2,pos_m);
    }
    else
        posDisp_m=t_Sne_algo.compute(2);
 
    
}


