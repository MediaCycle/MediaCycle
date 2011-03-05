/*
 *  ACOsgBrowserRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
 *  @copyright (c) 2009 – UMONS - Numediart
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

#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)

#include "ACOsgImageRenderer.h"
#include <ACImage.h>
#include <ACVideo.h>

using namespace osg;

#define IMAGE_BORDER

const int ACOsgImageRenderer::NCOLORS = 5;

ACOsgImageRenderer::ACOsgImageRenderer() {
	
	image_image = 0; image_geode = 0; border_geode = 0; image_transform = 0;
	
	Vec4 color(1.0f, 1.0f, 1.0f, 0.9f);	
	Vec4 color2(0.2f, 0.8f, 0.2f, 1.0f);	
	Vec4 color3(0.4f, 0.4f, 0.4f, 1.0f);	
	colors = new Vec4Array;
	colors2 = new Vec4Array;
	colors3 = new Vec4Array;

	colors->push_back(color);		
	colors2->push_back(color2);	
	colors3->push_back(color3);	

	
// was in updateNodes but not used...
//		colors[0] = Vec4(0.2,0.6,0.2,1);
//		colors[1] = Vec4(0.4,0.4,0.4,1);
//		colors[2] = Vec4(0.5,1,1,1);
//		colors[3] = Vec4(1,0.5,0.5,1);
//		colors[4] = Vec4(0.5,1,0.5,1);
	
}

ACOsgImageRenderer::~ACOsgImageRenderer() {
	// smart pointers (ref_ptr) will take care of garbage collection
//	delete colors;
//	delete colors2;
//	delete colors3;
	if (image_geode) { //ref_ptr//image_geode->unref();
		image_geode=0; }
	if (border_geode) { //ref_ptr//border_geode->unref();
		border_geode=0; }
	if (image_transform) { //ref_ptr//image_transform->unref(); 
		image_transform=0; }
}

void ACOsgImageRenderer::imageGeode(bool flip, float sizemul, float zoomin) {
	ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();
	if (media_type == MEDIA_TYPE_VIDEO)
		flip=true; //CF this hides a huger bug, to be tracked down (imageGeode created twice sometimes at video ACL import)(media_type == MEDIA_TYPE_IMAGE)
		
	std::cout << "Do we flip the image geode? " << flip << std::endl;
	int i;
	
	double xstep = 0.0005;
	
	float zpos = 0.02;
	double ylim = 0.025, xlim = 0.025;
	double imagex, imagey;
	
	double imagesurf;
	
	ylim *=sizemul;
	xlim *=sizemul;
	
	int width, height;
	//IplImage* thumbnail;
	//osg::ref_ptr<osg::Image> thumbnail;
	std::string thumbnail_filename;
	
	StateSet *state;
	
	Vec3Array* vertices;
	DrawElementsUInt* line_p;
	Vec2Array* texcoord;
	
	Geometry *image_geometry;
	Geometry *border_geometry;
	Texture2D *image_texture;
		
	// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
	int media_index = node_index; // or media_cycle->getBrowser()->getMediaNode(node_index).getMediaId(); 
	if (media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
		media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(node_index);	
	
	if (media_index<0)
		media_index = 0;
	
	width = media_cycle->getThumbnailWidth(media_index);//CF instead of node_index
	height = media_cycle->getThumbnailHeight(media_index);//CF instead of node_index
	//std::cout << "Geode with (thumbnail) width " <<  width << " and height " << height << std::endl;
	
	image_transform = new MatrixTransform();
	
	image_geode = new Geode();
	image_geometry = new Geometry();	

	zpos = zpos - 0.00001 * node_index;
	
	// image vertices
	float scale;
	imagesurf = xlim * ylim;
	scale = sqrt ((float)imagesurf / (width*height));
	imagex = scale * width;
	imagey = scale * height;
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(-imagex, -imagey, zpos);
	(*vertices)[1] = Vec3(imagex, -imagey, zpos);
	(*vertices)[2] = Vec3(imagex, imagey, zpos);
	(*vertices)[3] = Vec3(-imagex, imagey, zpos);
	image_geometry->setVertexArray(vertices);
	
	// Primitive Set
	DrawElementsUInt *poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	image_geometry->addPrimitiveSet(poly);
	
	// State Set
	state = image_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	//state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); //CF from OSG's examples/osgmovie.cpp, doesn't solve the transparent first frame for video geodes
	
	// Texture Coordinates
	texcoord = new Vec2Array;
	
// XS TODO!!
//http://lists.openscenegraph.org/pipermail/osg-users-openscenegraph.org/2009-August/032147.html
//	other ways of flipping:	
//	* use osg::Image's flipVertical()
//	* just flip the texture coordinates
	
	
	float a = (1.0-(1.0/zoomin)) / 2.0;
	float b = 1.0-a;
	texcoord->push_back(osg::Vec2(a, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? a : b));
	texcoord->push_back(osg::Vec2(a, flip ? a : b));
	image_geometry->setTexCoordArray(0, texcoord);	
	
	// Texture State (image)
	
	//ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();
	if (media_type == MEDIA_TYPE_IMAGE)
	{	
		image_texture = ((ACImage*)(media_cycle->getLibrary()->getMedia(media_index)))->getTexture();
	}
	else if (media_type == MEDIA_TYPE_VIDEO)
	{
		image_texture = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getTexture();
	}
	image_texture->setResizeNonPowerOfTwoHint(false); 
	
	// XS TODO add this line?
	// http://groups.google.com/group/osg-users/browse_thread/thread/f623b62f62e39473?pli=1
	//image_texture->setUnRefImageDataAfterApply(true);
	state = image_geometry->getOrCreateStateSet();
	state->setTextureAttribute(0, image_texture);
	state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
	
	image_geometry->setColorArray(colors);
	image_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	image_geode->addDrawable(image_geometry);
	
	image_transform->addChild(image_geode);
	
#ifdef IMAGE_BORDER
	/*
	//CF alternate version
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); //CF from OSG's examples/osgmovie.cpp, doesn't solve the transparent first frame for video geodes
	state->setMode(GL_BLEND,osg::StateAttribute::ON);
	
	osg::Vec3Array* normals = new osg::Vec3Array(1);
    (*normals)[0].set(-1.0f,0.0f,0.0f);
    image_geometry->setNormalArray(normals);
    image_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	image_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.
	//image_texture->setBorderColor(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	image_texture->setBorderColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//image_texture->setBorderWidth(1.5);
	image_texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
	image_texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
	//CF alternate version end
	*/
	
	border_geode = new Geode();
	border_geometry = new Geometry();	
	
	// border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
	(*vertices)[1] = Vec3(imagex+xstep, -imagey-xstep, zpos);
	(*vertices)[2] = Vec3(imagex+xstep, imagey+xstep, zpos);
	(*vertices)[3] = Vec3(-imagex-xstep, imagey+xstep, zpos);
	(*vertices)[4] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
	border_geometry->setVertexArray(vertices);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);	
	for(i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p);
	
	border_geometry->setColorArray(colors3);// XS was : colors2, but (0.4f, 0.4f, 0.4f, 1.0f)
	border_geometry->setColorBinding(Geometry::BIND_OVERALL);
	state = border_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	
	state = border_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
	border_geode->addDrawable(border_geometry);
	
	image_transform->addChild(border_geode);
	
	//(*colors2)[0] = Vec4(1.0f, 1.0f, 1.0f, 0.5f);
