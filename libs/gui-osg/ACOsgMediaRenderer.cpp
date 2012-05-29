/*
 *  ACOsgBrowserRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "ACOsgMediaRenderer.h"
using namespace osg;

ACOsgMediaRenderer::ACOsgMediaRenderer() {
	media_type = MEDIA_TYPE_NONE;
        setting = AC_SETTING_NONE;
	media = 0;
	media_changed = false;
	local_group = new Group(); 
#ifdef AUTO_TRANSFORM
	media_node = new AutoTransform();
#else
	media_node = new MatrixTransform();
#endif
	node_color = Vec4(1,1,0.5,1); //CF seminal yellow
	cluster_colors.push_back(osg::Vec4(1,1,0.5,1));
	cluster_colors.push_back(osg::Vec4(1,0.5,1,1));
	cluster_colors.push_back(osg::Vec4(0.5,1,1,1));
	cluster_colors.push_back(osg::Vec4(1,0.5,0.5,1));
	cluster_colors.push_back(osg::Vec4(0.5,1,0.5,1));
	user_defined_color = false;
	
	// SD - Results from centralized request to MediaCycle - GLOBAL
	media_cycle_deltatime = 0.0;
	media_cycle_zoom = 0.0f;
	media_cycle_angle = 0.0f;
	media_cycle_mode = 0;
	media_cycle_global_navigation_level = 0;
	
	// SD - Results from centralized request to MediaCycle - NODE SPECIFIC
	media_cycle_isdisplayed = false;
	media_cycle_current_pos.x = 0;
	media_cycle_current_pos.y = 0;
	media_cycle_current_pos.z = 0;
	media_cycle_view_pos.x = 0;
	media_cycle_view_pos.y = 0;
	media_cycle_view_pos.z = 0;
	media_cycle_next_pos.x = 0;
	media_cycle_next_pos.y = 0;
	media_cycle_next_pos.z = 0;
	media_cycle_navigation_level = 0;
	media_cycle_activity = 0;
	media_cycle_filename = "";
	media_index = -1;
	cluster_index = -1;
	prev_media_index = -1;	
	
#if defined(APPLE_IOS)
	 afac = 0.5;
#else
	 afac = 1.0;
#endif
	initialized = 0;
	frac = 0.0f;
	local_group->addChild(media_node);

        //CF this should be done once per application runtime, for instance in the browser renderer or composite viewer
        font = 0;
        std::string font_path(""),font_file("fudd.ttf");
    #ifdef USE_DEBUG
        boost::filesystem::path s_path( __FILE__ );
        font_path = s_path.parent_path().parent_path().parent_path().string() + "/data/fonts/";
    #else
    #ifdef __APPLE__
        font_path = "@executable_path/../Resources/fonts/";
    #elif __WIN32__
        font_path = "./";
    #else
        font_path = "/usr/share/mediacycle/fonts/";
    #endif
    #endif
        std::cout << "Current font path " << font_path << std::endl;

        font = osgText::readRefFontFile(font_path + font_file);
        if(!font)
            std::cerr << "ACOsgLibraryRenderer::ACOsgLibraryRenderer: couldn't load font " << std::endl;

}

ACOsgMediaRenderer::~ACOsgMediaRenderer() {
	media = 0;// the core will free media files
	cluster_colors.clear();
}

void ACOsgMediaRenderer::setDeltaTime(double media_cycle_deltatime) {
	this->media_cycle_deltatime = media_cycle_deltatime;
}

void ACOsgMediaRenderer::setZoomAngle(float media_cycle_zoom, float media_cycle_angle) {
	this->media_cycle_zoom = media_cycle_zoom;
	this->media_cycle_angle = media_cycle_angle;
}

void ACOsgMediaRenderer::setMode(int media_cycle_mode) {
	this->media_cycle_mode = media_cycle_mode;
}

void ACOsgMediaRenderer::setGlobalNavigation(int media_cycle_global_navigation_level) {
	this->media_cycle_global_navigation_level = media_cycle_global_navigation_level;
}

void ACOsgMediaRenderer::setIsDisplayed(int media_cycle_isdisplayed) {
	this->media_cycle_isdisplayed = media_cycle_isdisplayed;
}

/*
void ACOsgMediaRenderer::setPos(ACPoint &media_cycle_current_pos, ACPoint &media_cycle_next_pos) {
	this->media_cycle_current_pos = media_cycle_current_pos;
	this->media_cycle_next_pos = media_cycle_next_pos;
}
*/

void ACOsgMediaRenderer::setViewPos(ACPoint media_cycle_view_pos) {
	this->media_cycle_view_pos = media_cycle_view_pos;
}

void ACOsgMediaRenderer::setCurrentPos(ACPoint media_cycle_current_pos) {
	this->media_cycle_current_pos = media_cycle_current_pos;
	initialized = 1;
}

void ACOsgMediaRenderer::setNextPos(ACPoint media_cycle_next_pos) {
	this->media_cycle_next_pos = media_cycle_next_pos;
}

ACPoint ACOsgMediaRenderer::getViewPos() {
	return this->media_cycle_view_pos;
}

ACPoint ACOsgMediaRenderer::getCurrentPos() {
	return this->media_cycle_current_pos;
}

ACPoint ACOsgMediaRenderer::getNextPos() {
	return this->media_cycle_next_pos;
}

void ACOsgMediaRenderer::setNavigation(int media_cycle_navigation_level) {
	this->media_cycle_navigation_level = media_cycle_navigation_level;
}

void ACOsgMediaRenderer::setActivity(int media_cycle_activity) {
	this->media_cycle_activity = media_cycle_activity;	
}

/*void ACOsgMediaRenderer::setMediaIndex(int media_index) {
	this->media_index = media_index;
}*/

void ACOsgMediaRenderer::setMedia(ACMedia* _media) {
	this->media = _media;
	media_changed = true;
}

void ACOsgMediaRenderer::setFilename(std::string media_cycle_filename) {
	this->media_cycle_filename = media_cycle_filename;
}

