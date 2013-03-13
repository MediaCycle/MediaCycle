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

struct ACOsgLibraryTextRenderer {
    osg::ref_ptr<osgText::Text> text;
    osg::ref_ptr<osg::Geode> geode;
    std::string value;
    std::string caption;
    float x,y;
    ACOsgLibraryTextRenderer() : text(0),geode(0),value(""),caption(""),x(0.0f),y(0.0f){}
    ~ACOsgLibraryTextRenderer(){text = 0; geode = 0;}
};

struct ACOsgLibraryImageRenderer {
    osg::ref_ptr<osg::MatrixTransform> transform;
    std::string file;
    float x,y;
    ACOsgLibraryImageRenderer() : transform(0),file(""),x(0.0f),y(0.0f){}
    ~ACOsgLibraryImageRenderer(){transform = 0;}
};

class ACOsgLibraryRenderer : public ACOsgMediaRenderer  {
protected:

    ACOsgLibraryTextRenderer library_title,library_author,library_year,library_publisher,library_license,library_website,library_medias_number;
    ACOsgLibraryTextRenderer curator_name,curator_email,curator_website,curator_location;
    ACOsgLibraryImageRenderer library_cover,curator_picture;

    osg::ref_ptr<osg::MatrixTransform> library_node;
    osg::ref_ptr<osg::MatrixTransform> curator_node;

    void textGeode(std::string _string, osg::ref_ptr<osgText::Text>& _text, osg::ref_ptr<osg::Geode>& _geode,osg::Vec3 pos);
    void imageGeode(ACOsgLibraryImageRenderer& _renderer);
    void init();

public:
    ACOsgLibraryRenderer();
    ~ACOsgLibraryRenderer();
    void prepareNodes();
    void updateNodes(double ratio=0.0);
    void updateSize(int w, int h);

    void updateTextRenderer(ACOsgLibraryTextRenderer& _renderer, std::string _value, osg::ref_ptr<osg::MatrixTransform> node );
    void updateImageRenderer(ACOsgLibraryImageRenderer& _renderer, std::string _file, osg::ref_ptr<osg::MatrixTransform> node);
    //virtual osg::ref_ptr<osg::Geode> getMainGeode() { return image_geode;}
    virtual void changeSetting(ACSettingType _setting);

private:
    float max_side_size; // of the cover, in pixels
    float font_size; // in pixels
    float line_sep; // in pixels
    int width,height;
    float progress;
};

#endif
