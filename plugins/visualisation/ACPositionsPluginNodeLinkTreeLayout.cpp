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
 */

#include "ACPositionsPluginNodeLinkTreeLayout.h"

using namespace std;

ACPositionsPluginTreeNodeParams::ACPositionsPluginTreeNodeParams()
: x(0.0),y(0.0),width(10),height(10),
	prelim(0.0),mod(0.0),shift(0.0),change(0.0),
	number(-2),ancestor(-1),thread(-1)//or -1?
{
}

void ACPositionsPluginTreeNodeParams::init(int item)
{
	ancestor = item;
	number = -1;	
}

void ACPositionsPluginTreeNodeParams::clear()
{
	number = -2;
	prelim = mod = shift = change = 0;
	ancestor = -1;
	thread = -1;	
}	

ACPositionsPluginNodeLinkTreeLayout::ACPositionsPluginNodeLinkTreeLayout() 
//: m_bspace(5), m_tspace(25), m_dspace(50), m_offset(50),m_maxDepth(0),m_ax(0.0),m_ay(0.0) // from Prefuse Java
: m_bspace(0.005), m_tspace(0.025), m_dspace(0.05), m_offset(0.05),m_maxDepth(0),m_ax(0.0),m_ay(0.0) 
{
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "NodeLinkTreeLayoutPositions";
    this->mDescription = "Plugin for the computation of positions and layout for a node-link tree layout";
    this->mId = "";
	
    //local vars
	m_depths = vector<double>();
	m_depths.resize(10);
	m_orientation = ORIENT_LEFT_RIGHT;//CF: the current one is "Finder-like", try ORIENT_TOP_BOTTOM; or...
	m_nodeParams = vector<ACPositionsPluginTreeNodeParams*>();
}

ACPositionsPluginNodeLinkTreeLayout::~ACPositionsPluginNodeLinkTreeLayout() {
}

int ACPositionsPluginNodeLinkTreeLayout::initialize()
{
    std::cout << "ACPositionsPluginNodeLinkTreeLayout initialized" << std::endl;
    return 0;
}

void ACPositionsPluginNodeLinkTreeLayout::updateNextPositions(ACMediaBrowser* _mediaBrowser) {
	std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateNextPositions" << std::endl;
	//std::cout << "UserLogTree: is empty " << _mediaBrowser->getUserLog()->isEmpty() << " size " << _mediaBrowser->getUserLog()->getSize() << " max depth " << _mediaBrowser->getUserLog()->getMaxDepth() << " last clicked node " << _mediaBrowser->getUserLog()->getLastClickedNodeId() << std::endl;

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
		m_maxDepth = 0;
		m_depths.clear();
		m_depths.resize(10); // to init it at its default size
		for (int d = 0; d++; d<m_depths.size())
			m_depths[d]=0;
		m_nodeParams.clear();
		m_nodeParams.resize(mediaBrowser->getUserLog()->getSize());
		for(int n=0; n<mediaBrowser->getUserLog()->getSize(); n++)
			m_nodeParams[n]= new ACPositionsPluginTreeNodeParams();
		
		// do first pass - compute breadth information, collect depth info
		std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateNextPositions:firstWalk"<< std::endl;
		ACPositionsPluginTreeNodeParams* rp = getParams(0);		
		this->firstWalk(0, 0, 1);
		
		for(int n=0; n<mediaBrowser->getUserLog()->getSize(); n++)
			std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateNextPositions: Node " << n << " with prelim " << m_nodeParams[n]->getPrelim() << std::endl;
		
		// sum up the depth info
		std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateNextPositions: determineDepths"<< std::endl;
		this->determineDepths();
		
		// do second pass - assign layout positions
		std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateNextPositions: secondWalk"<< std::endl;
		this->secondWalk(0, NULL, -rp->getPrelim(), 0);
		
		for(int n=0; n<mediaBrowser->getUserLog()->getSize(); n++)
		{
			// TODO: check next
			mediaBrowser->setNodeNextPosition(n, m_nodeParams[n]->getX()/200, -m_nodeParams[n]->getY()/200); // CF: note OSG's inverted Y //CF n instead of mediaBrowser->getUserLog()->getMediaIdFromNodeId(n)
			mediaBrowser->setLoopIsDisplayed(n, true); // CF n instead of mediaBrowser->getUserLog()->getMediaIdFromNodeId(n)
			std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateNextPositions: Node " << n << " x " << m_nodeParams[n]->getX() << " y " << -m_nodeParams[n]->getY() << std::endl;
		}

		mediaBrowser->setNeedsDisplay(true);
	}
}	

