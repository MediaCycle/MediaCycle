/**
 * @brief Callback to share identical images between renderers
 * @author Christian Frisson
 * @date 9/04/2013
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

#ifndef __ACOsgReadAndShareImageCallback_H__
#define __ACOsgReadAndShareImageCallback_H__

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osgDB/ReadFile>

class ACOsgReadAndShareImageCallback : public osgDB::ReadFileCallback {
public:
    virtual osgDB::ReaderWriter::ReadResult readImage( const std::string& filename, const osgDB::Options* options);
protected:
    osg::Image* getImageByName( const std::string& filename ) {
        ImageMap::iterator itr = _imageMap.find(filename);
        if ( itr!=_imageMap.end() )
            return itr->second.get();
        return NULL;
    }
    typedef std::map<std::string, osg::ref_ptr<osg::Image> > ImageMap;
    ImageMap _imageMap;
};
#endif
