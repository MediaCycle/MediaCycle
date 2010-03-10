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

// replaces ACLoopAttribute AND ACUserNode
// contains all information about an item that the browser might need
// at least in theory...

#ifndef __ACMEDIANODE_H__
#define __ACMEDIANODE_H__

#include <vector>
using std::vector;

struct ACPoint
{
	float x, y, z;
};

class ACMediaNode {
private:
	// previsously in ACLoopAttribute:
	ACPoint 	currentPos, nextPos, nextPosGrid;
	ACPoint		viewPos;
	float		distanceMouse;
	int 		clusterId; //cluster index
	int			active;  // playing or not - and in which mode
	int			cursor;
	int 		navigationLevel; // initially all set to zero, while traversing, only the one incremented are kept
	int			hover;
	bool		displayed;
	
	// previously in ACUserNode:
	long int nodeId;
	long int mediaId;
//	bool isVisible; == isDisplayed
	vector<int> clickTime;
	
public:
	ACMediaNode();
	~ACMediaNode();
	
//	bool operator == (const ACUserNode &other) const; // ambiguous : nodeID or media ID
	void clickNode(long int clickTime);
	
	int getNodeId() const {return nodeId;}
	int getMediaId() const {return mediaId;}
	int getClusterId() const {return clusterId;}
	int getCursor() const {return cursor;}
	int getNavigationLevel() const {return navigationLevel;}
	int getActivity() const {return active;}
	ACPoint getCurrentPosition() const {return currentPos;}
	// XS X,Y,Z -> float *3 ?
	float getCurrentPositionX() const {return currentPos.x;}
	float getCurrentPositionY() const {return currentPos.y;}
	float getCurrentPositionZ() const {return currentPos.z;}

	ACPoint getNextPosition() const {return nextPos;}
	float getNextPositionX() const {return nextPos.x;}
	float getNextPositionY() const {return nextPos.y;}
	float getNextPositionZ() const {return nextPos.z;}

	ACPoint getNextPositionGrid(){return nextPosGrid;}

	void setCurrentPosition(float x, float y, float z);
	void setCurrentPosition(ACPoint p);
	void setNextPosition(float x, float y, float z);
	void setNextPosition(ACPoint p);
	void setNextPositionX(float x);
	void setNextPositionY(float y);
	void setNextPositionZ(float z);

	void commitPosition(){currentPos = nextPos;}

	// for proximity grid
	void setNextPositionGrid(ACPoint p){nextPosGrid = p;}

	void setActivity(int _act){active = _act;}
	void setClusterId(int _cid) { clusterId=_cid ;}
	void setCursor(int _cid) { cursor=_cid ;}
	void setNavigationLevel(int _nl) {navigationLevel = _nl;}
	void setHover(int _h){hover = _h;}
	
	bool isDisplayed() const {return displayed;}
	void setDisplayed(bool _dis){displayed = _dis;}

	void increaseNavigationLevel(){navigationLevel++;}
};

#endif // __ACMEDIANODE_H__
