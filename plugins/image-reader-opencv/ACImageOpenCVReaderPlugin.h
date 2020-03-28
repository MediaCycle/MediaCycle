/**
 * @brief A plugin that provides media and media data instances to parse and read images using OpenCV.
 * @author Christian Frisson
 * @date 12/03/2015
 * @copyright (c) 2015 – UMONS - Numediart
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

#include <ACPlugin.h>

#ifndef ACImageOpenCVReaderPlugin_H
#define	ACImageOpenCVReaderPlugin_H

// Since there is no way to check supported file extensions with OpenCV,
// and that we currently use both OpenCV and OSG with their FFmpeg backend/plugin
// we assume OSG can list the supported image file extensions

class ACImageOpenCVReaderPlugin : public ACMediaReaderPlugin
{
public:
    ACImageOpenCVReaderPlugin();
    virtual ~ACImageOpenCVReaderPlugin();
    virtual ACMedia* mediaFactory(ACMediaType mediaType, const ACMedia* media = 0);
    virtual ACMediaData* mediaReader(ACMediaType mediaType);
    std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL);
};

#endif
