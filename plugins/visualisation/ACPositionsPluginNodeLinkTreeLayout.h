/*
 *  ACPositionsPluginNodeLinkTreeLayout.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 11/03/2010
 *
 *  heavily based on the NodeLinkTreeLayout from Prefuse.org
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
 * Tree layout that computes a tidy layout of a node-link tree
 * diagram. This algorithm lays out a rooted tree such that each
 * depth level of the tree is on a shared line. The orientation of the
 * tree can be set such that the tree goes left-to-right (default),
 * right-to-left, top-to-bottom, or bottom-to-top.</p>
 * 
 * The algorithm used is that of Christoph Buchheim, Michael Junger,
 * and Sebastian Leipert from their research paper
 * http://citeseer.ist.psu.edu/buchheim02improving.html
 * Improving Walker's Algorithm to Run in Linear Time, Graph Drawing 2002.
 * This algorithm corrects performance issues in Walker's algorithm, which
 * generalizes Reingold and Tilford's method for tidy drawings of trees to
 * support trees with an arbitrary number of children at any given node.</p>
 */

#include "ACPlugin.h"
#include "ACMediaBrowser.h"

#ifndef _ACPOSITIONSPLUGINNODELINKTREELAYOUT_
#define _ACPOSITIONSPLUGINNODELINKTREELAYOUT_

typedef enum {
	ORIENT_LEFT_RIGHT = 0,
	ORIENT_RIGHT_LEFT = 1,
	ORIENT_TOP_BOTTOM = 2,
	ORIENT_BOTTOM_TOP = 3,
	ORIENT_CENTER     = 4,
	ORIENTATION_COUNT = 5,
} ACPositionsPluginNodeLinkTreeOrientation;

class ACPositionsPluginTreeNodeParams {
	public:	
		ACPositionsPluginTreeNodeParams();
		~ACPositionsPluginTreeNodeParams(){};
	public:
		int getWidth(){return width;}
		int getHeight(){return height;}
		double getPrelim(){return prelim;}
		void setPrelim(double _prelim){this->prelim=_prelim;}
		double getMod(){return mod;}
		void setMod(double _mod){this->mod=_mod;}
		double getShift(){return shift;}
		void setShift(double _shift){this->shift=_shift;}
		double getChange(){return change;}
		void setChange(double _change){this->change=_change;}
		int getNumber(){return number;}
		void setNumber(int _number){this->number=_number;}
		int getAncestor(){return ancestor;}
		void setAncestor(int _ancestor){this->ancestor=_ancestor;}
		int getThread(){return thread;}
		void setThread(int _thread){this->thread=_thread;}
		double getX(){return x;}
		void setX(double _x){this->x=_x;}
		double getY(){return y;}
		void setY(double _y){this->y=_y;}
	
	private:
		double x,y;
		double width;
		double height;
		double prelim;
		double mod;
		double shift;
		double change;
		int number;
		int ancestor;
		int thread;
};

class ACPositionsPluginNodeLinkTreeLayout : public ACPlugin {
	public:
		ACPositionsPluginNodeLinkTreeLayout();
		~ACPositionsPluginNodeLinkTreeLayout();
		virtual int initialize();
		void updateClusters(ACMediaBrowser* ){};//not implemented
		void updateNextPositions(ACMediaBrowser* );
		void updateNeighborhoods(ACMediaBrowser* ){};//not implemented
	protected:
		ACMediaBrowser* mediaBrowser;
		ACPositionsPluginNodeLinkTreeOrientation    m_orientation;  // the orientation of the tree
		double m_bspace;   // the spacing between sibling nodes
		double m_tspace;  // the spacing between subtrees
		double m_dspace;  // the spacing between depth levels
		double m_offset;  // pixel offset for root node position
		vector<double> m_depths;
		int      m_maxDepth;
		double m_ax, m_ay; // for holding anchor co-ordinates
		vector<ACPositionsPluginTreeNodeParams> m_nodeParams;
	
	public:
		void setOrientation(ACPositionsPluginNodeLinkTreeOrientation orientation);		
		int getOrientation() {return m_orientation;}
		void setDepthSpacing(double d) {m_dspace = d;}
		double getDepthSpacing() {return m_dspace;}
		void setBreadthSpacing(double b) {m_bspace = b;}
		double getBreadthSpacing() {return m_bspace;}
		void setSubtreeSpacing(double s) {m_tspace = s;}
		double getSubtreeSpacing() {return m_tspace;}
		void setRootNodeOffset(double o) {m_offset = o;} // The dimension in which this offset is applied is dependent upon the orientation of the tree.
		double getRootNodeOffset() {return m_offset;}
	
 	private:
		double spacing(int l, int r, bool siblings);
		void updateDepths(int depth, int item);
		void determineDepths();
	
	private:
		void firstWalk(int n, int num, int depth);
		int apportion(int v, int a);
		int nextLeft(int n);
		int nextRight(int n);
		void moveSubtree(int wm, int wp, double shift);
		void executeShifts(int n);
		int ancestor(int vim, int v, int a);
		void secondWalk(int n, int p, double m, int depth);
		void setBreadth(int n, int p, double b);
		void setDepth(int n, int p, double d);
};

#endif	/* _ACPOSITIONSPLUGINNODELINKTREELAYOUT_ */