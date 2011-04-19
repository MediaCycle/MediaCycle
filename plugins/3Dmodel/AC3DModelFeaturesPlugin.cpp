/**
 * @brief AC3DModelFeaturesPlugin.cpp
 * @author Xavier Siebert
 * @date 20/04/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include "AC3DModelFeaturesPlugin.h"
#include "AC3DModel.h"

#include <vector>
#include <string>
#include <osg/ComputeBoundsVisitor>

AC3DModelFeaturesPlugin::AC3DModelFeaturesPlugin() {
	
    this->mMediaType = MEDIA_TYPE_3DMODEL;
    this->mName = "3DModelFeatures";
    this->mDescription = "3DModelFeatures plugin";
    this->mId = "";
}

AC3DModelFeaturesPlugin::~AC3DModelFeaturesPlugin() {
	
}

std::vector<ACMediaFeatures*> AC3DModelFeaturesPlugin::calculate(std::string aFileName, bool _save_timed_feat)
{
	std::vector<ACMediaFeatures*> Fake;
	return Fake;
	
}
std::vector<ACMediaFeatures*> AC3DModelFeaturesPlugin::calculate(ACMediaData* model_data, ACMedia* theMedia, bool _save_timed_feat) {
	std::vector<ACMediaFeatures*> desc;
//	AC3DModelData* local_model_data = 0;
//	try{
//		local_model_data = static_cast <AC3DModelData*> (model_data);
//		if(! local_model_data) 
//			throw runtime_error("<AC3DModelFeaturesPlugin::calculate> problem with mediaData cast");
//	}catch (const exception& e) {
//		cerr << e.what() << endl;
//		return desc;
//	}
	
	osg::ref_ptr<osg::Node> local_model_ptr;
	ACMediaFeatures* desc_bounding_box_ratio;
	osg::ComputeBoundsVisitor cbv;
	
	local_model_ptr = static_cast< osg::Node* > (model_data->getData());
	local_model_ptr->accept( cbv );
	const osg::BoundingBox bb( cbv.getBoundingBox() );
	osg::Vec3 ext( bb._max - bb._min );	
	vector<float> extent;
	extent.resize(3);
	extent[0] = ext.x(); extent[1] = ext.y(); extent[2] = ext.z();
	vector<float> ratios;
	ratios.resize(3);
	ratios[0] = extent[1]/extent[0];
	ratios[1] = extent[2]/extent[0];
	ratios[2] = extent[2]/extent[1];
	
	desc_bounding_box_ratio = new ACMediaFeatures(ratios, "bounding_box_ratios");  

	desc.push_back(desc_bounding_box_ratio);	
		
	return desc;
}

