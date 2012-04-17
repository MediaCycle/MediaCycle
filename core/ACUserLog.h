/*
 *  ACMediaUserLog.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 25/02/10
 *  Filled by Christian Frisson on 12/03/2010
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

#ifndef ACUSERLOG_H
#define ACUSERLOG_H

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

#include <tree.hh>
#include "ACMediaNode.h"

using namespace std;

class ACUserLog {

public:
    ACUserLog();
    ~ACUserLog();

    void addRootNode(long int _mediaId, int _clickTime);
    long int addNode(long int _parentId, long int _mediaId, int _clickTime);
    ACMediaNode& getNodeFromId(long int _nodeId);
    ACMediaNode& getNodeFromMediaId(long int _mediaId);
    long int getMediaIdFromNodeId(long int _nodeId);
    void clickNode(long int _nodeId, long int _clickTime);
    int getSize();
    int getDepthAtNode(long int _nodeId);
    int getMaxDepth();
    bool isEmpty();
    int getLastClickedNodeId();
    void setLastClickedNodeId(long int id);
    int getSpanAtDepth(int _depth);
    int getChildCountAtNodeId(long int _nodeId);
    int getPreviousSiblingFromNodeId(long int _nodeId);
    int getNextSiblingFromNodeId(long int _nodeId);
    int getFirstChildFromNodeId(long int _nodeId);
    int getLastChildFromNodeId(long int _nodeId);
    int getParentFromNodeId(long int _nodeId);
    Tree<ACMediaNode> getTree();
    int getNthChildAtNodeId(long int _nodeId,long int _nthChild);
    void wrapToOrigin();
    void dump();
    void clean();

private:	
    int mNodeId;
    Tree<ACMediaNode> userLogTree;
    int mLastClickedNodeId;
};

#endif // ACUSERLOG_H
