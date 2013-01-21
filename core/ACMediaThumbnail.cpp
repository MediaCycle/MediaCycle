/**
 * @brief Base media thumbnail class
 * @author Christian Frisson
 * @date 7/10/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACMediaThumbnail.h"

#include <iostream>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;

ACMediaThumbnail::ACMediaThumbnail()
{
    this->init();
}

ACMediaThumbnail::ACMediaThumbnail(ACMediaType _type)
{
    this->init();
    this->media_type = _type;
}

void ACMediaThumbnail::init(){
    this->mid = -1;
    this->parentid = -1;
    this->media_type = MEDIA_TYPE_NONE;
    this->vectorial = false;
    this->circular = false;
    this->height = 0;
    this->width = 0;
    this->filename = "";
    this->description = "";
    this->name = "";
    this->start = 0.0f;
    this->end = 0.0f;
    this->startInt = 0;
    this->endInt = 0;
    this->discarded = false;
}

ACMediaThumbnail::~ACMediaThumbnail()
{
    for (std::vector<ACMediaThumbnail*>::iterator segment = segments.begin(); segment != segments.end();segment++) {
        delete *segment;
    }
    segments.clear();
}

void ACMediaThumbnail::saveXML(TiXmlElement* media){
    if (media == NULL) return;
    media->SetAttribute("Id", mid);
    media->SetAttribute("MediaId", parentid);
    media->SetAttribute("MediaType", media_type);
    media->SetAttribute("Vectorial",vectorial);
    media->SetAttribute("Circular",circular);
    media->SetAttribute("Height", height);
    media->SetAttribute("Width", width);
    media->SetAttribute("Length", length);
    media->SetAttribute("FileName", filename);
    media->SetAttribute("Name", name);
    media->SetAttribute("Description", description);
    media->SetDoubleAttribute("Start", start);
    media->SetDoubleAttribute("End", end);
    if(this->getNumberOfSegments()>0){
        TiXmlElement* _segments = new TiXmlElement( "Segments" );
        media->LinkEndChild( _segments );
        _segments->SetAttribute("NumberOfSegments", this->getNumberOfSegments());
        // saves info about segments (if any) : beginning, end, ID
        // the parent ID of the segment is the ID of the current media
        for (std::vector<ACMediaThumbnail*>::iterator segment = segments.begin(); segment != segments.end();segment++) {
            TiXmlElement* seg = new TiXmlElement( "Segment" );
            _segments->LinkEndChild( seg );
            (*segment)->saveXML(seg);
        }
    }
}

void ACMediaThumbnail::loadXML(TiXmlElement* _pMediaNode){

    if (!_pMediaNode)
        throw runtime_error("corrupted XML file, couldn't access thumbnail xml");

    int pId=-1;
    _pMediaNode->QueryIntAttribute("Id", &pId); // If this fails, original value is left as-is
    if (pId < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail ID");
    else
        this->setId(pId);

    int pParentId=-1;
    _pMediaNode->QueryIntAttribute("MediaId", &pParentId); // If this fails, original value is left as-is
    if (pParentId < 0)
        throw runtime_error("corrupted XML file, wrong parent media ID for thumbnail");
    else
        this->setParentId(pParentId);

    int pMediaType= -1;
    _pMediaNode->QueryIntAttribute("MediaType", &pMediaType); // If this fails, original value is left as-is
    if (pMediaType < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail media type");
    else
        this->media_type = (ACMediaType)pMediaType;

    int pVectorial= -1;
    _pMediaNode->QueryIntAttribute("Vectorial", &pVectorial); // If this fails, original value is left as-is
    if (pVectorial < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail vectorial status");
    else
        this->vectorial = (bool)pVectorial;

    int pCircular= -1;
    _pMediaNode->QueryIntAttribute("Circular", &pCircular); // If this fails, original value is left as-is
    if (pCircular < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail circular status");
    else
        this->circular = (bool)pCircular;

    int pHeight=-1;
    _pMediaNode->QueryIntAttribute("Height", &pHeight); // If this fails, original value is left as-is
    if (pHeight < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail height");
    else
        this->setHeight(pHeight);

    int pWidth=-1;
    _pMediaNode->QueryIntAttribute("Width", &pWidth); // If this fails, original value is left as-is
    if (pWidth < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail width");
    else
        this->setWidth(pWidth);

    int pLength=-1;
    _pMediaNode->QueryIntAttribute("Length", &pLength); // If this fails, original value is left as-is
    if (pLength < 0)
        throw runtime_error("corrupted XML file, wrong thumbnail length");
    else
        this->setLength(pLength);


    string pName ="";
    pName = _pMediaNode->Attribute("Name");
    if (pName == "")
        throw runtime_error("corrupted XML file, no thumbnail name");
    else
        this->setName(pName);

    string pDescription ="";
    pDescription = _pMediaNode->Attribute("Description");
//    if (pDescription == "")
//        throw runtime_error("corrupted XML file, no thumbnail description");
//    else
        this->setDescription(pDescription);

    double n_start=-1;
    _pMediaNode->QueryDoubleAttribute("Start", &n_start);
    if (n_start < 0) {
        throw runtime_error("corrupted XML file, wrong segment start");
    }
    this->setStart(n_start);

    double n_end=-1;
    _pMediaNode->QueryDoubleAttribute("End", &n_end);
    if (n_end < 0){
        throw runtime_error("corrupted XML file, wrong segment end");
    }
    this->setEnd(n_end);

    TiXmlHandle _pMediaNodeHandle(_pMediaNode);
    TiXmlElement* segmentsElement = _pMediaNodeHandle.FirstChild( "Segments" ).Element();
    if (segmentsElement) {
        int ns = -1;
        segmentsElement->QueryIntAttribute("NumberOfSegments", &ns);
        if (ns < 0)
            throw runtime_error("corrupted XML file, <segments> present, but no segments");

        TiXmlElement* segmentElement = _pMediaNodeHandle.FirstChild( "Segments" ).FirstChild( "Segment" ).Element();
        TiXmlText* segmentIDElementsAsText = 0;
        int count_s = 0;

        for( segmentElement; segmentElement; segmentElement = segmentElement->NextSiblingElement() ) {
            ACMediaThumbnail* segment_media = new ACMediaThumbnail();
            segment_media->loadXML(segmentElement);
        }
    }
    else{
        string pName ="";
        pName = _pMediaNode->Attribute("FileName");
        if (pName == "")
            throw runtime_error("corrupted XML file, no thumbnail filename");
        else {
            // #ifdef __APPLE__ //added by CF, white spaces are needed under Ubuntu!
            // fixWhiteSpace(pName);
            // #endif
            fs::path p( pName.c_str());
            if ( !fs::exists( p ) )
            {
                throw runtime_error("corrupted XML file, can't locate thumbnail file '" + pName + "'");
            }
            if ( !fs::is_regular( p ) )
            {
                throw runtime_error("corrupted XML file, thumbnail file '" + pName + "' is corrupted");
            }
            this->setFileName(pName);
        }

    }

    // loadXMLSpecific should throw an exception if a problem occured
    // but to make sure we'll throw one as well if return value is wrong
    /*if (!loadXMLSpecific(_pMediaNode)) {
        throw runtime_error("corrupted XML file, problem with loadXMLSpecific");
    }*/
}

void ACMediaThumbnail::addSegment(ACMediaThumbnail* _segment)
{
    segments.push_back(_segment);
    _segment->setParentId(this->getParentId());
    _segment->setId(segments.size()-1);
}
