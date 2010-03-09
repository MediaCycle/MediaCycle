/*
 *  ACMediaUserlog.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
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

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

#include "lib/tree/tree.hh"
//#include "lib/tree/tree.h"
//using namespace core;
using namespace std;

class ACUserNode {
	
public:
	ACUserNode(long int _nodeId, long int _mediaId, int _clickTime);
	~ACUserNode();
	bool operator == (const ACUserNode &other) const;
	void clickNode(long int clickTime);
	long int getNodeId(){return this->nodeId;}
	long int getMediaId(){return this->mediaId;}
	
private:
	long int nodeId;
	long int mediaId;
	bool isVisible;
	vector<int> clickTime;
};

class ACUserLog {

public:
	ACUserLog();
	~ACUserLog();
			
	void addRootNode(long int mediaId, int clickTime);
	long int addNode(long int parentId, long int mediaId, int clickTime);
	long int getMediaIdFromNodeId(long int nodeId);
	void clickNode(long int nodeId, long int clickTime);
	int getSize() {return userLogTree.size();};
	int getMaxDepth() {return userLogTree.max_depth();};
	bool isEmpty() {return userLogTree.empty();};
	int getLastClickedNodeId(){return mLastClickedNodeId;};
	//ACUserNode* getNode(long int nodeId);
	
private:	
	int mNodeId;
	tree<ACUserNode> userLogTree;
	//core::tree<ACUserNode> userLogTree;
	int mLastClickedNodeId;
};