#endif	
	
	//sprintf(name, "some audio element");
	image_transform->setUserData(new ACRefId(node_index));
	//curser_transform->setName(name);
	//ref_ptr//image_transform->ref();
	//sprintf(name, "some audio element");
	image_geode->setUserData(new ACRefId(node_index));
	//waveform_geode->setName(name);
#ifdef IMAGE_BORDER	
	//CF no need for alternate version
	//ref_ptr//image_geode->ref();	
	//ref_ptr//border_geode->ref();
#endif	
}

void ACOsgImageRenderer::prepareNodes() {
	
	// SD TODO - move this upwards?
	int media_index = node_index; // or media_cycle->getBrowser()->getMediaNode(node_index).getMediaId(); 
	if (media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
		media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(node_index);
	
	if  (media_cycle->getMediaNode(node_index).isDisplayed()) {
		if (!image_geode) {
			imageGeode();
			media_node->addChild(image_transform);
		}
	}
	
	prev_media_index = media_index;
}

void ACOsgImageRenderer::updateNodes(double ratio) {
	
	float x, y, z;
	float zpos = 0.001;
	
	const ACMediaNode &attribute = media_cycle->getMediaNode(node_index);
	
	const ACPoint &p = attribute.getCurrentPosition(), &p2 = attribute.getNextPosition();
	double omr = 1.0-ratio;
	
	
	int media_index = node_index; // or media_cycle->getBrowser()->getMediaNode(node_index).getMediaId(); 
	if (media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
		media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(node_index);
	
	if (media_index!=prev_media_index) {
		if(media_node->getNumChildren() == 1) {
			media_node->removeChild(0, 1);
		}
		imageGeode();
		media_node->addChild(image_transform);
		prev_media_index = media_index;
	}
	
	unsigned int mask = (unsigned int)-1;
	if(attribute.getNavigationLevel() >= media_cycle->getNavigationLevel()) {
		image_transform->setNodeMask(mask);
	}
	else {
		image_transform->setNodeMask(0);
	}
	
	z = 0;
	
	//	if (border_geode->getDrawable(0)) {
	//		if (attribute.getActivity()==1) {
	//			((Geometry*)border_geode->getDrawable(0))->setColorArray(colors2);
	//		}
	//		else {
	//			((Geometry*)border_geode->getDrawable(0))->setColorArray(colors3);
	//		}
	//	}
	
	float localscale;
	float maxdistance = 0.2;
	float maxscale = 3;//1.5;//CF
	float minscale = 0.6;				
	// Apply "rotation" to compensate camera rotation
	x = omr*p.x + ratio*p2.x;
	y = omr*p.y + ratio*p2.y;
	localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
	localscale = max(localscale,minscale);
	if (localscale>minscale) {
		z += 2*zpos;
	}
	else if (attribute.getActivity()==1) {
		z += zpos;
	}

#ifdef AUTO_TRANSFORM
	media_node->setPosition(Vec3(x,y,z));
	media_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
	media_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));	
#else

	Matrix T;	
	T.makeTranslate(Vec3(x, y, z)); // omr*p.z + ratio*p2.z));	
	T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0)) 
			* Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom) 
			* T;
	media_node->setMatrix(T);
#endif //AUTO_TRANSFORM

}
#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)