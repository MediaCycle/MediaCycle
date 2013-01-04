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

#ifndef ACMEDIATHUMBNAIL_H
#define ACMEDIATHUMBNAIL_H

#include "ACMediaTypes.h"
//#include "ACPluginManager.h"
class ACPluginManager;

#include <string>
#include <vector>
#define TIXML_USE_STL
#include <tinyxml.h>

class ACMediaThumbnail {
    // contains the minimal information about a media thumbnail
protected:

    int mid;
    int parentid; //CF so that segments can be defined as ACMedia having other ACMedia as parents
    ACMediaType media_type;
    bool vectorial,circular;
    int height, width, length;
    std::string filename;
    std::string description;
    std::string name; // identifier
    std::vector<ACMediaThumbnail*> segments;
    float start, end; // seconds
    int startInt, endInt; // frame numbers
    bool discarded; // allows to discard medias to be saved in libraries

public:
    ACMediaThumbnail();
    ACMediaThumbnail(ACMediaType _type);
    virtual ~ACMediaThumbnail(); // make this virtual to ensure that destructors of derived classes will be called
protected:
    void init();

public:
    ACMediaType getMediaType() {return media_type;}
    bool isVectorial(){return vectorial;}
    bool isCircular(){return circular;}
    void setCircular(bool _circular){this->circular = _circular;}
    virtual bool requiresRegeneration(){return false;}

    void setId(int _id) {mid = _id;}
    int getId() {return mid;}
    void setParentId(int _parentid) {parentid = _parentid;} //CF so that segments can be defined as ACMedia having other ACMedia as parents
    int getParentId() {return parentid;}

    double getDuration(){return this->getEnd()-this->getStart();}

    void addSegment(ACMediaThumbnail* _segment){segments.push_back(_segment);}
    //void removeSegment(ACMedia* _segment){segments.erase(_segment);}
    std::vector<ACMediaThumbnail*> &getAllSegments() { return segments; }
    void setAllSegments(std::vector<ACMediaThumbnail*> _segments) { segments=_segments; }
    void deleteAllSegments() { segments.clear();}
    ACMediaThumbnail* getSegment(int i) { return segments[i]; }
    int getNumberOfSegments(){return segments.size();}

    //virtual std::vector<std::string> getListOfFeaturesPlugins();

    std::string getFileName() { return filename; }
    void setFileName(std::string s) { filename = s; }
    void setFileName(const char* c) { std::string s(c); filename = s; }

    std::string getName() { return name; }
    void setName(std::string k) { name = k; }

    std::string getDescription(void){return description;}
    void setDescription(std::string iDescription){description=iDescription;}

    bool isDiscarded(){return discarded;}
    void setDiscarded(bool _discarded){discarded=_discarded;}

    // accessors -- these should not be redefined for each media
    int getWidth() {return width;}
    int getHeight() {return height;}
    int getLength() {return length;}
    void setWidth(int w) {width=w;}
    void setHeight(int h) {height=h;}
    void setLength(int l) {length=l;}
    ACMediaType getType() {return this->media_type;}

    // beginning and end as floats
    void setStart(float st){this->start = st;}
    void setEnd(float en){this->end = en;}
    float getStart(){return this->start;}
    float getEnd(){return this->end;}
    void setStartInt(int sti){this->startInt = sti;}
    void setEndInt(int eni){this->endInt = eni;}
    float getStartInt(){return this->startInt;}
    float getEndInt(){return this->endInt;}
    virtual float getFrameRate() {return 0;}//CF video
    virtual int getSampleRate() {return 0;}//CF audio - merge both?

    // I/O -- common part
    void saveXML(TiXmlElement* _medias);
    void loadXML(TiXmlElement* _pMediaNode);
    
    // I/O -- media-specific part
    //virtual void saveXMLSpecific(TiXmlElement* _media){}
    //virtual int loadXMLSpecific(TiXmlElement* _pMediaNode){return -1;}
};

#endif // ACMEDIATHUMBNAIL_H
