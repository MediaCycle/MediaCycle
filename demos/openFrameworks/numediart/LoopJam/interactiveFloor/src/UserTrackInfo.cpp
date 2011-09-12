/*
 *  UserTrackInfo.cpp
 *  PSaMAT
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

#include "UserTrackInfo.h"
vector<int> m_trajectory_X(2,0);
vector<int> m_trajectory_Y(2,0);
vector<int> m_trajectory_Z(2,0);
vector<int> m_handRtrajectory_X(2,0);
vector<int> m_handRtrajectory_Y(2,0);
vector<int> m_handRtrajectory_Z(2,0);
vector<int> m_handLtrajectory_X(2,0);
vector<int> m_handLtrajectory_Y(2,0);
vector<int> m_handLtrajectory_Z(2,0);
userInfo::userInfo() 
{
	m_name="unknow";
	m_id=-1;
	
}
userInfo::userInfo(int idc,int idg) 
{
	m_name="unknow";
	m_id=idc;
	m_id_g=idg;
	//m_trajectory_X(2,0);
	//m_trajectory_Y(2,0);
	//m_trajectory_Z(2,0);
	//m_trajectory_X.push_back(0);
	//m_trajectory_Y.push_back(0);
	//m_trajectory_Z.push_back(0);
	//m_handRtrajectory_X(2,0);
	//m_handRtrajectory_Y(2,0);
	//m_handRtrajectory_Z(2,0);
	/*m_handRtrajectory_X.push_back(0);
	m_handRtrajectory_Y.push_back(0);
	m_handRtrajectory_Z.push_back(0);
	m_handLtrajectory_X.push_back(0);
	m_handLtrajectory_Y.push_back(0);
	m_handLtrajectory_Z.push_back(0);
	 */
}
userInfo::~userInfo() 
{
    
}
void userInfo::addToTrajectory(int x,int y,int z)
{
	//cout << x<<" "<<m_trajectory_X.back()<<endl;
	m_dx=x-m_trajectory_X.back();
	m_dy=y-m_trajectory_Y.back();
	m_dz=z-m_trajectory_Z.back();
	
	m_trajectory_X.front()=m_trajectory_X.back();
	m_trajectory_Y.front()=m_trajectory_Y.back();
	m_trajectory_Z.front()=m_trajectory_Z.back();
	
	m_trajectory_X.back()=x;
	m_trajectory_Y.back()=y;
	m_trajectory_Z.back()=z;
	
	/*
	m_trajectory_X.push_back(x);
	m_trajectory_Y.push_back(y);
	m_trajectory_Z.push_back(z);
	 */
	
}
void userInfo::addToHandRTrajectory(int x,int y,int z)
{
	//cout << x<<" "<<m_handRtrajectory_X.back()<<endl;
	/*m_handRdx=x-m_handRtrajectory_X.back();
	m_handRdy=y-m_handRtrajectory_Y.back();
	m_handRdz=z-m_handRtrajectory_Z.back();
	
	m_handRtrajectory_X.push_back(x);
	m_handRtrajectory_Y.push_back(y);
	m_handRtrajectory_Z.push_back(z);*/
	
	m_handRdx=x-m_handRtrajectory_X.back();
	m_handRdy=y-m_handRtrajectory_Y.back();
	m_handRdz=z-m_handRtrajectory_Z.back();
	
	m_handRtrajectory_X.front()=m_handRtrajectory_X.back();
	m_handRtrajectory_Y.front()=m_handRtrajectory_Y.back();
	m_handRtrajectory_Z.front()=m_handRtrajectory_Z.back();
	
	m_handRtrajectory_X.back()=x;
	m_handRtrajectory_Y.back()=y;
	m_handRtrajectory_Z.back()=z;
	
	
}
void userInfo::addToHandLTrajectory(int x,int y,int z)
{
	//cout << x<<" "<<m_handLtrajectory_X.back()<<endl;
	/*m_handLdx=x-m_handLtrajectory_X.back();
	m_handLdy=y-m_handLtrajectory_Y.back();
	m_handLdz=z-m_handLtrajectory_Z.back();
	
	m_handLtrajectory_X.push_back(x);
	m_handLtrajectory_Y.push_back(y);
	m_handLtrajectory_Z.push_back(z);*/
	
	m_handLdx=x-m_handLtrajectory_X.back();
	m_handLdy=y-m_handLtrajectory_Y.back();
	m_handLdz=z-m_handLtrajectory_Z.back();
	
	m_handLtrajectory_X.front()=m_handLtrajectory_X.back();
	m_handLtrajectory_Y.front()=m_handLtrajectory_Y.back();
	m_handLtrajectory_Z.front()=m_handLtrajectory_Z.back();
	
	m_handLtrajectory_X.back()=x;
	m_handLtrajectory_Y.back()=y;
	m_handLtrajectory_Z.back()=z;
	
	
}
void userInfo::getTrajectory(vector<int> &trajectory_X,vector<int> &trajectory_Y,vector<int> &trajectory_Z)
{
	trajectory_X=m_trajectory_X;
	trajectory_Y=m_trajectory_Y;
	trajectory_Z=m_trajectory_Z;
	
}
void userInfo::getHandRTrajectory(vector<int> &trajectory_X,vector<int> &trajectory_Y,vector<int> &trajectory_Z)
{
	trajectory_X=m_handRtrajectory_X;
	trajectory_Y=m_handRtrajectory_Y;
	trajectory_Z=m_handRtrajectory_Z;
	
}
void userInfo::getHandLTrajectory(vector<int> &trajectory_X,vector<int> &trajectory_Y,vector<int> &trajectory_Z)
{
	trajectory_X=m_handLtrajectory_X;
	trajectory_Y=m_handLtrajectory_Y;
	trajectory_Z=m_handLtrajectory_Z;
	
}
void userInfo::getDelta(int &dx,int &dy,int &dz)
{
	dx=m_dx;
	dy=m_dy;
	dz=m_dz;
}
void userInfo::getHandRDelta(int &dx,int &dy,int &dz)
{
	dx=m_handRdx;
	dy=m_handRdy;
	dz=m_handRdz;
}
void userInfo::getHandLDelta(int &dx,int &dy,int &dz)
{
	dx=m_handLdx;
	dy=m_handLdy;
	dz=m_handLdz;
}
void userInfo::getId(int &idG,int &nid)
{
	idG=m_id_g;
	nid=m_id;
}
