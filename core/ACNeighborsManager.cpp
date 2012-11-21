/*
 *  ACNeighborsManager.cpp
 *  MediaCycle
 *
 *  @author Stphane Dupont, Damien Tardieu
 *  @date 25/02/10
 *  Filled by Christian Frisson since 12/03/2010
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

#include "ACNeighborsManager.h"

using namespace std;

ACNeighborsManager::ACNeighborsManager() {
    mNodeId = -1;
    mLastClickedNodeId = -1;
}

ACNeighborsManager::~ACNeighborsManager() {
}

void ACNeighborsManager::setReferenceNode(long int _mediaId, int _clickTime) {
    if(mTree.size() != 0){
        this->clean();
    }
    mTree.set_head(_mediaId);
    mNodeId = _mediaId;
}


bool ACNeighborsManager::addNode(long int _parentId, long int _mediaId, int _clickTime) {
    if(_parentId == -1){
        std::cerr << "ACNeighborsManager::addNode: there is already a root node " << std::endl;
        return false;
    }

    Tree<long int>::iterator media_location;
    media_location = find(mTree.begin(), mTree.end(), _mediaId);
    if(media_location != mTree.end()){
        std::cerr << "ACNeighborsManager::addNode: media " << _mediaId << " already in the user log." << std::endl;
        return false;
    }

    Tree<long int>::iterator parent_location;
    parent_location = find(mTree.begin(), mTree.end(), _parentId);
    if(parent_location != mTree.end()){
        mTree.append_child(parent_location, _mediaId);
        mNodeId++;
        return true;
    }
    else{
        std::cerr << "ACNeighborsManager::addNode: parent " << _parentId << " of media " << _mediaId << " not found in the user log." << std::endl;
        return false;
    }
}

bool ACNeighborsManager::removeNode(long int _id) {
    Tree<long int>::iterator location;
    location = find(mTree.begin(), mTree.end(), _id);
    if(location != mTree.end()){
        //CF we keep the media nodes in the browser
        mTree.erase_children(location);
        mTree.erase(location);
        return true;
    }
    else
        std::cerr << "ACNeighborsManager::removeNode: node " << _id << " not found in the user log." << std::endl;
    return false;
}

bool ACNeighborsManager::removeChildrenNodes(long int _id) {
    Tree<long int>::iterator location;
    // temporary node for comparison on nodeID using "==" method
    location = find(mTree.begin(), mTree.end(), _id);
    if(location != mTree.end()){
        //CF we keep the media nodes in the browser
        mTree.erase_children(location);
        return true;
    }
    else
        std::cerr << "ACNeighborsManager::removeChidlrenNodes: node " << _id << " not found in the user log." << std::endl;
    return false;
}

void ACNeighborsManager::setClickedNode(long int _mediaId) {
    mLastClickedNodeId = _mediaId;
}

int ACNeighborsManager::getSize()
{
    return this->mTree.size();
}

int ACNeighborsManager::getDepthAtNode(long int _nodeId)
{
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end())
        return this->mTree.depth(node);
    else{
        std::cerr << "ACNeighborsManager::getDepthAtNode: node " << _nodeId << " not found " << std::endl;
        return -1;
    }
}

int ACNeighborsManager::getMaxDepth()
{
    return this->mTree.max_depth();
}

bool ACNeighborsManager::isEmpty()
{
    bool empty = true;
    if (this->mTree.size() != 0){
        empty = false;
    }
    return empty;
}

int ACNeighborsManager::getSpanAtDepth(int _depth){
    int _span = -1;
    if ((_depth >=0) && (_depth <= getMaxDepth())){
        _span = 0;
        Tree<long int>::iterator node = mTree.begin();
        while( (node!=mTree.end()) ) {
            if (mTree.depth(node) == _depth) {
                _span++;
            }
            ++node;
        }
    }
    return _span;
}

int ACNeighborsManager::getChildCountAtNodeId(long int _nodeId)
{
    int _childCount = -1;
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end())
        _childCount = mTree.number_of_children(node);
    return _childCount;
}

long int ACNeighborsManager::getPreviousSiblingFromNodeId(long int _nodeId)
{
    long int _prevNodeId = -1;
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end()){
        Tree<long int>::sibling_iterator sib;
        sib = mTree.previous_sibling(node);
        if (mTree.is_valid(sib)) {
            _prevNodeId = (*sib);
        }
    }
    return _prevNodeId;
}

long int ACNeighborsManager::getNextSiblingFromNodeId(long int _nodeId)
{
    long int _nextNodeId = -1;
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end()){
        Tree<long int>::sibling_iterator sib;
        sib = mTree.next_sibling(node);
        if (mTree.is_valid(sib)){
            _nextNodeId = (*sib);
        }
    }
    return _nextNodeId;
}

long int ACNeighborsManager::getFirstChildFromNodeId(long int _nodeId)
{
    long int _firstChildId = -1;
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end()){
        if (getChildCountAtNodeId(_nodeId) > 0) {
            Tree<long int>::sibling_iterator child = mTree.begin(node);
            if (mTree.is_valid(child)) {
                _firstChildId = (*child);
            }
        }
    }
    return _firstChildId;
}

long int ACNeighborsManager::getLastChildFromNodeId(long int _nodeId)
{
    long int _lastChildId = -1;
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end()){
        if (getChildCountAtNodeId(_nodeId) > 0) {
            Tree<long int>::sibling_iterator child = mTree.end(node);
            --child;
            if (mTree.is_valid(child)) {
                _lastChildId = (*child);
            }
        }
    }
    return _lastChildId;
}


long int ACNeighborsManager::getParentFromNodeId(long int _nodeId)
{
    long int _parentId = -1;
    Tree<long int>::iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end()){
        if (mTree.is_valid(mTree.parent(node)) )
            _parentId = (*(mTree.parent(node)));
    }
    return _parentId;
}


void ACNeighborsManager::dump(){
    Tree<long int>::iterator loc  = mTree.begin();
    Tree<long int>::iterator end  = mTree.end();
    int rootdepth=mTree.depth(loc);
    std::cout << "-----" << std::endl;
    while(loc!=end) {
        for(int i=0; i < mTree.depth(loc)-rootdepth; ++i)
            std::cout << "  ";
        std::cout << "m=" << (*loc) << std::endl;
        ++loc;
    }
    std::cout << "-----" << std::endl;
}

std::list<long int> ACNeighborsManager::getNodeIds(){
    std::list<long int> nodeIds;
    for(Tree<long int>::iterator e = mTree.begin(); e!=mTree.end();e++)
        nodeIds.push_back( (*e) );
    //std::cout << " m=" << (*e) << std::endl;
    return nodeIds;
}

std::list<long int> ACNeighborsManager::getNodeIds(long int _nodeId){
    std::list<long int> nodeIds;
    Tree<long int>::sibling_iterator node = std::find(mTree.begin(), mTree.end(), _nodeId);
    if (node!=mTree.end()){
        Tree<long int>::sibling_iterator sib;
        sib = mTree.next_sibling(node);
        Tree<long int> tempTree;
        tempTree= mTree.subTree(node,sib);
        for(Tree<long int>::iterator e = tempTree.begin(); e!=tempTree.end();e++)
            nodeIds.push_back( (*e) );
    }
    return nodeIds;
}

void ACNeighborsManager::clean()
{
    this->mNodeId = -1;
    this->mLastClickedNodeId = -1;
    if (this->mTree.size()>0)
        this->mTree.clear(); //CF erases all nodes, what about the root?
}
