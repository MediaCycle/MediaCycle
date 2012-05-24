/*
 *  ACOsgLibraryRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 04/05/2012
 *
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ACOsgLibraryRenderer.h"
#include <osg/Version>
using namespace osg;

//#define IMAGE_BORDER

ACOsgLibraryRenderer::ACOsgLibraryRenderer()
    :ACOsgMediaRenderer()
{
    /*max_side_size = 64; //pixels
    font_size = 16; // pixels
    line_sep = font_size; //pixels*/

    max_side_size = 128; //pixels
    font_size = 32; // pixels
    line_sep = font_size; //pixels

    node_color = Vec4(0.4f, 0.4f, 0.4f, 1.0f);
    image_image = 0;
    image_geode = 0;
    border_geode = 0;
    image_transform = 0;
    image_texture = 0;
    thumbnail = 0;

    title_text = 0;
    author_text = 0;
    curator_text = 0;
    year_text = 0;
    publisher_text = 0;
    license_text = 0;
    website_text = 0;
    medias_text = 0;

    title_geode = 0;
    author_geode = 0;
    curator_geode = 0;
    year_geode = 0;
    publisher_geode = 0;
    license_geode = 0;
    website_geode = 0;
    medias_geode = 0;

    title="";
    author="";
    curator="";
    year="";
    publisher="";
    license="";
    website="";
    cover="";
    medias=0;

    title_x = 0.0f;
    title_y = 0.0f;
    author_x = 0.0f;
    author_y = 0.0f;
    curator_x = 0.0f;
    curator_y = 0.0f;
    year_x = 0.0f;
    year_y = 0.0f;
    publisher_x = 0.0f;
    publisher_y = 0.0f;
    license_x = 0.0f;
    license_y = 0.0f;
    website_x = 0.0f;
    website_y = 0.0f;
    cover_x = 0.0f;
    cover_y = 0.0f;
    medias_x = 0.0f;
    medias_y = 0.0f;

    title_caption = "Title: ";
    author_caption = "Author: ";
    curator_caption = "Curator: ";
    year_caption = "Year: ";
    publisher_caption = "Publisher: ";
    license_caption = "License: ";
    website_caption = "Website: ";
    medias_caption = "Medias: ";

    /*
    font = 0;
std::string font_path(""),font_file("fudd.ttf");
#ifdef USE_DEBUG
    boost::filesystem::path s_path( __FILE__ );
    font_path = s_path.parent_path().parent_path().parent_path().string() + "/data/fonts/";
#else
#ifdef __APPLE__
    font_path = "@executable_path/../MacOS/fonts/";
#elif __WIN32__
    font_path = "./";
#else
    font_path = "/usr/share/mediacycle/fonts/";
#endif
#endif
    std::cout << "Current font path " << font_path << std::endl;

    font = osgText::readRefFontFile(font_path + font_file);
    if(!font)
        std::cerr << "ACOsgLibraryRenderer::ACOsgLibraryRenderer: couldn't load font " << std::endl;
    */
}

ACOsgLibraryRenderer::~ACOsgLibraryRenderer() {
    image_image = 0;
    image_geode = 0;
    border_geode = 0;
    image_transform = 0;
    image_texture = 0;
    thumbnail = 0;
    font = 0;

    title_text = 0;
    author_text = 0;
    curator_text = 0;
    year_text = 0;
    publisher_text = 0;
    license_text = 0;
    website_text = 0;
    medias_text = 0;

    title_geode = 0;
    author_geode = 0;
    curator_geode = 0;
    year_geode = 0;
    publisher_geode = 0;
    license_geode = 0;
    website_geode = 0;
    medias_geode = 0;
}

void ACOsgLibraryRenderer::textGeode(std::string _string, osg::ref_ptr<osgText::Text>& _text, osg::ref_ptr<osg::Geode>& _geode,osg::Vec3 pos)
{
    _geode = 0;
    Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
    _geode = new Geode();
    _text = new osgText::Text;
    _text->setColor(textColor);
    //_text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    _text->setCharacterSize(font_size);
    //_text->setPosition(osg::Vec3(50,25,0.0));
    _text->setPosition(pos);
    _text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    //#if OSG_MIN_VERSION_REQUIRED(3,0,0)
    if(font)
        _text->setFont(font);
    _text->setFontResolution(font_size,font_size);
    //#else
    //    _text->setFontResolution(12,12);
    //#endif
    _text->setAlignment( osgText::Text::LEFT_BASE_LINE );
    _text->setAxisAlignment( osgText::Text::SCREEN );

    osgText::String osg_string = osgText::String(_string,osgText::String::ENCODING_SIGNATURE);
    std::string utf8_string = osg_string.createUTF8EncodedString();

    _text->setText( utf8_string,osgText::String::ENCODING_UTF8 );
    _geode->addDrawable(_text);
    //_geode->setCullingActive(false);
}

