 /*
 *  ACOsgLibraryRenderer.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 04/05/2012
 *
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

#ifndef __ACOSG_LIBRARY_RENDERER_H__
#define __ACOSG_LIBRARY_RENDERER_H__

#include "ACOsgMediaRenderer.h"

class ACOsgLibraryRenderer : public ACOsgMediaRenderer  {
protected:

    osg::ref_ptr<osgText::Text> title_text,author_text,curator_text,year_text,license_text,publisher_text,website_text,medias_text;
    osg::ref_ptr<osg::Geode> title_geode,author_geode,curator_geode,year_geode,license_geode,publisher_geode,website_geode,medias_geode;
    std::string title,author,curator,year,publisher,license,website,cover;
    std::string title_caption,author_caption,curator_caption,year_caption,publisher_caption,license_caption,website_caption,medias_caption;
    int medias;
    float title_x,title_y,author_x,author_y,curator_x,curator_y,year_x,year_y,publisher_x,publisher_y,license_x,license_y,website_x,website_y,cover_x,cover_y,medias_x,medias_y;

    osg::ref_ptr<osg::Image> image_image;
    osg::ref_ptr<osg::Geode> image_geode;
    osg::ref_ptr<osg::Geode> border_geode;
    osg::ref_ptr<osg::MatrixTransform> image_transform;
    osg::ref_ptr<osg::Image> thumbnail;
    osg::ref_ptr<osg::Texture2D> image_texture;

    void textGeode(std::string _string, osg::ref_ptr<osgText::Text>& _text, osg::ref_ptr<osg::Geode>& _geode,osg::Vec3 pos);
    void imageGeode(bool flip=false, float sizemul=1.0, float zoomin=1.0);
    void init();

public:
    ACOsgLibraryRenderer();
    ~ACOsgLibraryRenderer();
    void prepareNodes();
    void updateNodes(double ratio=0.0);
    void updateSize(int w, int h);
    void setTitle(std::string _title);
    void setAuthor(std::string _author);//{this->author=_author;}
    void setCurator(std::string _curator){this->curator=_curator;}
    void setYear(std::string _year){this->year=_year;}
    void setPublisher(std::string _publisher);//{this->publisher=_publisher;}
    void setLicense(std::string _license){this->license=_license;}
    void setWebsite(std::string _website){this->website=_website;}
    void setCover(std::string _cover);
    void setNumberOfMedia(int _number);
    //virtual osg::ref_ptr<osg::Geode> getMainGeode() { return image_geode;}
    virtual void changeSetting(ACSettingType _setting);

private:
    float max_side_size; // of the cover, in pixels
    float font_size; // in pixels
    float line_sep; // in pixels
};

#endif
