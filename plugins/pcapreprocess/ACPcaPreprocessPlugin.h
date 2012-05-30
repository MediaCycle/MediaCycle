/*
 *  ACPcaPreprocessPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 22/05/12
 *  @copyright (c) 2010 – UMONS - Numediart
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
#ifndef _ACPCAPLUGIN_H
#define	_ACPCAPLUGIN_H

#include "MediaCycle.h"
#include "ACMediaData.h"

#include<iostream>
/*
 
 typedef void* preProcessInfo;
 
 class ACPreProcessPlugin : virtual public ACPlugin {
 public:
 ACPreProcessPlugin();
 virtual preProcessInfo update(std::vector<ACMedia*> media_library)=0;
 virtual std::vector<ACMediaFeatures*> apply(preProcessInfo info,ACMedia* theMedia)=0;
 virtual void freePreProcessInfo(preProcessInfo &info)=0;
 protected:
 };
 */

class ACPcaPreprocessPlugin : public ACPreProcessPlugin {
public:
	ACPcaPreprocessPlugin();
	~ACPcaPreprocessPlugin();

	virtual preProcessInfo update(std::vector<ACMedia*> media_library);
	
	virtual std::vector<ACMediaFeatures*> apply(preProcessInfo info,ACMedia* theMedia);
	
	virtual void freePreProcessInfo(preProcessInfo &info);
	void setLambda(float lambda){
		if (lambda>0.f&&lambda<1)
			mLambda=lambda;
		else 
			lambda=1.f;};
	float getLambda(void){return mLambda;};
	
private:
	float k;
	float mLambda;
	
protected:	
};

#endif	/* _ACPCAPLUGIN_H */
