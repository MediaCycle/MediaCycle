/**
 * @brief 3D model data and container, implemented with OSG
 * @author Xavier Siebert, Christian Frisson
 * @date 7/04/2011
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

#include "AC3DModelOsgData.h"
#include <iostream>
#include "boost/filesystem.hpp"
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <osg/ComputeBoundsVisitor>

using std::cerr;
using std::endl;
using std::string;

AC3DModelOsgDataContainer::AC3DModelOsgDataContainer()
    : ACMediaDataContainer(), ACSpatialData(), data(0)
{}

AC3DModelOsgDataContainer::~AC3DModelOsgDataContainer()
{
    data = 0;
}

AC3DModelOsgData::AC3DModelOsgData() : AC3DModelData(){
    model_ptr = 0;
    dimensions = osg::Vec3(0,0,0);
}

AC3DModelOsgData::~AC3DModelOsgData() {
	// model_ptr automatically destroyed thanks to ref_ptr (smart pointer)
    model_ptr=0;
}

bool AC3DModelOsgData::readData(std::string _fname){
	if(_fname=="") return false;
	model_ptr=0;
	
	std::cout << boost::filesystem::extension(_fname);
	osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(_fname).substr(1));
	if (!readerWriter){
		cerr << "<AC3DModelData::readData> problem loading file, no OSG plugin available" << endl;
		return false;
	}
	model_ptr = osgDB::readNodeFile(_fname);
	readerWriter = 0;
    if( model_ptr == 0 ) {
		cerr << "<AC3DModelData::readData> file can not be read !" << endl;
		return false;
	}

    osg::ComputeBoundsVisitor cbv;
    model_ptr->accept( cbv );
    const osg::BoundingBox bb( cbv.getBoundingBox() );
    //osg::Vec3 ext( bb._max - bb._min );

    /*std::vector<float> center;
    std::vector<float> extent;

    center.resize(3);
    extent.resize(3);

    center[0] = bb.center().x();
    center[1] = bb.center().y();
    center[2] = bb.center().z();
    extent[0] = ext.x();
    extent[1] = ext.y();
    extent[2] = ext.z();*/

    dimensions = osg::Vec3(bb._max - bb._min);

    AC3DModelOsgDataContainer* model_data = new AC3DModelOsgDataContainer();
    model_data->setData(model_ptr);
    this->setData(model_data);

    return true;
}

int AC3DModelOsgData::getWidth(){
    return dimensions.x();
}

int AC3DModelOsgData::getHeight(){
    return dimensions.y();
}

int AC3DModelOsgData::getDepth(){
    return dimensions.z();
}
