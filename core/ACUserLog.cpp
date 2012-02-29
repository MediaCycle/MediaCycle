/*
 *  ACMediaUserLog.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont, Damien Tardieu
 *  @date 25/02/10
 *  Filled by Christian Frisson on 12/03/2010
 *  XS 150310 removed ACUserNode, replaced by ACMediaNode (see separate file)
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

#include <sys/time.h>

using namespace std;

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};

    gettimeofday(&tv, &tz);

    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

ACUserLog::ACUserLog() {
    mNodeId = 0;
	mLastClickedNodeId = -1;
}

ACUserLog::~ACUserLog() {
}

void ACUserLog::addRootNode(long int _mediaId, int _clickTime) {
	ACMediaNode *userNode;
	userNode = new ACMediaNode(0, _mediaId, _clickTime);
	userNode->setDisplayed(true);
	userLogTree.set_head(*userNode);
	mNodeId++;
    //delete userNode;
}


long int ACUserLog::addNode(long int _parentId, long int _mediaId, int _clickTime) {
    tree<ACMediaNode>::iterator location;
	ACMediaNode *userNode;
	userNode = new ACMediaNode(mNodeId, _mediaId, _clickTime);
	userNode->setDisplayed(true);
	// temporary node for comparison on nodeID using "==" method 
	ACMediaNode *tmpNode;
	tmpNode = new ACMediaNode(_parentId, 0, 0);
	location = find(userLogTree.begin(), userLogTree.end(), *tmpNode);
    if(location != userLogTree.end()){
        userLogTree.append_child(location, *userNode); //CF: instead of location.insert(*userNode);
        this->getNodeFromId(mNodeId).setDisplayed(true);
        mNodeId++;
    }
    else
        std::cerr << "ACUserLog::addNode: parent " << _parentId << " of media " << _mediaId << " not found in the user log." << std::endl;
    //delete userNode;
	delete tmpNode;
	return mNodeId;
}

void ACUserLog::clickNode(long int _nodeId, long int _clickTime) {
    tree<ACMediaNode>::iterator location;
	ACMediaNode *tmpNode;
	tmpNode = new ACMediaNode(_nodeId, 0, 0);
    location = find(userLogTree.begin(), userLogTree.end(), *tmpNode);
    if (location!=userLogTree.end()){
        (*location).clickNode(_clickTime);
        mLastClickedNodeId = _nodeId;
    }
    else
        std::cerr << "ACUserLog::clickNode: node " << _nodeId << " not found in the user log." << std::endl;
    delete tmpNode;
}

int ACUserLog::getSize()
{
    return this->userLogTree.size();
}

int ACUserLog::getDepthAtNode(long int _nodeId)
{
    if ((_nodeId >=0) && (_nodeId <= mNodeId)){
        tree<ACMediaNode>::iterator node = userLogTree.begin();
        while( (node!=userLogTree.end()) ) { // (_nodeId != (*node).getNodeId()) &&
            if ((*node).getNodeId() == _nodeId) {
                return this->userLogTree.depth(node);
            }
            ++node;
        }
    }
    std::cerr << "ACUserLog::getDepthAtNode: node " << _nodeId << " not found " << std::endl;
    return -1;
}

int ACUserLog::getMaxDepth()
{
    return this->userLogTree.max_depth();
}

bool ACUserLog::isEmpty()
{
    //return userLogTree.empty();
    bool empty = true;
    if (this->userLogTree.size() != 0){
        empty = false;
    }
    return empty;
}

int ACUserLog::getLastClickedNodeId()
{
    return mLastClickedNodeId;
}

void ACUserLog::setLastClickedNodeId(long int id)
{
    mLastClickedNodeId=id;
}

// XS TODO return value
ACMediaNode& ACUserLog::getNodeFromId(long int _nodeId) {
	if ((_nodeId >=0) && (_nodeId <= mNodeId)){
		tree<ACMediaNode>::iterator node = userLogTree.begin();		
		while( (node!=userLogTree.end()) ) { // (_nodeId != (*node).getNodeId()) &&
			if ((*node).getNodeId() == _nodeId) {
				return (*node);
			}
			++node;
		}	
	}
	//CF return what?
    std::cerr << "ACUserLog::getNodeFromId " << _nodeId << " not found " << std::endl;
}

ACMediaNode& ACUserLog::getNodeFromMediaId(long int _mediaId) {
    if (_mediaId >=0){
        tree<ACMediaNode>::iterator node = userLogTree.begin();
        while( (node!=userLogTree.end()) ) { // (_nodeId != (*node).getNodeId()) &&
            if ((*node).getMediaId() == _mediaId) {
                return (*node);
            }
            ++node;
        }
    }
    //CF return what?
    std::cerr << "ACUserLog::getNodeFromMediaId " << _mediaId << " not found " << std::endl;
}

long int ACUserLog::getMediaIdFromNodeId(long int _nodeId) {
	long int _mediaId = -1;
	if ((_nodeId >=0) && (_nodeId <= mNodeId)){
		tree<ACMediaNode>::iterator node = userLogTree.begin();		
		while( (node!=userLogTree.end()) ) { //(_nodeId != node->getNodeId()) && 
			//std::cout << "Node Id loop" << _nodeId << " node " << node->getNodeId() << std::endl;
			if ((*node).getNodeId() == _nodeId) {
				_mediaId = (*node).getMediaId();
				break;
			}	
			++node;
		}
	}
    if (_mediaId == -1)
        std::cerr << "ACUserLog::getMediaIdFromNodeId: no corresponding media for node " << _nodeId << " found in the user log." << std::endl;
	return _mediaId;
}

int ACUserLog::getSpanAtDepth(int _depth){
	int _span = -1;
	if ((_depth >=0) && (_depth <= getMaxDepth())){
		_span = 0;
		tree<ACMediaNode>::iterator node = userLogTree.begin();		
		while( (node!=userLogTree.end()) ) {
			if (userLogTree.depth(node) == _depth) {
				_span++;
			}	
			++node;
		}		
	}
	return _span;
}

int ACUserLog::getChildCountAtNodeId(long int _nodeId)
{
	int _childCount = -1;
	tree<ACMediaNode>::iterator node = userLogTree.begin();		
	while( (node!=userLogTree.end()) ) {
		if ((*node).getNodeId() != _nodeId) {
			++node;
		}	
		else {
			_childCount = userLogTree.number_of_children(node);
			break;
		}	
	}
	return 	_childCount;	
}

int ACUserLog::getPreviousSiblingFromNodeId(long int _nodeId)
{
	int _prevNodeId = -1;
	tree<ACMediaNode>::iterator node = userLogTree.begin();		
	while( (node!=userLogTree.end()) ) {
		if ((*node).getNodeId() != _nodeId) {
			++node;
		}	
		else {
			tree<ACMediaNode>::sibling_iterator sib;
			sib = userLogTree.previous_sibling(node);
			if (userLogTree.is_valid(sib)) {
				_prevNodeId = (*sib).getNodeId();
			}	
			break;
		}	
	}
	return 	_prevNodeId;	
}

int ACUserLog::getNextSiblingFromNodeId(long int _nodeId)
{
	int _nextNodeId = -1;
	tree<ACMediaNode>::iterator node = userLogTree.begin();		
	while( (node!=userLogTree.end()) ) {
		if ((*node).getNodeId() != _nodeId) {
			++node;
		}	
		else {
			tree<ACMediaNode>::sibling_iterator sib;
			sib = userLogTree.next_sibling(node);
			if (userLogTree.is_valid(sib)){
				_nextNodeId = (*sib).getNodeId();
			}	
			break;
		}	
	}
	return 	_nextNodeId;	
}

int ACUserLog::getFirstChildFromNodeId(long int _nodeId)
{
	int _firstChildId = -1;
	tree<ACMediaNode>::iterator node = userLogTree.begin();		
	while( (node!=userLogTree.end()) ) {
		if ((*node).getNodeId() != _nodeId) {
			++node;
		}	
		else {
			if (getChildCountAtNodeId(_nodeId) > 0) {
				tree<ACMediaNode>::sibling_iterator child = userLogTree.begin(node);
				if (userLogTree.is_valid(child)) {
					_firstChildId = (*child).getNodeId();
				}	
			}
			break;
		}	
	}
	return 	_firstChildId;	
}

int ACUserLog::getLastChildFromNodeId(long int _nodeId)
{
	int _lastChildId = -1;
	tree<ACMediaNode>::iterator node = userLogTree.begin();		
	while( (node!=userLogTree.end()) ) {
		if ((*node).getNodeId() != _nodeId) {
			++node;
		}	
		else {
			if (getChildCountAtNodeId(_nodeId) > 0) {
				tree<ACMediaNode>::sibling_iterator child = userLogTree.end(node);
				--child;
				if (userLogTree.is_valid(child)) {
					_lastChildId = (*child).getNodeId();
				}	
			}
			break;
		}	
	}
	return 	_lastChildId;	
}


int ACUserLog::getParentFromNodeId(long int _nodeId)
{
	int _parentId = -1;
	tree<ACMediaNode>::iterator node = userLogTree.begin();		
	while( (node!=userLogTree.end()) ) {
		if ((*node).getNodeId() != _nodeId) {
			++node;
		}	
		else {
			if (userLogTree.is_valid(userLogTree.parent(node)) )
				_parentId = (*(userLogTree.parent(node))).getNodeId();
			break;
		}	
	}
	return 	_parentId;	
}


void ACUserLog::dump(){
	tree<ACMediaNode>::iterator loc  = userLogTree.begin();
	tree<ACMediaNode>::iterator end  = userLogTree.end();
	
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

void ACUserLog::wrapToOrigin()
{
    tree<ACMediaNode>::iterator loc  = userLogTree.begin();
    tree<ACMediaNode>::iterator end  = userLogTree.end();

    int rootdepth=userLogTree.depth(loc);

    ACPoint p;
    p.x = 0;
    p.y = 0;
    p.z = 0;

    double t = getTime();

    while(loc!=end) {
        for(int i=0; i < userLogTree.depth(loc)-rootdepth; ++i)
            std::cout << "  ";
        std::cout << "n=" << loc->getNodeId() << " m=" << loc->getMediaId() << std::endl;
        loc->setNextPosition(p, t);
        ++loc;
    }
}

tree<ACMediaNode> ACUserLog::getTree()
{
    return userLogTree;
}

int ACUserLog::getNthChildAtNodeId(long int _nodeId,long int _nthChild)
{
	int _childId = -1;
	int _childCount = getChildCountAtNodeId(_nodeId);
	if ( _childCount>0 && _nthChild < _childCount )
	{
		_childId = getFirstChildFromNodeId(_nodeId);
		int _nth = 0;
		while ( _childId !=1 && _childId != getLastChildFromNodeId(_nodeId) && _nth < _nthChild) {
			_childId = getNextSiblingFromNodeId(_childId);
			_nth++;
		}	
		
	}	
	return _childId;
}

void ACUserLog::clean()
{
    this->mNodeId = 0;
	this->mLastClickedNodeId = -1;
    if (this->userLogTree.size()>0)
        this->userLogTree.clear(); //CF erases all nodes, what about the root?
	//std::cout << "Is the tree empty? " << userLogTree.empty() << std::endl;
}	
