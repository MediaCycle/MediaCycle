/*
 *  ACNeighborsManager.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont, Damien Tardieu
 *  @date 25/02/10
 *  Filled by Christian Frisson since 12/03/2010
 *  UnGPL'd by Alexis Moinet on 26/06/2015
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
    mTree.root(_mediaId);
    mNodeId = _mediaId;
}


bool ACNeighborsManager::addNode(long int _parentId, long int _mediaId, int _clickTime) {
    if(_parentId == -1){
        std::cerr << "ACNeighborsManager::addNode: there is already a root node " << std::endl;
        return false;
    }

    if (this->mTree.append(_parentId,_mediaId)) {
        return true;
    } else {
        std::cerr << "ACNeighborsManager::addNode: error adding media " << _mediaId << " as child of " << _parentId << "not found in the user log." << std::endl;
        return false;
    }
}

bool ACNeighborsManager::removeNode(long int _id) {
    if(this->mTree.remove(_id)){
        return true;
    } else {
        std::cerr << "ACNeighborsManager::removeNode: node " << _id << " not found in the user log." << std::endl;
        return false;
    }
}

bool ACNeighborsManager::removeChildrenNodes(long int _id) {
    if(this->mTree.removeChildren(_id)){
        return true;
    } else {
        std::cerr << "ACNeighborsManager::removeChidlrenNodes: node " << _id << " not found in the user log." << std::endl;
        return false;
    }
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
    long int depth = this->mTree.depth(_nodeId);
    if (depth >= 0)
        return depth;
    else{
        std::cerr << "ACNeighborsManager::getDepthAtNode: node " << _nodeId << " not found " << std::endl;
        return -1;
    }
}

int ACNeighborsManager::getMaxDepth()
{
    return (int) this->mTree.maxDepth();
}

bool ACNeighborsManager::isEmpty()
{
    return this->mTree.empty();
}

int ACNeighborsManager::getSpanAtDepth(int _depth){
    int _span = -1;
    if ((_depth >=0) && (_depth <= getMaxDepth())){
        _span = 0;
        mc::tree_iterator node = this->mTree.begin();
        while( (node!=mTree.end()) ) {
            if (node->second->depth() == _depth) {
                _span++;
            }
            ++node;
        }
    }
    return _span;
}

int ACNeighborsManager::getChildCountAtNodeId(long int _nodeId)
{
    mc::Node * node = this->mTree.find(_nodeId);

    if (node) // nb: returns 0 for a leaf
        return node->size();
    else // not found
        return -1;
}

long int ACNeighborsManager::getPreviousSiblingFromNodeId(long int _nodeId)
{
    mc::Node * node = this->mTree.prev(_nodeId);

    if (node)
        return node->value();
    else
        return -1;
}

long int ACNeighborsManager::getNextSiblingFromNodeId(long int _nodeId)
{
    mc::Node * node = this->mTree.next(_nodeId);

    if (node)
        return node->value();
    else
        return -1;
}

long int ACNeighborsManager::getFirstChildFromNodeId(long int _nodeId)
{
    mc::Node * node = this->mTree.first(_nodeId);

    if (node)
        return node->value();
    else
        return -1;
}

long int ACNeighborsManager::getLastChildFromNodeId(long int _nodeId)
{
    mc::Node * node = this->mTree.last(_nodeId);

    if (node)
        return node->value();
    else
        return -1;
}

long int ACNeighborsManager::getParentFromNodeId(long int _nodeId)
{
    mc::Node * node = this->mTree.find(_nodeId);

    if (node && node->parent())
        return node->parent()->value();
    else
        return -1;
}

void ACNeighborsManager::dump(){
/*    mc::tree_iterator loc = this->mTree.begin();
    mc::tree_iterator end = this->mTree.end();
    int rootdepth = loc->second->depth(); // == mTree->root()->depth()
    std::cout << "-----" << std::endl;
    while(loc!=end) {
        for(int i=0; i < loc->second->depth()-rootdepth; ++i)
            std::cout << "  ";
        std::cout << "m=" << loc->second->value() << std::endl;
        ++loc;
    }
    std::cout << "-----" << std::endl;*/
    
    std::cout << "-----" << std::endl;

    this->mTree.print();

    std::cout << "-----" << std::endl;
}

std::list<long int> ACNeighborsManager::getNodeIds(){
    std::list<long int> nodeIds;
    for(mc::tree_iterator e = mTree.begin(); e!=mTree.end();e++)
        nodeIds.push_back( (e->first) );
    //std::cout << " m=" << (*e) << std::endl;
    return nodeIds;
}

std::list<long int> ACNeighborsManager::getNodeIds(long int _nodeId) {
    std::list<long int> nodeIds;

    mc::Node * node = this->mTree.find(_nodeId);

    if (node) {
        nodeIds.push_back(node->value());

        //cf. Node::print(Node)
        mc::Node * current = node->first();

        while(current) {
            nodeIds.push_back(current->value());

            // next
            if (!current->leaf()) {
                current = current->children().front();
            } else {
                if (current->next()) {
                    current = current->next();
                } else {
                    while (current != node && !current->next()) {
                        current = current->parent();
                    }

                    if (current) {
                        if (current == node)
                            break;
                        current = current->next();
                    }
                }
            }
        }
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
