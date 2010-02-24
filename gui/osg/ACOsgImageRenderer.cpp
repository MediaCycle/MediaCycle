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

#include "ACOsgImageRenderer.h"

#define IMAGE_BORDER

osg::Image* Convert_OpenCV_TO_OSG_IMAGE(IplImage* cvImg)
{
	
	/*cvNamedWindow("T", CV_WINDOW_AUTOSIZE);
	 cvShowImage("T", cvImg);
	 cvWaitKey(0);
	 cvDestroyWindow("T");	
	 */
	
	if(cvImg->nChannels == 3)
	{
		// Flip image from top-left to bottom-left origin
		if(cvImg->origin == 0) {
			cvConvertImage(cvImg , cvImg, CV_CVTIMG_FLIP);
			cvImg->origin = 1;
		}
		
		// Convert from BGR to RGB color format
		//printf("Color format %s\n",cvImg->colorModel);
		if ( !strcmp(cvImg->channelSeq,"BGR") ) {
			cvCvtColor( cvImg, cvImg, CV_BGR2RGB );
		}
		
		osg::Image* osgImg = new osg::Image();
		
		/*temp_data = new unsigned char[cvImg->width * cvImg->height * cvImg->nChannels];
		 for (i=0;i<cvImg->height;i++) {
		 memcpy( (char*)temp_data + i*cvImg->width*cvImg->nChannels, (char*)(cvImg->imageData) + i*(cvImg->widthStep), cvImg->width*cvImg->nChannels);
		 }*/
		
		osgImg->setImage(
						 cvImg->width, //s
						 cvImg->height, //t
						 3, //r
						 3, //GLint internalTextureformat, (GL_LINE_STRIP, 0x0003)
						 6407, // GLenum pixelFormat, (GL_RGB, 0x1907)
						 5121, // GLenum type, (GL_UNSIGNED_BYTE, 0x1401)
						 (unsigned char*)(cvImg->imageData), // unsigned char* data
						 //temp_data,
						 osg::Image::NO_DELETE, // AllocationMode mode (shallow copy)
						 1);//int packing=1); (???)
		
		//printf("Conversion completed\n");
		return osgImg;
	}
	// XS TODO : what happens with BW images ?
	else {
		printf("Unrecognized image type");
		return 0;
	}
	
}

ACOsgImageRenderer::ACOsgImageRenderer() {
	
	image_image = 0; image_geode = 0; border_geode = 0; image_transform = 0;
}

ACOsgImageRenderer::~ACOsgImageRenderer() {
	
	if (image_geode) { image_geode->unref(); image_geode=0; }
	if (border_geode) { border_geode->unref(); border_geode=0; }
	if (image_transform) { image_transform->unref(); image_transform=0; }
	//if (image_image) { delete image_image->data(); }
}

void ACOsgImageRenderer::imageGeode(int flip, float sizemul, float zoomin) {
	
	int i;
	
	double xstep = 0.0005;
	
	float zpos = 0.02;
	double ylim = 0.025, xlim = 0.025;
	double imagex, imagey;
	
	double imagesurf;
	
	ylim *=sizemul;
	xlim *=sizemul;
	
	int width, height;
	IplImage *thumbnail;
	std::string thumbnail_filename;
	
	StateSet *state;
	
	Vec3Array* vertices;
	DrawElementsUInt* line_p;
	Vec2Array* texcoord;
	
	Geometry *image_geometry;
	Geometry *border_geometry;
	Texture2D *image_texture;
	
	image_transform = new MatrixTransform();
	
	image_geode = new Geode();
	image_geometry = new Geometry();	
	
	// XS TODO check this wild cast
	// SD REQUIRED FOR THUMBNAIL ACImage* loop = dynamic_cast<ACImage*> ( browser.getLibrary()->getMedia(loop_index) );
	
	width = media_cycle->getWidth(loop_index);
	height = media_cycle->getHeight(loop_index);
	
	zpos = zpos + 0.00001 * loop_index;
	
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
	
	// Texture Coordinates
	texcoord = new Vec2Array;
	
	float a = (1.0-(1.0/zoomin)) / 2.0;
	float b = 1.0-a;
	texcoord->push_back(osg::Vec2(a, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? a : b));
	texcoord->push_back(osg::Vec2(a, flip ? a : b));
	
	image_geometry->setTexCoordArray(0, texcoord);	
	
	// Texture State (image)
	// image_image = osgDB::readImageFile("/dupont/databases/Bark.0000.jpg");
	// Use pre-computed thumbnail instead
	// SD REQUIRED
	
	// XS Debug
	thumbnail = (IplImage*)media_cycle->getThumbnailPtr(loop_index);
	if (thumbnail) {
		image_image = Convert_OpenCV_TO_OSG_IMAGE(thumbnail);
	}
	else {
		thumbnail_filename = media_cycle->getThumbnailFileName(loop_index);
		image_image = osgDB::readImageFile(thumbnail_filename);
	}
	
	image_texture = new Texture2D;
	image_texture->setImage(image_image);
	//image_texture->setUnRefImageDataAfterApply(true);
	state = image_geometry->getOrCreateStateSet();
	state->setTextureAttribute(0, image_texture);
	state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
	
	Vec4 color(1.0f, 1.0f, 1.0f, 0.9f);	
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);		
	image_geometry->setColorArray(colors);
	image_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	image_geode->addDrawable(image_geometry);
	
	image_transform->addChild(image_geode);
	
