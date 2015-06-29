/*
 *  ACNeighborsManager.h
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

#ifndef ACNEIGHBORSMANAGER_H
#define ACNEIGHBORSMANAGER_H

#include <algorithm>
#include <string>
#include <iostream>
#include <list>

#include "Tree.h"

class ACNeighborsManager {

public:
    ACNeighborsManager();
    ~ACNeighborsManager();

    void setReferenceNode(long int _mediaId, int _clickTime=0);
    bool addNode(long int _parentId, long int _mediaId, int _clickTime=0);
    bool removeNode(long int _id);
    bool removeChildrenNodes(long int _id);
    void setClickedNode(long int _mediaId);
    long int getPreviousSiblingFromNodeId(long int _nodeId);
    long int getNextSiblingFromNodeId(long int _nodeId);
    long int getFirstChildFromNodeId(long int _nodeId);
    long int getLastChildFromNodeId(long int _nodeId);
    int getChildCountAtNodeId(long int _nodeId);
    long int getParentFromNodeId(long int _nodeId);
    void dump();
    void clean();
    std::list<long int> getNodeIds();
    std::list<long int> getNodeIds(long int _nodeId);
    int getSize();

//protected: //CF these methods are not yet bound by ACMediaBrowser
    int getDepthAtNode(long int _nodeId);
    int getMaxDepth();
    bool isEmpty();
    int getSpanAtDepth(int _depth);
    long int getNthChildAtNodeId(long int _nodeId,long int _nthChild);

private:	
    int mNodeId;
    mc::Tree mTree;
    int mLastClickedNodeId;
};

#endif // ACNEIGHBORSMANAGER_H
