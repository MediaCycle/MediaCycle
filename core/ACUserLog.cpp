/*
 *  ACMediaUserlog.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont, Damien Tardieu
 *  @date 25/02/10
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

// Check http://tree.phi-sci.com/

#include "ACUserLog.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

ACUserNode::ACUserNode(long int nodeId, long int mediaId, int clickTime) {
	this->nodeId = nodeId;
	this->mediaId = mediaId;
	this->isVisible = 1;
	clickNode(clickTime);// CF clicked when constructed?!
}

ACUserNode::~ACUserNode() {
}

bool ACUserNode::operator==(const ACUserNode &other) const {
    return (this->nodeId == other.nodeId);
}

void ACUserNode::clickNode(long int clickTime) {
	(this->clickTime).push_back(clickTime);
}

///////////////////////////////////////////////////////////////////////////////

ACUserLog::ACUserLog() {
	mNodeId = 0;
	mLastClickedNodeId = -1;
}

ACUserLog::~ACUserLog() {
	
}

long int ACUserLog::addNode(long int parentId, long int mediaId, int clickTime) {
	
	tree<ACUserNode>::iterator location;
	ACUserNode *tmpNode;
	ACUserNode *userNode;
	
	tmpNode = new ACUserNode(parentId, 0, 0);
	userNode = new ACUserNode(mNodeId, mediaId, clickTime);
	mNodeId++;
	
	location = find(userLogTree.begin(), userLogTree.end(), *tmpNode);
	
	userLogTree.append_child(location, *userNode);
	
	delete userNode;
	delete tmpNode;
	
	return mNodeId;
}

void ACUserLog::clickNode(long int nodeId, long int clickTime) {
	
	tree<ACUserNode>::iterator location;
	ACUserNode *tmpNode;
	
	tmpNode = new ACUserNode(nodeId, 0, 0);
	
	// XS what if it does not find it ?
	location = find(userLogTree.begin(), userLogTree.end(), *tmpNode);
	
	(*location).clickNode(clickTime);
	
	delete tmpNode;
	
	mLastClickedNodeId = nodeId;
}

///////////////////////////////////////////////////////////////////////////////
