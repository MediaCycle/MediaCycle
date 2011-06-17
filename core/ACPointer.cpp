/*
 *  ACPointer.cpp
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

#include "ACPointer.h"

using namespace std;

#include <sys/time.h>

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
	
    gettimeofday(&tv, &tz);
	
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

ACPointer::ACPointer(std::string _text,ACPointerType _pointerType)
	: text(""),timeTag(getTime()),closestNode(-1),pointerType(AC_POINTER_UNKNOWN){
	currentPos.x = 0.0f;
	currentPos.y = 0.0f;
	currentPos.z = 0.0f;
	pointerType = _pointerType;
	text = _text;
}

void ACPointer::setCurrentPosition(ACPoint p){
	currentPos = p;
}

ACPoint ACPointer::getCurrentPosition(){
	return this->currentPos;
}

void ACPointer::setText(string t){
	this->text = t;
}

string ACPointer::getText(){
	return this->text;
}

double ACPointer::getTimeTag(){
	return this->timeTag;
}

void ACPointer::resetTimeTag(){
	this->timeTag = getTime();
}

int ACPointer::getClosestNode(){
	return this->closestNode;
}

void ACPointer::setClosestNode(int _closestNode){ 
	this->closestNode = _closestNode;
}

ACPointerType ACPointer::getType(){ 
	return this->pointerType;
}

void ACPointer::setType(ACPointerType _type){ 
	this->pointerType = _type;
}