/*
 *  ACMediaNode.cpp
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

#include "ACMediaNode.h"

ACMediaNode::ACMediaNode(){
	clusterId = 0;
	active = 0;
	cursor = 0; 
	navigationLevel = 0;
	hover = 0;
	displayed = false;
	
	currentPos.x = 0;
	currentPos.y = 0;
	currentPos.z = 0;
	
	nextPos.x = 0;
	nextPos.y = 0;
	nextPos.z = 0;

	nextPosGrid.x = 0;
	nextPosGrid.y = 0;
	nextPosGrid.z = 0;

	viewPos.x = 0;
	viewPos.y = 0;
	viewPos.z = 0;
}

ACMediaNode::~ACMediaNode(){
}

void ACMediaNode::setCurrentPosition(float x, float y, float z){
	currentPos.x = x;
	currentPos.y = y;
	currentPos.z = z;
}

void ACMediaNode::setCurrentPosition(ACPoint p){
	currentPos = p;
}

void ACMediaNode::setNextPosition(float x, float y, float z){
	nextPos.x = x;
	nextPos.y = y;
	nextPos.z = z;
}

void ACMediaNode::setNextPosition(ACPoint p){
	nextPos = p;
}

void ACMediaNode::setNextPositionX(float x){
	nextPos.x = x;
}

void ACMediaNode::setNextPositionY(float y){
	nextPos.y = y;
}

void ACMediaNode::setNextPositionZ(float z){
	nextPos.z = z;
}