void ACPositionsPluginNodeLinkTreeLayout::setOrientation(ACPositionsPluginNodeLinkTreeOrientation orientation) {
	if ( orientation < 0 || 
		orientation >= ORIENTATION_COUNT ||
		orientation == ORIENT_CENTER )
	{
		std::cerr << "Unsupported orientation value: " << orientation << std::endl;
	}
	m_orientation = orientation;
}

double ACPositionsPluginNodeLinkTreeLayout::spacing(int l, int r, bool siblings) {
	bool w = ( m_orientation == ORIENT_TOP_BOTTOM ||
			  m_orientation == ORIENT_BOTTOM_TOP );
	return (siblings ? m_bspace : m_tspace) + 0.5 *
	( w ? m_nodeParams[l]->getWidth() + m_nodeParams[r]->getWidth()
	 : m_nodeParams[l]->getHeight() + m_nodeParams[r]->getHeight() );
}

void ACPositionsPluginNodeLinkTreeLayout::updateDepths(int depth, int item) {
	bool v = ( m_orientation == ORIENT_TOP_BOTTOM ||  m_orientation == ORIENT_BOTTOM_TOP );
	double d = ( v ? m_nodeParams[item]->getHeight() : m_nodeParams[item]->getWidth() );
	if ( m_depths.size() <= (unsigned int)depth )
		m_depths.resize(3*depth/2);
	m_depths[depth] = max(m_depths[depth], d);
	m_maxDepth = max(m_maxDepth, depth);
}

void ACPositionsPluginNodeLinkTreeLayout::determineDepths() {
	for ( int i=1; i<m_maxDepth; ++i )
		m_depths[i] += m_depths[i-1] + m_dspace;
}

void ACPositionsPluginNodeLinkTreeLayout::firstWalk(int n, int num, int depth) {
	std::cout << "firstWalk n" << n << " num " << num << " depth "<< depth << std::endl;
	ACPositionsPluginTreeNodeParams* np = getParams(n);
	m_nodeParams[n]->setNumber(num);
	updateDepths(depth, n);
	
	bool expanded = mediaBrowser->getUserLog()->getNodeFromId(n).getVisibility(); //n.isExpanded();
	
	if ( mediaBrowser->getUserLog()->getChildCountAtNodeId(n) == 0 || !expanded ) // is leaf
	{ 
		int l = mediaBrowser->getUserLog()->getPreviousSiblingFromNodeId(n);
		if ( l == -1 ) {
			np->setPrelim(0.0);
		} else {
			np->setPrelim( getParams(l)->getPrelim() + spacing(l,n,true));
		}
	}
	else if ( expanded )
	{
		int leftMost = mediaBrowser->getUserLog()->getFirstChildFromNodeId(n);
		int rightMost = mediaBrowser->getUserLog()->getLastChildFromNodeId(n);
		int defaultAncestor = leftMost;
		int c = leftMost;
		//for ( int i=0; c > -1; ++i, c = mediaBrowser->getUserLog()->getNextSiblingFromNodeId(c) )
		int i=0;
		while (c != -1)
		{
			firstWalk(c, i, depth+1);
			defaultAncestor = apportion(c, defaultAncestor);
			++i; 
			c = mediaBrowser->getUserLog()->getNextSiblingFromNodeId(c);
		}
		
		executeShifts(n);
		
		double midpoint = 0.5 * (getParams(leftMost)->getPrelim() + getParams(rightMost)->getPrelim());
		
		int left = mediaBrowser->getUserLog()->getPreviousSiblingFromNodeId(n);
		if ( left != -1 ) {
			np->setPrelim( getParams(left)->getPrelim() + spacing(left, n, true));
			np->setMod( np->getPrelim() - midpoint);
		} else {
			np->setPrelim( midpoint );
		}
	}
}

