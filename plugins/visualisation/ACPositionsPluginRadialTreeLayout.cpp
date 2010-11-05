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
 */

#include "ACPositionsPluginRadialTreeLayout.h"

#define PI 3.1415926535897932384626433832795f

//CF: uncomment this if you need to adapt ACPositionsPluginRadialTreeNodeParams::sortedChildren()
//#include <armadillo>
//using namespace arma;

using namespace std;

ACPositionsPluginRadialTreeNodeParams::ACPositionsPluginRadialTreeNodeParams()
: width(10),height(10),x(0.0),y(0.0),angle(0.0) {
}

ACPositionsPluginRadialTreeLayout::ACPositionsPluginRadialTreeLayout() 
: DEFAULT_RADIUS(50), MARGIN(30), m_maxDepth(0),m_radiusInc(DEFAULT_RADIUS),
	m_theta1(0.0f), m_theta2(m_theta1 + 2*PI), m_setTheta(false), m_autoScale(true),
	m_prevRoot(-1) {
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
    this->mPluginType = PLUGIN_TYPE_NEIGHBORS_POSITIONS;
    this->mName = "RadialTreeLayoutPositions";
    this->mDescription = "Plugin for the computation of positions and layout for a radial node-link tree layout";
    this->mId = "";
	
    //local vars
	m_origin.x=0.0f;
	m_origin.y=0.0f;
	m_nodeParams = vector<ACPositionsPluginRadialTreeNodeParams*>();
}

int ACPositionsPluginRadialTreeLayout::initialize() {
    std::cout << "ACPositionsPluginRadialTreeLayout initialized" << std::endl;
    return 0;
}

void ACPositionsPluginRadialTreeLayout::updateNextPositions(ACMediaBrowser* _mediaBrowser) {
	std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions" << std::endl;

	if (mediaBrowser == NULL){
		mediaBrowser = _mediaBrowser;
	}
	if (!(mediaBrowser->getUserLog()->isEmpty()))
	{
		mediaBrowser->setLayout(AC_LAYOUT_TYPE_NODELINK);
		int librarySize = mediaBrowser->getLibrary()->getSize();
		
		if(librarySize==0) {
			mediaBrowser->setNeedsDisplay(true);
			return;
		}
		
		// init params:
		m_nodeParams.resize(mediaBrowser->getUserLog()->getSize());
		for(int n=0; n<mediaBrowser->getUserLog()->getSize(); n++)
			m_nodeParams[n]= new ACPositionsPluginRadialTreeNodeParams();
		
		int n = 0; // root node
		ACPositionsPluginRadialTreeNodeParams* np = getParams(n);
		m_maxDepth = 0;
		
		//std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions::calcAngularWidth" << std::endl;
		calcAngularWidth(n, 0);

		//std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions::setScale" << std::endl;
		if ( m_autoScale ) setScale(1,1);
		
		//std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions::calcAngularBounds" << std::endl;
		if ( !m_setTheta ) calcAngularBounds(n);

		// perform the layout
		//std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions::layout" << std::endl;
		if ( m_maxDepth > 0 )
			layout(n, m_radiusInc, m_theta1, m_theta2);
		
		// update properties of the root node
		m_nodeParams[n]->setX(m_origin.x);
		m_nodeParams[n]->setY(m_origin.y);
		np->setAngle(m_theta2-m_theta1);
		
		ACPoint p;
		for(int n=0; n<mediaBrowser->getUserLog()->getSize(); n++)
		{
			// CF: hack so that newly expanded nodes animate from their parental position
			int pn = mediaBrowser->getUserLog()->getParentFromNodeId(n);
			p = mediaBrowser->getMediaNode(n).getCurrentPosition();
			if ( pn > 0 && p.x == 0.0f && p.y == 0.0f)
				mediaBrowser->getMediaNode(n).setCurrentPosition(mediaBrowser->getMediaNode(pn).getCurrentPosition());
			
			p.x = m_nodeParams[n]->getX()/150;
			p.y = -m_nodeParams[n]->getY()/150;
			p.z = 0;
			mediaBrowser->setNodeNextPosition(n, p); // CF: note OSG's inverted Y //CF n instead of mediaBrowser->getUserLog()->getMediaIdFromNodeId(n)
			mediaBrowser->setLoopIsDisplayed(n, true); // CF n instead of mediaBrowser->getUserLog()->getMediaIdFromNodeId(n)
			//std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions: Node " << n << " x " << m_nodeParams[n]->getX() << " y " << -m_nodeParams[n]->getY() << std::endl;
		}

		mediaBrowser->setNeedsDisplay(true);
	}
}

