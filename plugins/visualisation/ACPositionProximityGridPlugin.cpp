/*
 *  ACPositionProximityGridPlugin.cpp
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

#include "ACPositionProximityGridPlugin.h"

ACPositionProximityGridPlugin::ACPositionProximityGridPlugin() {
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
   // this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "Proximity Grid";
    this->mDescription = "Plugin for Positioning items on a grid";
    this->mId = "";
	
	proxgridboundsset = 0;
}

ACPositionProximityGridPlugin::~ACPositionProximityGridPlugin() {
}

void ACPositionProximityGridPlugin::updateNextPositions(ACMediaBrowser* _mediaBrowser) {
	std::cout << "ACPositionProximityGridPlugin::updateNextPositions" << std::endl;
	
	if (mediaBrowser == 0){
		mediaBrowser = _mediaBrowser;
	}
	
	if (!(mediaBrowser->getUserLog()->isEmpty()))
	{
		
		int librarySize = mediaBrowser->getLibrary()->getSize();
		
		if(librarySize==0) {
			mediaBrowser->setNeedsDisplay(true);
			return;
		}
				
		mediaBrowser->setNeedsDisplay(true);
	}
}	

void ACPositionProximityGridPlugin::setProximityGridQuantize(ACPoint p, ACPoint *pgrid) {
	pgrid->x = (int)round( (p.x-proxgridl)/proxgridstepx );
	pgrid->y = (int)round( (p.y-proxgridb)/proxgridstepy );		
}

void ACPositionProximityGridPlugin::setProximityGridUnquantize(ACPoint pgrid, ACPoint *p) {
	p->x = pgrid.x * proxgridstepx + proxgridl;
	p->y = pgrid.y * proxgridstepy + proxgridb;		
}

void ACPositionProximityGridPlugin::setProximityGridBounds(float l, float r, float b, float t) {
	proxgridl = l;
	proxgridr = r;
	proxgridb = b;
	proxgridt = t;
	proxgridboundsset = 1;
}

void ACPositionProximityGridPlugin::setProximityGrid() {
	
	float jitter;
	
	int i, j, k, l;
	int n;
	int found_slot;
	
	ACPoint p, pgrid, p2, curpos;
	int index, pgridindex, curposindex;
	
	float langle, orientation, spiralstepx, spiralstepy, lorientation;
	
	n = mediaBrowser->getNumberOfMediaNodes();
	
	// Proximity Grid Size	
	if (!proxgridboundsset) {
		if (n>0) {
			p = mediaBrowser->getMediaNode(0).getNextPosition();
			proxgridl = p.x;
			proxgridr = p.x;
			proxgridb = p.y;
			proxgridt = p.y;
		}
		for(i=1; i<n; i++) {
			p = mediaBrowser->getMediaNode(i).getNextPosition();
			if (p.x<proxgridl) {
				proxgridl = p.x;
			}
			if (p.x>proxgridr) {
				proxgridr = p.x;
			}
			if (p.y<proxgridb) {
				proxgridb = p.y;
			}
			if (p.y>proxgridt) {
				proxgridt = p.y;
			}
		}
	}
	
	// Proximity Grid Density
	proxgridlx = 2;
	proxgridstepx = (proxgridr-proxgridl)/(proxgridlx-1);
	proxgridaspectratio = 9.0/16.0;
	proxgridstepy = proxgridstepx * proxgridaspectratio;
	proxgridly = (proxgridt-proxgridb)/proxgridstepy + 1;	
	proxgridmaxdistance = 2;
	proxgridjitter = 0.25;
	
	// Init
	proxgrid.resize((proxgridlx)*(proxgridly));
	fill(proxgrid.begin(), proxgrid.end(), -1);
	
	// SD TODO - maybe need to compute MST (Minimum Spanning Tree)
	
	for(i=0; i<n; i++) {
		
		found_slot = 0;
		
		p = mediaBrowser->getMediaNode(i).getNextPosition();
		
		// grid quantization
		this->setProximityGridQuantize(p, &pgrid);
		pgridindex =  pgrid.y * proxgridlx + pgrid.x;
		
		// no further processing for this media if out of grid
		if ( (pgrid.x<0) || (pgrid.x>=proxgridlx) || (pgrid.y<0) || (pgrid.y>=proxgridly) ) {
			continue;
		}
		
		// spiral search
	    this->setProximityGridUnquantize(pgrid, &p2);
		langle = atan( (p.y-p2.y) / (p.x-p2.x) );
		orientation = fmod(ceil(langle/(M_PI/4.0)), 2) * 2 - 1; // +1 means positive angles
		spiralstepx = fmod(round((langle-M_PI/2.0)/(M_PI/2.0)), 2);
		if ((p.x-p2.x)<0) spiralstepx = - spiralstepx;
		spiralstepy = fmod(round(langle/(M_PI/2.0)), 2);
		if ((p.y-p2.y)<0) spiralstepy = - spiralstepy;
		lorientation = atan(spiralstepy/spiralstepx);
		
		curpos.x = pgrid.x;
		curpos.y = pgrid.y;
		curposindex = curpos.y * proxgridlx + curpos.x;
		if (proxgrid[curposindex]==-1) {
			l = proxgridmaxdistance;
			found_slot = 1;
		}
		else {
			l = 1;
		}
		
		while (l<proxgridmaxdistance) {
			for (j=0;j<2;j++) {	
				for (k=0;k<l;k++) {
					curpos.x += spiralstepx;
					curpos.y += spiralstepy;
					curposindex = curpos.y * proxgridlx + curpos.x;
					if ( (curpos.x<0) || (curpos.x>=proxgridlx) || (curpos.y<0) || (curpos.y>=proxgridly) ) {
						continue;
					}
					else {
						if (proxgrid[curposindex]==-1) {
							l=proxgridmaxdistance;
							found_slot = 1;
							k=l;
							j=2;
						}
					}
				}
				// minus PI or plus PI depending on orientation
				lorientation += (orientation * M_PI / 2.0);
				lorientation = fmod((double)lorientation, 2.0*M_PI);
				spiralstepx = cos(lorientation);
				spiralstepy = sin(lorientation);
			}	
			l++;
		}
		
		// empty stategy
		if (found_slot) {
			proxgrid[curposindex] = i;
		}
		
		// swap strategy
		// SD TODO
		
		// bump strategy
		// SD TODO
		
	}
	
	// XS TODO iter
	for(i=0; i<n; i++) {
		mediaBrowser->getMediaNode(i).setNextPositionGrid (mediaBrowser->getMediaNode(i).getNextPosition());
	}
	
	for(i=0; i<proxgrid.size(); i++) {
		index = proxgrid[i];
		if ( (index>=0) && (index<n) ) {
			curpos.x = fmod((float)i,proxgridlx);
			curpos.y = floor((float)i/(proxgridlx));
			this->setProximityGridUnquantize(curpos, &p2);
			p2.z = mediaBrowser->getMediaNode(index).getNextPosition().z;
			mediaBrowser->getMediaNode(index).setNextPositionGrid(p2);
		}
	}
	
	// XS TODO iter
	double t = getTime();
	for(i=0; i<n; i++) {
		mediaBrowser->getMediaNode(i).setNextPosition(mediaBrowser->getMediaNode(i).getNextPositionGrid(), t);
	}
	
	if (proxgridjitter>0) {
		for(i=0; i<n; i++) {
			// XS heavy ?
			p = mediaBrowser->getMediaNode(i).getNextPosition();
			jitter = ACRandom()-0.5;
			p.x = p.x + jitter*proxgridjitter*proxgridstepx;
			jitter = ACRandom()-0.5;
			p.y = p.y + jitter*proxgridjitter*proxgridstepy;
			p.x = max(min(p.x,proxgridr), proxgridl);
			p.y = max(min(p.y,proxgridt), proxgridb);
			p.z = p.z;
			mediaBrowser->getMediaNode(i).setNextPosition(p, t);
		}
	}	
	return;
}

void ACPositionProximityGridPlugin::setRepulsionEngine() {
	return;
}
