/*
 *  ACPDF.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 27/05/2011
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

#ifndef ACPDF_H
#define ACPDF_H

#include <ostream>
#include <podofo/podofo.h>

#include "ACMedia.h"
#include "ACPDFData.h"
#include <string>

#include <osg/ImageStream>
#include <osg/Texture2D>

class ACPDF: public ACMedia {
    // contains the *minimal* information about a PDF
public:
    ACPDF();
    ACPDF(const ACPDF& m);
    ~ACPDF();
private:
    void init();

public:
    void saveACLSpecific(std::ofstream &library_file);
    int loadACLSpecific(std::ifstream &library_file);
    void saveXMLSpecific(TiXmlElement* _media);
    int loadXMLSpecific(TiXmlElement* _pMediaNode);

    //void setThumbnail(IplImage *_thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->width; thumbnail_height = _thumbnail->height; }
    osg::ref_ptr<osg::Image> getThumbnail() { return thumbnail; }
    osg::ref_ptr<osg::Texture2D> getTexture() { return image_texture; }

    bool extractData(std::string fname);

    int getPageCount(){return page_count;}
    std::string getAuthor(){return author;}
    std::string getCreator(){return creator;}
    std::string getSubject(){return subject;}
    std::string getTitle(){return title;}
    std::string getKeywords(){return keywords;}

private:
    bool computeThumbnail(int w=0, int h=0);
    bool computeThumbnailSize(int w_, int h_);

private:
    static const int default_thumbnail_area;
    int thumbnail_width, thumbnail_height;
    osg::ref_ptr<osg::Image> thumbnail;
    osg::ref_ptr<osg::Texture2D> image_texture;
    //	ACPDFData* data;

    std::string author,creator,subject,title,keywords,format_unit;
    int page_count;
};
#endif // ACPDF_H
