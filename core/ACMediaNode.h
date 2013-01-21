/*
 *  ACMediaNode.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 9/03/10
 *  @copyright (c) 2010 – UMONS - Numediart
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

// contains all information about an item that the browser might need

#ifndef __ACMEDIANODE_H__
#define __ACMEDIANODE_H__

#include <vector>
using std::vector;

struct ACPoint
{
    float x, y, z;
    ACPoint():x(0.0f),y(0.0f),z(0.0f){}
    ACPoint(float _x, float _y, float _z):x(_x),y(_y),z(_z){}
};

class ACMediaNode {
private:
    ACPoint currentPos, nextPos, nextPosGrid;
    double nextPosTime;
    float distanceMouse;
    int clusterId; //cluster index
    int active;  // playing or not - and in which mode
    int cursor;
    int frame;
    int navigationLevel; // initially all set to zero, while traversing, only the one incremented are kept
    int hover;
    bool displayed;
    bool selected;
    bool changed;
    long int mediaId;
    //	bool isVisible; == isDisplayed
    vector<int> clickTime;

    void init(long int _mediaId=0);

public:
    ACMediaNode();
    ACMediaNode(long int _mediaId);
    ACMediaNode(long int _mediaId, int _clickTime);
    ~ACMediaNode();

    // previously in ACUserNode:
    int getMediaId() const {return mediaId;}
    // the "==" operator HAS to be defined for tree (see stl_algo.h, line 172)
    bool operator == (const ACMediaNode &other) const;
    void clickNode(long int clickTime);

    int getClusterId() const {return clusterId;}
    int getCursor() const {return cursor;}
    int getCurrentFrame() const {return frame;}
    int getNavigationLevel() const {return navigationLevel;}
    int getActivity() const {return active;}
    int getHover() const {return hover;}
    ACPoint getCurrentPosition() const {return currentPos;}
    // XS X,Y,Z -> float *3 ?
    ACPoint getNextPosition() const {return nextPos;}

    double getNextTime() const {return nextPosTime;}

    ACPoint getNextPositionGrid(){return nextPosGrid;}

    void setCurrentPosition(ACPoint p);
    void setNextPosition(ACPoint p, double t);

    void commitPosition(){currentPos = nextPos;}

    bool getChanged() { return changed; }
    void setChanged(bool changed) { this->changed = changed; }

    // for proximity grid
    void setNextPositionGrid(ACPoint p){nextPosGrid = p;}

    void setActivity(int _act){active = _act;}
    void setClusterId(int _cid) { clusterId=_cid ;}
    void setCursor(int _cid) { cursor=_cid ;}
    void setCurrentFrame(int _cid) { frame=_cid ;}
    void setNavigationLevel(int _nl) {navigationLevel = _nl;}
    void setHover(int _h){hover = _h;}

    void toggleActivity(int type=1);

    bool isDisplayed() const {return displayed;}
    void setDisplayed(bool _dis){displayed = _dis;}

    bool isSelected() const {return selected;}
    void setSelection(bool _sel){selected = _sel;}

    void increaseNavigationLevel(){navigationLevel++;}
};

#endif // __ACMEDIANODE_H__
