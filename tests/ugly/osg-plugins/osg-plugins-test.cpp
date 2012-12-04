/*
 *  osg-plugins-test.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 4/12/2012
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

#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

using namespace std;

/// Try to load an OSG plugin by basename:
/// can be file extension without dot
/// or osg plugin without prefix (usually osgdb_) and without extension
/// Could test plugin existence as file, but it requires boost
bool try_load_plugin(string name){
    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(name);
    if (!readerWriter){
        std::cerr << "osg-plugins-test: couldn't load OSG plugin named '"<< name << "'" << std:: endl;
        return false;
    }
    else
        std::cout << "osg-plugins-test: could load OSG plugin named '" << name << "' with className '" << readerWriter->className() << "'" << std::endl;
    readerWriter = 0;
    return true;
}

int main(int, char **) {
    std::cout << "osg-plugins-test: loading generic image plugins..." << std::endl;
    try_load_plugin("bmp");
    try_load_plugin("gif");
    try_load_plugin("jpeg");
    try_load_plugin("jpg");
    try_load_plugin("png");
    try_load_plugin("svg");
    try_load_plugin("tiff");
    std::cout << "osg-plugins-test: loading generic video plugin..." << std::endl;
    try_load_plugin("ffmpeg");
    std::cout << "osg-plugins-test: loading generic text/font plugin..." << std::endl;
    try_load_plugin("ttf");
}