#ifdef IMAGE_BORDER
	
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
	
	Vec4 color2(0.4f, 0.4f, 0.4f, 1.0f);	
	Vec4Array* colors2 = new Vec4Array;
	colors2->push_back(color2);		
	border_geometry->setColorArray(colors2);
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
	image_transform->setUserData(new ACRefId(loop_index));
	//curser_transform->setName(name);
	image_transform->ref();
	//sprintf(name, "some audio element");
	image_geode->setUserData(new ACRefId(loop_index));
	//waveform_geode->setName(name);
	image_geode->ref();	
	border_geode->ref();	
}

void ACOsgImageRenderer::prepareNodes() {
	
	if (!image_geode) {
		imageGeode();
	}
	
	media_node->addChild(image_transform);
}


void ACOsgImageRenderer::updateNodes(double ratio) {
	
	float x, y, z;
	float zpos = 0.001;
	
#define NCOLORS 5
	static Vec4 colors[NCOLORS];
	static bool colors_ready = false;
	
	static Vec4 color2(0.2f, 0.8f, 0.2f, 1.0f);	
	static Vec4Array* colors2;
	
	static Vec4 color3(0.4f, 0.4f, 0.4f, 1.0f);	
	static Vec4Array* colors3;
	
	if(!colors_ready)
	{
		colors[0] = Vec4(0.2,0.6,0.2,1);
		colors[1] = Vec4(0.4,0.4,0.4,1);
		colors[2] = Vec4(0.5,1,1,1);
		colors[3] = Vec4(1,0.5,0.5,1);
		colors[4] = Vec4(0.5,1,0.5,1);
		colors2 = new Vec4Array;
		colors2->push_back(color2);	
		colors3 = new Vec4Array;
		colors3->push_back(color3);	
	}
	
	const ACLoopAttribute &attribute = media_cycle->getLoopAttributes(loop_index);
	const ACPoint &p = attribute.currentPos, &p2 = attribute.nextPos;
	double omr = 1.0-ratio;
	
	float zoom = media_cycle->getCameraZoom();
	float angle = media_cycle->getCameraRotation();
	
	Matrix T;
	Matrix Trotate;
	Matrix imageT;
	
	unsigned int mask = (unsigned int)-1;
	if(attribute.navigationLevel >= media_cycle->getNavigationLevel()) {
		image_transform->setNodeMask(mask);
	}
	else {
		image_transform->setNodeMask(0);
	}
	
	z = 0;
	
	if (border_geode->getDrawable(0)) {
		if (attribute.active) {
			((Geometry*)border_geode->getDrawable(0))->setColorArray(colors2);
		}
		else {
			((Geometry*)border_geode->getDrawable(0))->setColorArray(colors3);
		}
	}
	
	float localscale;
	float maxdistance = 0.2;
	float maxscale = 1;
	float minscale = 0.3;				
	// Apply "rotation" to compensate camera rotation
	x = omr*p.x + ratio*p2.x;
	y = omr*p.y + ratio*p2.y;
	localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
	localscale = max(localscale,minscale);
	if (localscale>minscale) {
		z += 2*zpos;
	}
	else if (attribute.active) {
		z += zpos;
	}

	T.makeTranslate(Vec3(x, y, z)); // omr*p.z + ratio*p2.z));	
	T =  Matrix::rotate(-angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/zoom,localscale/zoom,localscale/zoom) * T;
	media_node->setMatrix(T);
	
}
