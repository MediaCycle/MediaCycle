/*
 *  UserTrackInfo.h
 *  hellohal
 *
 *  @author Julien Leroy
 *  @date 21/03/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#ifndef _USERINFO
#define _USERINFO

#include <iostream>
#include <vector>
#include <string.h>
#include <XnCppWrapper.h>

using namespace std;
using namespace xn;



class userInfo {
	
public:
	
	userInfo();
	userInfo(int ,int); 
	~userInfo();
	void addToTrajectory(int x,int y,int z);
	void getTrajectory(vector<int> &trajectory_X,vector<int> &trajectory_Y,vector<int> &trajectory_Z);
	void getDelta(int &dx,int &dy,int &dz);
	void addToHandRTrajectory(int x,int y,int z);
	void getHandRTrajectory(vector<int> &trajectory_X,vector<int> &trajectory_Y,vector<int> &trajectory_Z);
	void getHandRDelta(int &dx,int &dy,int &dz);
	void addToHandLTrajectory(int x,int y,int z);
	void getHandLTrajectory(vector<int> &trajectory_X,vector<int> &trajectory_Y,vector<int> &trajectory_Z);
	void getHandLDelta(int &dx,int &dy,int &dz);
	void getId(int &idG,int &nid);
	
	
	
private:
	
	string m_name;
	int m_id;
	int m_id_g;
	/*
	vector<int> m_trajectory_X;
	vector<int> m_trajectory_Y;
	vector<int> m_trajectory_Z;
	vector<int> m_handRtrajectory_X;
	vector<int> m_handRtrajectory_Y;
	vector<int> m_handRtrajectory_Z;
	vector<int> m_handLtrajectory_X;
	vector<int> m_handLtrajectory_Y;
	vector<int> m_handLtrajectory_Z;
	*/
	int m_dx;
	int m_dy;
	int m_dz;
	int m_handRdx;
	int m_handRdy;
	int m_handRdz;
	int m_handLdx;
	int m_handLdy;
	int m_handLdz;
	
	int dir;
	
	int size;
	
};


#endif