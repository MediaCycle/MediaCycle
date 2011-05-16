/*
 *  AC3DModel.cpp
 *  MediaCycle
 *
 *  Created by St�phane Dupont on 09/09/10.
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

#if defined (SUPPORT_3DMODEL)
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

void AC3DModel::init() {
	media_type = MEDIA_TYPE_3DMODEL;
}	

AC3DModel::~AC3DModel() {
	
}

void AC3DModel::setData(osg::ref_ptr<osg::Node> _data)
{
	if (data == 0) 
		data = new AC3DModelData();
	data->setData(_data);
}

void AC3DModel::extractData(string fname) {
	if (data) delete data;
	data = new AC3DModelData();
	data->readData(fname);

	osg::ref_ptr<osg::Node> local_model_ptr = 0;
	osg::ComputeBoundsVisitor cbv;
	
	local_model_ptr = this->getData();
	local_model_ptr->accept( cbv );
	const osg::BoundingBox bb( cbv.getBoundingBox() );
	osg::Vec3 ext( bb._max - bb._min );
	
	center.resize(3);
	extent.resize(3);
	
	center[0] = bb.center().x(); center[1] = bb.center().y(); center[2] = bb.center().z();
	extent[0] = ext.x(); extent[1] = ext.y(); extent[2] = ext.z();
		
	//return model_data;
}

void AC3DModel::deleteData(){
	if (data)
		delete data;
	data=0;
}

void AC3DModel::saveACLSpecific(ofstream &library_file) {
	
	library_file << center[0] << endl;
	library_file << center[1] << endl;
	library_file << center[2] << endl;
	library_file << extent[0] << endl;
	library_file << extent[1] << endl;
	library_file << extent[2] << endl;
}

int AC3DModel::loadACLSpecific(ifstream &library_file) {
	
	library_file >> center[0];
	library_file >> center[1];
	library_file >> center[2];
	library_file >> extent[0];
	library_file >> extent[1];
	library_file >> extent[2];
		
	return 1;
}
#endif //defined (SUPPORT_3DMODEL)
