/*
 *  tree_test.cpp
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


#include <algorithm>
#include <string>
#include <iostream>
#include "ACUserLog.h"

using namespace std;

int main(int, char **)
{
	ACUserLog *pipo = new ACUserLog();
	pipo->addRootNode(0,0);
	// clicked on node 0 at time 0
	pipo->clickNode(0,0);
	
	// add his neighbors
	pipo->addNode(0, 17, 0); // 1
	pipo->addNode(0, 3, 0);  // 2
	pipo->addNode(0, 14, 0); // 3
	pipo->addNode(0, 12, 0); // 4
	pipo->addNode(0, 8, 0);  // 5
	
	// clicked on node 3 at time 1
	pipo->clickNode(3,1);  
	
	// add his neighbors
	pipo->addNode(3, 13, 0);  // 6 == 0
	pipo->addNode(3, 4, 0);  // 7
	pipo->addNode(3, 11, 0); // 8
	pipo->addNode(3, 10, 0); // 9
	pipo->addNode(3, 9, 0) ; // 10	
	
	pipo->dump();
	
	delete pipo;
}
