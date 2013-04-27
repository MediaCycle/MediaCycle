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

#include "ACOsgReadAndShareImageCallback.h"

using namespace osg;

osgDB::ReaderWriter::ReadResult ACOsgReadAndShareImageCallback::readImage(const std::string& filename, const osgDB::Options* options )
{
    osg::Image* image = getImageByName( filename );
    if ( !image ) {
        osgDB::ReaderWriter::ReadResult rr;
        rr = osgDB::Registry::instance()->readImageImplementation(filename, options);
        if ( rr.success() ){
            _imageMap[filename] = rr.getImage();
            std::cout << "ACOsgReadAndShareImageCallback::readImage: opening image " << filename << std::endl;
        }
        else
            std::cout << "ACOsgReadAndShareImageCallback::readImage: couldn't open image " << filename << std::endl;
        return rr;
    }
    std::cout << "ACOsgReadAndShareImageCallback::readImage: sharing image " << filename << std::endl;
    return image;
}
