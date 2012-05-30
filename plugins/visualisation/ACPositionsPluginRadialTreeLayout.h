/*
 *  ACPositionsPluginRadialTreeLayout.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 04/05/2010
 *
 *  heavily based on the RadialTreeLayout from Prefuse.org
 *  by Jeffrey Heer - http://jheer.org
 *
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
 * Tree layout instance that computes a radial layout, laying out subsequent
 * depth levels of a tree on circles of progressively increasing radius.
 * 
 * The algorithm used is that of Ka-Ping Yee, Danyel Fisher, Rachna Dhamija,
 * and Marti Hearst in their research paper
 * http://citeseer.ist.psu.edu/448292.html 
 * Animated Exploration of Dynamic Graphs with Radial Layout, InfoVis 2001. 
 * This algorithm computes a radial layout which factors in possible variation
 * in sizes, and maintains both orientation and ordering constraints to 
 * facilitate smooth and understandable transitions between layout configurations.
 */

#include "ACPlugin.h"
#include "ACMediaBrowser.h"

#ifndef _ACPOSITIONSPLUGINRADIALTREELAYOUT_
#define _ACPOSITIONSPLUGINRADIALTREELAYOUT_

class ACPositionsPluginRadialTreeNodeParams {
	public:	
		ACPositionsPluginRadialTreeNodeParams();
		~ACPositionsPluginRadialTreeNodeParams(){};
	
	public:	
		double getWidth(){return width;}
		double getHeight(){return height;}
		void setWidth(double _width){this->width=_width;}
		void setHeight(double _height){this->height=_height;}
		double getX(){return x;}
		void setX(double _x){this->x=_x;}
		double getY(){return y;}
		void setY(double _y){this->y=_y;}
		double getAngle(){return angle;}
		void setAngle(double _angle){this->angle=_angle;}
	
	private:
		double width;
		double height;
		double x,y;
		double angle;
};

class ACPositionsPluginRadialTreeLayout : public ACNeighborPositionsPlugin {
	public:
		ACPositionsPluginRadialTreeLayout();
	~ACPositionsPluginRadialTreeLayout(){};
//		virtual int initialize();
		void updateNextPositions(ACMediaBrowser* );
		//void prepareLayout(ACOsgBrowserRenderer*, int start){};
		//void updateLayout(ACOsgBrowserRenderer*, double ratio){};
	
	protected:
		ACMediaBrowser* mediaBrowser;
		int DEFAULT_RADIUS;
		int MARGIN;
		int m_maxDepth;
		double m_radiusInc; // radius increment between subsequent rings of the layout when auto-scaling is disabled
		double m_theta1, m_theta2; // starting angle, in radians
		bool m_setTheta;
		bool m_autoScale; // indicates if the layout automatically scales to fit the layout bounds
		ACPoint m_origin;
		int m_prevRoot;
		double m_ax, m_ay; // for holding anchor co-ordinates
		vector<ACPositionsPluginRadialTreeNodeParams*> m_nodeParams;
	
	public:
		double getRadiusIncrement() {return m_radiusInc;}
		void setRadiusIncrement(double inc) {m_radiusInc = inc;}
		bool getAutoScale() {return m_autoScale;}
		void setAutoScale(bool s) {m_autoScale = s;}
		void setAngularBounds(double theta, double width) {
			m_theta1 = theta;
			m_theta2 = theta+width;
			m_setTheta = true;
		};// constrains this layout to the specified angular sector
	
	protected:
		void setScale(double width, double height);
 	
	private:
		void calcAngularBounds(int r); // calculates the angular bounds of the layout, attempting to preserve the angular orientation of the display across transitions.
		double calcAngularWidth(int n, int d); // computes relative measures of the angular widths of each expanded subtree. Node diameters are taken into account to improve space allocation for variable-sized nodes.
		double normalize(double angle);
		vector<int> sortedChildren(int n);
	
	protected:
		void layout(int n, double r, double theta1, double theta2);
		void setPolarLocation(int n, double r, double t);
	
	private:
		ACPositionsPluginRadialTreeNodeParams* getParams(int item);
};

#endif	/* _ACPOSITIONSPLUGINRADIALTREELAYOUT_ */