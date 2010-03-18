/*
 *  ACPositionProximityGridPlugin.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 16/03/10
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

#include "ACPlugin.h"
#include "ACMediaBrowser.h"

#ifndef _ACPOSITIONSPROXIMITYGRID_
#define _ACPOSITIONSPROXIMITYGRID_

class ACPositionProximityGridPlugin : public ACPlugin {
public:
	ACPositionProximityGridPlugin();
	~ACPositionProximityGridPlugin();
	void updateClusters(ACMediaBrowser* ){};//not implemented
	void updateNextPositions(ACMediaBrowser* );
	void updateNeighborhoods(ACMediaBrowser* ){};//not implemented
protected:
	ACMediaBrowser* mediaBrowser;
private:
	float proxgridstepx;
	float proxgridstepy;
	float proxgridaspectratio;
	int proxgridlx;
	int proxgridly;
	float proxgridl;
	float proxgridr;
	float proxgridb;
	float proxgridt;
	int proxgridmaxdistance;
	float proxgridjitter;
	int proxgridboundsset;
	vector<int> proxgrid;
	
	void setProximityGrid();
	void setProximityGridQuantize(ACPoint p, ACPoint *pgrid);	
	void setProximityGridUnquantize(ACPoint pgrid, ACPoint *p);
	void setProximityGridBounds(float l, float r, float b, float t);
	void setRepulsionEngine();
	
	
};

#endif // _ACPOSITIONSPROXIMITYGRID_