void ACOsgLibraryRenderer::imageGeode(bool flip, float sizemul, float zoomin) {
    image_transform = 0;
    image_geode = 0;
    image_texture = 0;
    thumbnail = 0;

    if(cover!=""){
        std::cout << "ACOsgLibraryRenderer::imageGeode " << cover << std::endl;
        int i;
        double xstep = 0.0005;
        float zpos = 0;
        double imagex, imagey;
        int width, height;
        float scale;
        std::string thumbnail_filetitle;
        StateSet *state;
        Vec3Array* vertices;
        osg::ref_ptr<DrawElementsUInt> line_p;
        Vec2Array* texcoord;
        osg::ref_ptr<Geometry> image_geometry;
        osg::ref_ptr<Geometry> border_geometry;

        // Texture State (image)

        osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(cover).substr(1));

        if (!readerWriter){
            cerr << "<ACOsgLibraryRenderer::imageGeode: problem loading file, no OSG plugin available" << endl;
            return;
        }
        else{
            cout <<"<ACOsgLibraryRenderer::imageGeode: using OSG plugin: "<< readerWriter->className() <<std::endl;
        }

        thumbnail = osgDB::readImageFile(cover);
        readerWriter = 0;

        if (!thumbnail){
            cerr << "<ACImage::computeThumbnail> problem converting thumbnail to osg" << endl;
        }
        else{
            image_texture = new osg::Texture2D;
            image_texture->setImage(thumbnail);
        }
        image_texture->setResizeNonPowerOfTwoHint(false);

        width = thumbnail->s();
        height = thumbnail->t();
        image_transform = new MatrixTransform();
        image_geode = new Geode();
        image_geometry = new Geometry();

        // image vertices
        if(width>height){
            scale = max_side_size / (float)width;
        }
        else
            scale = max_side_size / (float)height;
        imagex = scale * 0.5 * width;
        imagey = scale * 0.5 * height;
        vertices = new Vec3Array(4);
        (*vertices)[0] = Vec3(-imagex, -imagey, zpos);
        (*vertices)[1] = Vec3(imagex, -imagey, zpos);
        (*vertices)[2] = Vec3(imagex, imagey, zpos);
        (*vertices)[3] = Vec3(-imagex, imagey, zpos);
        image_geometry->setVertexArray(vertices);

        // Primitive Set
        osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
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
        float a = (1.0-(1.0/zoomin)) / 2.0;
        float b = 1.0-a;
        texcoord->push_back(osg::Vec2(a, a));
        texcoord->push_back(osg::Vec2(b, a));
        texcoord->push_back(osg::Vec2(b, b));
        texcoord->push_back(osg::Vec2(a, b));
        image_geometry->setTexCoordArray(0, texcoord);

        state = image_geometry->getOrCreateStateSet();
        state->setTextureAttribute(0, image_texture);
        state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
        osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
        (*colors)[0] = node_color;
        //image_geometry->setColorArray(colors);
        //image_geometry->setColorBinding(Geometry::BIND_OVERALL);
        image_geode->addDrawable(image_geometry);
        image_transform->addChild(image_geode);

#ifdef IMAGE_BORDER
        //border by box, more smooth
        TessellationHints *hints = new TessellationHints();
        hints->setDetailRatio(0.0);
        border_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.0001),2*(imagex+5*xstep),2*(imagey+5*xstep),0), hints));

        state = border_geode->getOrCreateStateSet();
#if defined(APPLE_IOS)
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
#else
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
#endif
        image_transform->addChild(border_geode);

#endif
        //image_transform->setUserData(new ACRefId(node_index));
        //image_geode->setUserData(new ACRefId(node_index));
    }
}

void ACOsgLibraryRenderer::prepareNodes() {

    /*if(cover!="" && !image_transform){
        imageGeode();
        media_node->addChild(image_transform);
    }
    if(text_string!="" && !text_geode){
        textGeode();
        media_node->addChild(text_geode);
    }*/

}

