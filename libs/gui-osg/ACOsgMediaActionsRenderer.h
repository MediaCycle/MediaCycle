/**
 * @brief A class that displays the last media action performed.
 * @author Christian Frisson
 * @date 05/05/2013
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

#ifndef __ACOsgMediaActionsRenderer_H__
#define __ACOsgMediaActionsRenderer_H__

#include "ACOsgMediaRenderer.h"

#include "ACOsgLibraryRenderer.h"

class ACOsgMediaActionsRenderer : public ACOsgMediaRenderer, public ACEventListener  {
protected:

    ACOsgLibraryTextRenderer media_action_renderer;

    osg::ref_ptr<osg::MatrixTransform> media_action_node;

    void textGeode(std::string _string, osg::ref_ptr<osgText::Text>& _text, osg::ref_ptr<osg::Geode>& _geode,osg::Vec3 pos);
    void init();

public:
    ACOsgMediaActionsRenderer();
    ~ACOsgMediaActionsRenderer();
    void prepareNodes();
    void updateNodes(double ratio=0.0);
    void updateSize(int w, int h);

    virtual void mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());

    void updateTextRenderer(ACOsgLibraryTextRenderer& _renderer, std::string _value, osg::ref_ptr<osg::MatrixTransform> node );
    virtual void changeSetting(ACSettingType _setting);

    virtual void setMediaCycle(MediaCycle *_media_cycle);

private:
    float max_side_size; // of the cover, in pixels
    float font_size; // in pixels
    float line_sep; // in pixels
    int width,height;
    double last_time, refresh;
};

#endif
