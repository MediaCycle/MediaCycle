/*
 *  ACPointer.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 29/05/2011
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

#ifndef __ACPOINTER_H__
#define __ACPOINTER_H__

#include <vector>
#include <string>
#include <map>

#include "ACMediaNode.h"  // this contains ACPoint

enum ACPointerType {
	AC_POINTER_UNKNOWN=0,
	AC_POINTER_MOUSE=1, //any other single-pointer device
	AC_POINTER_FINGER=2, //multi-finger/touch devices such as trackpads
	AC_POINTER_BODY=3, //using crowd-tracking devices such as the Kinect
};

class ACPointer {
	
private:
	ACPoint currentPos;
	std::string text;
	double timeTag;
	int closestNode;
	ACPointerType pointerType;
	
public:
	ACPointer(std::string _text="",ACPointerType _pointerType=AC_POINTER_UNKNOWN);
	~ACPointer(){};
	
	void setCurrentPosition(ACPoint p);
	void setCurrentPosition(double xx,double yy);
	ACPoint getCurrentPosition();
	void setText(std::string t);
	std::string getText();
	double getTimeTag();
	void resetTimeTag();
	int getClosestNode();
	void setClosestNode(int _closestNode);
	ACPointerType getType();
	void setType(ACPointerType _type);
};

//typedef vector<ACPointer> ACPointers;
typedef std::map< int, ACPointer* > ACPointers;

#endif //__ACPOINTER_H__