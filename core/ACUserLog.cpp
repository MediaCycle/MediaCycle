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

ACUserNode::ACUserNode(long int _nodeId, long int _mediaId, int _clickTime) {
	this->nodeId = _nodeId;
	this->mediaId = _mediaId;
	this->isVisible = true;
	clickNode(_clickTime);// CF clicked when constructed?!
}

ACUserNode::~ACUserNode() {
}

bool ACUserNode::operator==(const ACUserNode &other) const {
    return (this->nodeId == other.nodeId);
}

void ACUserNode::clickNode(long int _clickTime) {
	(this->clickTime).push_back(_clickTime);
}

///////////////////////////////////////////////////////////////////////////////

ACUserLog::ACUserLog() {
	mNodeId = 0;
	mLastClickedNodeId = -1;
}

ACUserLog::~ACUserLog() {
	
}

void ACUserLog::addRootNode(long int _mediaId, int _clickTime) {
	
	ACUserNode *userNode;
	userNode = new ACUserNode(0, _mediaId, _clickTime);
	
	userLogTree.set_head(*userNode);
	//location.insert(*userNode);

	mNodeId++;
	
	delete userNode;
}


long int ACUserLog::addNode(long int _parentId, long int _mediaId, int _clickTime) {
	
	tree<ACUserNode>::iterator location;

	ACUserNode *userNode;
	userNode = new ACUserNode(mNodeId, _mediaId, _clickTime);

	
	ACUserNode *tmpNode;
	tmpNode = new ACUserNode(_parentId, 0, 0);

	mNodeId++;

	location = find(userLogTree.begin(), userLogTree.end(), *tmpNode);
	//location = userLogTree.tree_find_depth(*tmpNode);
	
	userLogTree.append_child(location, *userNode);
	//location.insert(*userNode);
	
	delete userNode;
	delete tmpNode;
	
	return mNodeId;
}

void ACUserLog::clickNode(long int _nodeId, long int _clickTime) {
	
	tree<ACUserNode>::iterator location;
	ACUserNode *tmpNode;
	
	tmpNode = new ACUserNode(_nodeId, 0, 0);
	
	// XS what if it does not find it ?
	location = find(userLogTree.begin(), userLogTree.end(), *tmpNode);
	//location = userLogTree.tree_find_depth(*tmpNode);
	
	(*location).clickNode(_clickTime);
	
	delete tmpNode;
	
	mLastClickedNodeId = _nodeId;
}

long int ACUserLog::getMediaIdFromNodeId(long int _nodeId) {
	long int _mediaId = -1;
	if ((_nodeId >=0) && (_nodeId <= mNodeId)){
		tree<ACUserNode>::iterator node = userLogTree.begin();		
		while( (node!=userLogTree.end()) ) { //(_nodeId != node->getNodeId()) && 
			//std::cout << "Node Id loop" << _nodeId << " node " << node->getNodeId() << std::endl;
			if (node->getNodeId() == _nodeId) {
				_mediaId = node->getMediaId();
				break;
			}	
			++node;
		}
	}
	return _mediaId;
}	

void ACUserLog::dump(){
	tree<ACUserNode>::iterator loc  = userLogTree.begin();
	tree<ACUserNode>::iterator end  = userLogTree.end();
	
	int rootdepth=userLogTree.depth(loc);
	
	std::cout << "-----" << std::endl;
	while(loc!=end) {
		for(int i=0; i < userLogTree.depth(loc)-rootdepth; ++i) 
			std::cout << "  ";
		std::cout << "n=" << loc->getNodeId() << " m=" << loc->getMediaId() << std::endl;
		++loc;
	}
	std::cout << "-----" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