void ACOsgLibraryRenderer::setTitle(std::string _title)
{
    if(_title=="")
        return;
    if(this->title!=_title){
        this->title=_title;
        if(title_geode)
            media_node->removeChild(title_geode);
        textGeode(title_caption+title,title_text,title_geode,osg::Vec3(title_x,title_y,0.0));
        media_node->addChild(title_geode);
    }
}

void ACOsgLibraryRenderer::setAuthor(std::string _author)
{
    if(_author=="")
        return;
    if(this->author!=_author){
        this->author=_author;
        if(author_geode)
            media_node->removeChild(author_geode);
        textGeode(author_caption+author,author_text,author_geode,osg::Vec3(author_x,author_y,0.0));
        media_node->addChild(author_geode);
    }
}

void ACOsgLibraryRenderer::setPublisher(std::string _publisher)
{
    if(_publisher=="")
        return;
    if(this->publisher!=_publisher){
        this->publisher=_publisher;
        if(publisher_geode)
            media_node->removeChild(publisher_geode);
        textGeode(publisher_caption+publisher,publisher_text,publisher_geode,osg::Vec3(publisher_x,publisher_y,0.0));
        media_node->addChild(publisher_geode);
    }
}

/*void ACOsgLibraryRenderer::setCurator(std::string _curator)
{
    if(_curator=="")
        return;
    if(this->curator!=_curator){
        this->curator=_curator;
        if(curator_geode)
            media_node->removeChild(curator_geode);
        textGeode(curator_caption+curator,curator_text,curator_geode,osg::Vec3(curator_x,curator_y,0.0));
        media_node->addChild(curator_geode);
    }
}*/

void ACOsgLibraryRenderer::setCover(std::string _cover)
{
    if(_cover=="")
        return;
    if(this->cover!=_cover){
        this->cover=_cover;
        if(image_transform)
            media_node->removeChild(image_transform);
        imageGeode();
        media_node->addChild(image_transform);
    }
}

void ACOsgLibraryRenderer::setNumberOfMedia(int _number)
{
    if(_number==0)
        return;
    if(this->medias!=_number){
        this->medias=_number;
        if(medias_geode)
            media_node->removeChild(medias_geode);
        std:stringstream medias_number;
        medias_number << medias_caption << medias;
        textGeode(medias_number.str(),medias_text,medias_geode,osg::Vec3(medias_x,medias_y,0.0));
        media_node->addChild(medias_geode);
    }
}

void ACOsgLibraryRenderer::updateNodes(double ratio) {
    if(media_cycle && media_cycle->getLibrary()){
        this->setCover(media_cycle->getLibrary()->getCover());

        if(this->cover!=""){
            title_x = author_x = curator_x = year_x = publisher_x = license_x = website_x = medias_x = max_side_size/2;
        }

        medias_y = 0*line_sep-max_side_size/2;
        //curator_y = 1*line_sep-max_side_size/2;
        publisher_y = 1*line_sep-max_side_size/2;
        author_y = 2*line_sep-max_side_size/2;
        title_y = 3*line_sep-max_side_size/2;

        this->setTitle(media_cycle->getLibrary()->getTitle());
        this->setAuthor(media_cycle->getLibrary()->getAuthor());
        this->setCurator(media_cycle->getLibrary()->getCuratorName());
        this->setYear(media_cycle->getLibrary()->getYear());
        this->setPublisher(media_cycle->getLibrary()->getPublisher());
        this->setLicense(media_cycle->getLibrary()->getLicense());
        this->setWebsite(media_cycle->getLibrary()->getWebsite());
        this->setNumberOfMedia(media_cycle->getLibrarySize());
    }

    float x,y,z;
    x = max_side_size/2;
    y = max_side_size/2+line_sep;
    z = 0;

#ifdef AUTO_TRANSFORM
    media_node->setPosition(Vec3(x,y,z));
    //media_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
    //media_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else

    Matrix T;
    T.makeTranslate(Vec3(x, y, z)); // omr*p.z + ratio*p2.z));
    /*T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0))
   * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom)
                        * T;*/
    media_node->setMatrix(T);
#endif //AUTO_TRANSFORM
}

void ACOsgLibraryRenderer:: updateSize(int w, int h){


}