int ACPositionsPluginNodeLinkTreeLayout::apportion(int v, int a) {        
	int w = mediaBrowser->getUserLog()->getPreviousSiblingFromNodeId(v);
	if ( w != -1 ) {
		int vip, vim, vop, vom;
		double   sip, sim, sop, som;
		
		vip = vop = v;
		vim = w;
		vom = mediaBrowser->getUserLog()->getFirstChildFromNodeId( mediaBrowser->getUserLog()->getParentFromNodeId(vip) );

		sip = getParams(vip)->getMod();
		sop = getParams(vop)->getMod();
		sim = getParams(vim)->getMod();
		som = getParams(vom)->getMod();
		
		int nr = nextRight(vim);
		int nl = nextLeft(vip);
		while ( nr != -1 && nl != -1 ) {
			vim = nr;
			vip = nl;
			vom = nextLeft(vom);
			vop = nextRight(vop);
			getParams(vop)->setAncestor(v);
			double shift = ( getParams(vim)->getPrelim() + sim) - (getParams(vip)->getPrelim() + sip) + spacing(vim,vip,false);
			if ( shift > 0 ) {
				moveSubtree(ancestor(vim,v,a), v, shift);
				sip += shift;
				sop += shift;
			}
			sim += getParams(vim)->getMod();
			sip += getParams(vip)->getMod();
			som += getParams(vom)->getMod();
			sop += getParams(vop)->getMod();
			
			nr = nextRight(vim);
			nl = nextLeft(vip);
		}
		
		if ( nr != -1 && nextRight(vop) == -1 ) {
			ACPositionsPluginTreeNodeParams* vopp = getParams(vop);
			vopp->setThread(nr);
			vopp->setMod( vopp->getMod() + sim - sop);
		}
		if ( nl != -1 && nextLeft(vom) == -1 ) {
			ACPositionsPluginTreeNodeParams* vomp = getParams(vom);
			vomp->setThread(nl);
			vomp->setMod( vomp->getMod() + sip - som);
			a = v;
		}
	}
	return a;
}

int ACPositionsPluginNodeLinkTreeLayout::nextLeft(int n) {
	int c = -1;
	if ( mediaBrowser->getUserLog()->getNodeFromId(n).getVisibility() ) c = mediaBrowser->getUserLog()->getFirstChildFromNodeId(n);
	return ( c != -1 ? c : getParams(n)->getThread() );
}

int ACPositionsPluginNodeLinkTreeLayout::nextRight(int n) {
	int c = -1;
	if ( mediaBrowser->getUserLog()->getNodeFromId(n).getVisibility() )
		c = mediaBrowser->getUserLog()->getLastChildFromNodeId(n);
	return ( c != -1 ? c : getParams(n)->getThread() );
}

void ACPositionsPluginNodeLinkTreeLayout::moveSubtree(int wm, int wp, double shift) {
	ACPositionsPluginTreeNodeParams* wmp = getParams(wm);
	ACPositionsPluginTreeNodeParams* wpp = getParams(wp);
	double subtrees = wpp->getNumber() - wmp->getNumber();
	wpp->setChange( wpp->getChange() - shift/subtrees );
	wpp->setShift( wpp->getShift() + shift);
	wmp->setChange( wmp->getChange() + shift/subtrees );
	wpp->setPrelim( wpp->getPrelim() + shift );
	wpp->setMod( wpp->getMod() + shift);
}

void ACPositionsPluginNodeLinkTreeLayout::executeShifts(int n) {
	double shift = 0.0, change = 0.0;
	for ( int c = mediaBrowser->getUserLog()->getLastChildFromNodeId(n);
		 	c != -1; c = mediaBrowser->getUserLog()->getPreviousSiblingFromNodeId(c) )
	{
		ACPositionsPluginTreeNodeParams* cp = getParams(c);
		cp->setPrelim( cp->getPrelim() + shift );
		cp->setMod( cp->getMod() + shift );
		change += cp->getChange();
		shift += cp->getShift() + change;
	}
}

