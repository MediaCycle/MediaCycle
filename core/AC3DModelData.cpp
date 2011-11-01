/*
 *  AC3DModelData.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 7/04/11
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


#if defined (SUPPORT_3DMODEL)
#include "AC3DModelData.h"
#include <iostream>
#include "boost/filesystem.hpp"
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

using std::cerr;
using std::endl;
using std::string;


AC3DModelData::AC3DModelData() { 
	this->init();
}

void AC3DModelData::init() {
	media_type = MEDIA_TYPE_3DMODEL;
	model_ptr = 0;
}

AC3DModelData::AC3DModelData(std::string _fname) { 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

AC3DModelData::~AC3DModelData() {
	// model_ptr automatically destroyed thanks to ref_ptr (smart pointer)
	model_ptr=0;	
}

bool AC3DModelData::readData(std::string _fname){ 
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
}

void AC3DModelData::setData(osg::ref_ptr<osg::Node> _data)
{
	model_ptr = dynamic_cast<osg::Node*>( _data->clone(osg::CopyOp::DEEP_COPY_ALL));		
	if( !model_ptr ) {
		// Either the model does not exist, or osg problem
		cerr << "<AC3DModelData::setData> Could not set data" << endl;
	}	
}

#endif //defined (SUPPORT_3DMODEL)