void ACPositionsPluginRadialTreeLayout::setScale(double width, double height)
{
	double r = min(width,height)/2.0;
	if ( m_maxDepth > 0 )
		m_radiusInc = (r-MARGIN)/m_maxDepth;
}

void ACPositionsPluginRadialTreeLayout::calcAngularBounds(int r)
{
	if ( m_prevRoot == -1 || r == m_prevRoot ) // || !m_prevRoot.isValid()
	{
		m_prevRoot = r;
		return;
	}
	
	// try to find previous parent of root
	int p = m_prevRoot;
	while ( true ) {
		int pp = mediaBrowser->getUserLog()->getParentFromNodeId(p);
		if ( pp == r ) {
			break;
		} else if ( pp == -1 ) {
			m_prevRoot = r;
			return;
		}
		p = pp;
	}
	
	// compute offset due to children's angular width
	double dt = 0;
	vector<int> childs = sortedChildren(r);

	for (int i=0;i<childs.size(); i++)
	{
		int n = childs[i];
		if ( n == p ) break;
		dt += getParams(n)->getWidth();
	}	
	
	double rw = getParams(r)->getWidth();
	double pw = getParams(p)->getWidth();
	dt = -2*PI * (dt+pw/2)/rw;
	
	// set angular bounds
	m_theta1 = dt + atan2(m_nodeParams[p]->getY()-m_nodeParams[r]->getY(), m_nodeParams[p]->getX()-m_nodeParams[r]->getX());
	m_theta2 = m_theta1 + 2*PI;
	m_prevRoot = r;     
}

double ACPositionsPluginRadialTreeLayout::calcAngularWidth(int n, int d)
{
	if ( d > m_maxDepth ) m_maxDepth = d;       
	double aw = 0.0f;
	
	double w = m_nodeParams[n]->getWidth();
	double h = m_nodeParams[n]->getHeight();

	double diameter = d == 0.0f ? 0.0f : sqrt(w*w+h*h) / d;
	
	if ( mediaBrowser->getUserLog()->getChildCountAtNodeId(n) > 0 ) { // CF: should be "if the branch at node n is expanded"
		int child = mediaBrowser->getUserLog()->getFirstChildFromNodeId(n);
		while ( child!= -1) {
			aw += calcAngularWidth(child,d+1);
			child = mediaBrowser->getUserLog()->getNextSiblingFromNodeId(child);
		}
		aw = max(diameter, aw);
	} else {
		aw = diameter;
	}
	getParams(n)->setWidth(aw);
	//std::cout << "calcAngularWidth: node=" << n << " aw=" << aw << std::endl;
	return aw;	
}	

double ACPositionsPluginRadialTreeLayout::normalize(double angle)
{
	while ( angle > 2*PI ) {
		angle -= 2*PI;
	}
	while ( angle < 0 ) {
		angle += 2*PI;
	}
	return angle;
}

