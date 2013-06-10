/**
 * @brief A plugin that extracts images from PDF files with PoDoFo.
 * @author Christian Frisson
 * @date 10/06/2013
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

#ifndef ACPDFPoDoFoImagesExtractThumbnailerPlugin_H
#define	ACPDFPoDoFoImagesExtractThumbnailerPlugin_H

#include <ACPlugin.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

class ACPDFPoDoFoImagesExtractThumbnailerPlugin : public ACThumbnailerPlugin
{
public:
    ACPDFPoDoFoImagesExtractThumbnailerPlugin();
    virtual ~ACPDFPoDoFoImagesExtractThumbnailerPlugin();
    virtual std::vector<std::string> getThumbnailNames();
    virtual std::map<std::string,ACMediaType> getThumbnailTypes();
    virtual std::map<std::string,std::string> getThumbnailDescriptions();
    virtual std::map<std::string,std::string> getThumbnailExtensions();
    virtual std::string requiresMediaReaderPlugin();
    virtual std::vector<std::string> requiresFeaturesPlugins(); // list of plugin names (not paths)
    virtual std::vector<std::string> requiresSegmentationPlugins(); // list of plugin names (not paths)
    virtual std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia);
};

#endif
