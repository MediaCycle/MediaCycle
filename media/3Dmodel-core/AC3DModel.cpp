/*
 *  AC3DModel.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 09/09/10
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

#include "AC3DModel.h"
#include <fstream>

using std::vector;
using std::string;

using std::ofstream;
using std::ifstream;
using std::endl;

//------------------------------------------------------------------

AC3DModel::AC3DModel() : ACMedia() {
	this->init();
}

AC3DModel::AC3DModel(const AC3DModel& m) : ACMedia(m) {
    this->init();
}

void AC3DModel::init() {
	media_type = MEDIA_TYPE_3DMODEL;
}	

AC3DModel::~AC3DModel() {
}

bool AC3DModel::extractData(string fname) {
    AC3DModelData* model_data = dynamic_cast<AC3DModelData*>(this->getMediaData());
    if(!model_data)
    {
        std::cerr << "AC3DModel::extractData: no 3Dmodel data set" << std::endl;
        return 0;
    }

    this->setWidth( model_data->getWidth() );
    this->setHeight( model_data->getHeight() );

    /*osg::ref_ptr<osg::Node> local_model_ptr = 0;
    osg::ComputeBoundsVisitor cbv;

    ACMediaDataContainer* data_container = model_data->getData();
    AC3DModelOsgDataContainer* model_data_container = dynamic_cast<AC3DModelOsgDataContainer*>(data_container);
    if(!model_data_container){
        std::cerr << "AC3DModel::extractData: couldn't access the 3Dmodel data container from " << fname << std::endl;
        return false;
    }
    local_model_ptr = model_data_container->getData();
    local_model_ptr->accept( cbv );
    const osg::BoundingBox bb( cbv.getBoundingBox() );
    osg::Vec3 ext( bb._max - bb._min );

    center.resize(3);
    extent.resize(3);

    center[0] = bb.center().x();
    center[1] = bb.center().y();
    center[2] = bb.center().z();
    extent[0] = ext.x();
    extent[1] = ext.y();
    extent[2] = ext.z();*/
		
	return true;
}

void AC3DModel::saveACLSpecific(ofstream &library_file) {
//	library_file << center[0] << endl;
//	library_file << center[1] << endl;
//	library_file << center[2] << endl;
//	library_file << extent[0] << endl;
//	library_file << extent[1] << endl;
//	library_file << extent[2] << endl;
}

int AC3DModel::loadACLSpecific(ifstream &library_file) {
//	library_file >> center[0];
//	library_file >> center[1];
//	library_file >> center[2];
//	library_file >> extent[0];
//	library_file >> extent[1];
//	library_file >> extent[2];
	return 1;
}