vector<int> ACPositionsPluginRadialTreeLayout::sortedChildren(int n)
{
	double base = 0;
	// update base angle for node ordering
	int p = mediaBrowser->getUserLog()->getParentFromNodeId(n);
	if ( p != -1 ) {
		base = normalize(atan2(m_nodeParams[p]->getY()-m_nodeParams[n]->getY(), m_nodeParams[p]->getX()-m_nodeParams[n]->getX()));
	}

	int cc = mediaBrowser->getUserLog()->getChildCountAtNodeId(n);
	if ( cc == 0 ) return vector<int>();//CF -1?
	
	int c = mediaBrowser->getUserLog()->getFirstChildFromNodeId(n);
	
	vector<int> _children;
	// CF: message from the Prefuse team:
	//   TODO: this is hacky and will break when filtering
	//   how to know that a branch is newly expanded?
	//   is there an alternative property we should check?
	// They tested "if ( !c.isStartVisible() )"
	// TO CHECK It seems they offer an alternative way to sort the childrens of a given node so as to dynamicly update the viz without recomputing the whole graph.
	// As we're not dealing with huge graphs FOR NOW and as we can't easily translate the test condition above, this feature is momentarily disabled.
	// To re-enable it, uncomment the following /* */ and armadillo at the beginning of this file.
    // We could test "if c is part of a new branch is being expanded at node n" by checking if the child(ren) positions are set (ie non-zero). 
	/*
	if ( mediaBrowser->getMediaNode(c).getCurrentPositionX() == 0.0f && mediaBrowser->getMediaNode(c).getCurrentPositionY() == 0.0f)//p>0
	{	 
	*/
		// use natural ordering for previously invisible nodes
		int i=0;
		while (i<cc)
		{
			_children.push_back(c);
			c = mediaBrowser->getUserLog()->getNextSiblingFromNodeId(c);
			++i; 
		}	
		return _children;
	/*
	}
	else
	{
		colvec angle(cc);
		ucolvec idx(cc);

		int i=0;
		while (i<cc)
		{
			angle(i) = normalize(-base +atan2(m_nodeParams[c]->getY()-m_nodeParams[n]->getY(), m_nodeParams[c]->getX()-m_nodeParams[n]->getX()));
			//std::cout << "angle("<<i<<") "<<angle(i)<< " for c=" << c << std::endl;
			++i; 
			c = mediaBrowser->getUserLog()->getNextSiblingFromNodeId(c);
		}
		idx = sort_index(angle);

		for (i=0;i<cc;i++)
			_children.push_back(mediaBrowser->getUserLog()->getNthChildAtNodeId(n,idx(i)));
		return _children;
	}
	*/ 
}

void ACPositionsPluginRadialTreeLayout::layout(int n, double r, double theta1, double theta2)
{
	std::cout << "ACPositionsPluginRadialTreeLayout::updateNextPositions::layout node " << n << std::endl;
	double dtheta  = (theta2-theta1);
	double dtheta2 = dtheta / 2.0;
	double width = getParams(n)->getWidth();
	double cfrac, nfrac = 0.0;
	
	vector<int> childs = sortedChildren(n);
	for (int i=0;i<childs.size(); i++)
	{
		//childIter++;
		int c = childs[i];//*childIter;
		ACPositionsPluginRadialTreeNodeParams* cp = getParams(c);
		cfrac = cp->getWidth() / width;
		if (  mediaBrowser->getUserLog()->getChildCountAtNodeId(c) > 0 ) { // && c.isExpanded()
			layout(c, r+m_radiusInc, theta1 + nfrac*dtheta, theta1 + (nfrac+cfrac)*dtheta);
		}
		setPolarLocation(c,r, theta1 + nfrac*dtheta + cfrac*dtheta2);
		std::cout<< "setPolarLocation(c:"<<c<<",r:"<<r<<" theta1 + nfrac*dtheta + cfrac*dtheta2 " << theta1 + nfrac*dtheta + cfrac*dtheta2 << " "<<std::endl;
		cp->setAngle(cfrac*dtheta);
		nfrac += cfrac;
	}	
}	

void ACPositionsPluginRadialTreeLayout::setPolarLocation(int n, double r, double t) {
	m_nodeParams[n]->setX(m_origin.x + r*cos(t));
	m_nodeParams[n]->setY(m_origin.y + r*sin(t));
}	

ACPositionsPluginRadialTreeNodeParams* ACPositionsPluginRadialTreeLayout::getParams(int item) {
	return m_nodeParams[item];
}


/*
void ACPositionsPluginRadialTreeLayout::prepareLayout(ACOsgBrowserRenderer*, int start)
{
	std::cout << "ACPositionsPluginRadialTreeLayout::prepareLayout" << std::endl;
}
void ACPositionsPluginRadialTreeLayout::updateLayout(ACOsgBrowserRenderer*, double ratio)
{
	std::cout << "ACPositionsPluginRadialTreeLayout::updateLayout" << std::endl;
}
*/