int ACPositionsPluginNodeLinkTreeLayout::ancestor(int vim, int v, int a) {
	int p = mediaBrowser->getUserLog()->getParentFromNodeId(v);
	ACPositionsPluginTreeNodeParams* vimp = getParams(vim);
	if ( mediaBrowser->getUserLog()->getParentFromNodeId( m_nodeParams[vim]->getAncestor() ) == p ) {
		return vimp->getAncestor();
	} else {
		return a;
	}
}

void ACPositionsPluginNodeLinkTreeLayout::secondWalk(int n, int p, double m, int depth) {
	ACPositionsPluginTreeNodeParams* np = getParams(n);
	setBreadth(n, p, np->getPrelim() + m);
	setDepth(n, p, m_depths[depth]);
	
	if ( mediaBrowser->getUserLog()->getNodeFromId(n).getVisibility() ) {
		depth += 1;
		for ( int c = mediaBrowser->getUserLog()->getFirstChildFromNodeId(n);
			 c != -1; c = mediaBrowser->getUserLog()->getNextSiblingFromNodeId(c) )
		{
			secondWalk(c, n, m + np->getMod(), depth);
		}
	}
	np->clear();
}

void ACPositionsPluginNodeLinkTreeLayout::setBreadth(int n, int p, double b) {
	switch ( m_orientation ) {
		case ORIENT_LEFT_RIGHT:
		case ORIENT_RIGHT_LEFT:
			m_nodeParams[n]->setY(m_ay + b);
			break;
		case ORIENT_TOP_BOTTOM:
		case ORIENT_BOTTOM_TOP:
			m_nodeParams[n]->setX(m_ax + b);
			break;
		default:
			std::cerr << "setBreath error" << std::endl;
	}
}

void ACPositionsPluginNodeLinkTreeLayout::setDepth(int n, int p, double d) {
	switch ( m_orientation ) {
		case ORIENT_LEFT_RIGHT:
			m_nodeParams[n]->setX(m_ax + d);
			break;
		case ORIENT_RIGHT_LEFT:
			m_nodeParams[n]->setX(m_ax - d);
			break;
		case ORIENT_TOP_BOTTOM:
			m_nodeParams[n]->setY(m_ay + d);
			break;
		case ORIENT_BOTTOM_TOP:
			m_nodeParams[n]->setY(m_ay - d);
			break;
		default:
			std::cerr << "setDepth error" << std::endl;
	}
}

ACPositionsPluginTreeNodeParams* ACPositionsPluginNodeLinkTreeLayout::getParams(int item) {
	//ACPositionsPluginTreeNodeParams rp = ACPositionsPluginTreeNodeParams();
	//if ((item >=0) && (item<m_nodeParams.size()))
	//{
		//if (m_nodeParams[item] == NULL)
		//	m_nodeParams[item] = ACPositionsPluginTreeNodeParams();
		/*
		rp.setWidth(m_nodeParams[item].getWidth());
		rp.setHeight(m_nodeParams[item].getHeight());
		rp.setPrelim(m_nodeParams[item].getPrelim());
		rp.setMod(m_nodeParams[item].getMod());
		rp.setShift(m_nodeParams[item].getShift());
		rp.setChange(m_nodeParams[item].getChange());
		rp.setNumber(m_nodeParams[item].getNumber());
		rp.setAncestor(m_nodeParams[item].getAncestor());
		rp.setThread(m_nodeParams[item].getThread());
		rp.setX(m_nodeParams[item].getX());
		rp.setY(m_nodeParams[item].getY());
		 */
		
		if ( m_nodeParams[item]->getNumber() == -2 ) {
			m_nodeParams[item]->init(item);
		}	
	
		return m_nodeParams[item];
	//}
	//else
	//	return ACPositionsPluginTreeNodeParams();
}
/*
void ACPositionsPluginNodeLinkTreeLayout::prepareLayout(ACOsgBrowserRenderer*, int start)
{
	std::cout << "ACPositionsPluginNodeLinkTreeLayout::prepareLayout" << std::endl;
}
void ACPositionsPluginNodeLinkTreeLayout::updateLayout(ACOsgBrowserRenderer*, double ratio)
{
	std::cout << "ACPositionsPluginNodeLinkTreeLayout::updateLayout" << std::endl;
}
*/