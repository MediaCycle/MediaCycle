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

#include "ACPDFPoDoFoImagesExtractThumbnailerPlugin.h"
#include <ACPDFData.h>
#include <ACMedia.h>
#include <iostream>

#include "ImageExtractor.h"

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace PoDoFo;
using namespace std;

ACPDFPoDoFoImagesExtractThumbnailerPlugin::ACPDFPoDoFoImagesExtractThumbnailerPlugin() : ACThumbnailerPlugin(){
    this->mName = "PDF Images Extract Thumbnailer (PoDoFo)";
    this->mDescription ="Plugin for extracting images from PDF files with PoDoFo";
    this->mMediaType = MEDIA_TYPE_PDF;
}

ACPDFPoDoFoImagesExtractThumbnailerPlugin::~ACPDFPoDoFoImagesExtractThumbnailerPlugin(){

}

std::string ACPDFPoDoFoImagesExtractThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "ACPDFPoDoFoReaderPlugin"; //for now
}

std::vector<std::string> ACPDFPoDoFoImagesExtractThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    // No features plugin is required
    return features_plugins;
}

std::vector<std::string> ACPDFPoDoFoImagesExtractThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

std::vector<ACMediaThumbnail*> ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize(ACMedia* media){
    float start_time = getTime();
    float progress = 0.0f;
    std::vector<ACMediaThumbnail*> thumbnails;

    if(!media){
        std::cerr << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: no media to summarize" << std::endl;
        return thumbnails;
    }

    std::string filename = media->getFileName();

    if(filename == ""){
        std::cerr << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: no media filename" << std::endl;
        return thumbnails;
    }


    boost::filesystem::path thumbnail_path;
    boost::filesystem::path media_path(filename.c_str());
    std::string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif
    thumbnail_path = boost::filesystem::path( media_path.parent_path().string() + slash + media_path.stem().string() );
    std::string output_path = thumbnail_path.string();

    std::cout << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: extracting to path '" << output_path << "'" << std::endl;

    int nNum = 0;


    ImageExtractor extractor;
    try {

        extractor.Init( filename.c_str(), output_path.c_str(), &nNum );
    } catch( PdfError & e ) {
        std::cerr << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: an error ocurred during processing the pdf file: " << e.GetError() << std::endl;
        e.PrintErrorMsg();
        return thumbnails;//e.GetError();
    }

    nNum = extractor.GetNumImagesExtracted();

    std::cout << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: extracted " << nNum << " images sucessfully from the PDF file" << std::endl;

    std::vector<std::string> _files = extractor.GetExtractedImageNames();
    for(std::vector<std::string>::iterator _file = _files.begin(); _file != _files.end(); ++ _file){
        ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
        thumbnail->setFileName(*_file);
        thumbnail->setName("PDF Images");
        //thumbnail->setWidth();
        //thumbnail->setHeight();
        //thumbnail->setLength();
        thumbnails.push_back(thumbnail);
    }

    //std::cout << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: done computing waveform(s) for " << filename << " in " << getTime()-waveform_in << " sec." << std::endl;
    progress = 1.0f;

    //thumbnails_specs.clear();
    std::cout << "ACPDFPoDoFoImagesExtractThumbnailerPlugin::summarize: creating thumbnails took " << getTime() - start_time << std::endl;
    return thumbnails;
}

std::vector<std::string> ACPDFPoDoFoImagesExtractThumbnailerPlugin::getThumbnailNames(){
    std::vector<std::string> names;
    names.push_back("PDF Images");
    return names;
}

std::map<std::string,std::string> ACPDFPoDoFoImagesExtractThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    extensions["PDF Images"] = ".jpg";
    return extensions;
}

std::map<std::string,ACMediaType> ACPDFPoDoFoImagesExtractThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    thumbnail_types["PDF Images"] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACPDFPoDoFoImagesExtractThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    thumbnail_descriptions["PDF Images"] = "PDF Images";
    return thumbnail_descriptions;
}
