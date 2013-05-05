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

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

ACOsgLibraryRenderer::ACOsgLibraryRenderer()
    :ACOsgMediaRenderer()
{
    width = 0;
    height = 0;
    progress = 1.0f;
    this->changeSetting(this->setting);
}

void ACOsgLibraryRenderer::init()
{
    node_color = Vec4(0.4f, 0.4f, 0.4f, 1.0f);

    library_title.caption = "Title: ";
    library_author.caption = "Author: ";
    library_year.caption = "Year: ";
    library_publisher.caption = "Publisher: ";
    library_license.caption = "License: ";
    library_website.caption = "Website: ";
    library_medias_number.caption = "Elements: "; //"Medias: ";

    curator_name.caption = "Curator: ";
    curator_email.caption = "Email: ";
    curator_website.caption = "Website: ";
    curator_location.caption = "From: ";

    library_node = new MatrixTransform();
    curator_node = new MatrixTransform();
    media_node->addChild(library_node);
    media_node->addChild(curator_node);
}

void ACOsgLibraryRenderer::changeSetting(ACSettingType _setting)
{
    // Init setting
    if(_setting == AC_SETTING_INSTALLATION){
        max_side_size = 128; //pixels
        font_size = 32; // pixels
        line_sep = font_size; //pixels
    }
    else{
        max_side_size = 64; //pixels
        font_size = 16; // pixels
        line_sep = font_size; //pixels
    }
    this->init();

    // Change setting
    if(this->setting == _setting)
        return;
    this->setting = _setting;

    //this->updateNodes();
}

ACOsgLibraryRenderer::~ACOsgLibraryRenderer() {
    font = 0;
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

void ACOsgLibraryRenderer::imageGeode(ACOsgLibraryImageRenderer& _renderer) {
    _renderer.transform = 0;
    osg::ref_ptr<osg::Geode> image_geode = 0;
    osg::ref_ptr<osg::Texture2D> image_texture = 0;
    osg::ref_ptr<osg::Image> thumbnail = 0;

    if(_renderer.file!=""){
        std::cout << "ACOsgLibraryRenderer::imageGeode " << _renderer.file << std::endl;
        int i;
        float zpos = 0;
        double imagex, imagey;
        int width, height;
        float scale;
        StateSet *state;
        Vec3Array* vertices;
        osg::ref_ptr<DrawElementsUInt> line_p;
        Vec2Array* texcoord;
        osg::ref_ptr<Geometry> image_geometry;
        osg::ref_ptr<Geometry> border_geometry;

        // Texture State (image)

        osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(_renderer.file).substr(1));

        if (!readerWriter){
            cerr << "<ACOsgLibraryRenderer::imageGeode: problem loading file, no OSG plugin available" << endl;
            return;
        }
        else{
            cout <<"<ACOsgLibraryRenderer::imageGeode: using OSG plugin: "<< readerWriter->className() <<std::endl;
        }

        thumbnail = osgDB::readImageFile(_renderer.file);

        if (!thumbnail){
            cerr << "<ACOsgLibraryRenderer::imageGeode> image reader " << readerWriter->className() << " couldn't read image " << _renderer.file << endl;
            return;
        }
        else{
            image_texture = new osg::Texture2D;
            image_texture->setImage(thumbnail);
        }
        readerWriter = 0;

        image_texture->setResizeNonPowerOfTwoHint(false);

        width = thumbnail->s();
        height = thumbnail->t();
        _renderer.transform = new MatrixTransform();
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
        float a = 0.0;//(1.0-(1.0/zoomin)) / 2.0;
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
        _renderer.transform->addChild(image_geode);
        //image_transform->setUserData(new ACRefId(node_index));
        //image_geode->setUserData(new ACRefId(node_index));
    }
}

void ACOsgLibraryRenderer::prepareNodes() {
    //medias_caption = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(this->media_cycle->getMediaType()) + "s";
}

void ACOsgLibraryRenderer::updateTextRenderer(ACOsgLibraryTextRenderer& _renderer, std::string _value, osg::ref_ptr<osg::MatrixTransform> _node)
{
    if(_value=="")
        return;
    if(_renderer.value!=_value){
        _renderer.value=_value;
        if(_renderer.geode)
            _node->removeChild(_renderer.geode);
        textGeode(_renderer.caption+_renderer.value,_renderer.text,_renderer.geode,osg::Vec3(_renderer.x,_renderer.y,0.0));
        _node->addChild(_renderer.geode);
    }
}

void ACOsgLibraryRenderer::updateImageRenderer(ACOsgLibraryImageRenderer& _renderer, std::string _file, osg::ref_ptr<osg::MatrixTransform> _node)
{
    if(_file=="")
        return;
    if(_renderer.file!=_file){
        _renderer.file=_file;
        if(_renderer.transform)
            _node->removeChild(_renderer.transform);
        imageGeode(_renderer);
        _node->addChild(_renderer.transform);
    }
}

void ACOsgLibraryRenderer::updateNodes(double ratio) {
    if(media_cycle && media_cycle->getLibrary()){

        ///CF comment the following line to disable the realtime progress report
        progress = media_cycle->getLibrary()->getImportProgress();

        this->updateImageRenderer(library_cover,media_cycle->getLibrary()->getCover(),library_node);

        if(this->library_cover.file!=""){
            library_title.x = library_author.x = library_year.x = library_publisher.x = library_license.x = library_website.x = library_medias_number.x = max_side_size/2;
        }

        //CF be careful with positionning, overlapping text geodes explode CPU usage
        library_title.y = 4*line_sep-max_side_size/2;
        library_author.y = 3*line_sep-max_side_size/2;
        library_publisher.y = 2*line_sep-max_side_size/2;
        library_medias_number.y = 1*line_sep-max_side_size/2;

        this->updateTextRenderer(library_title,media_cycle->getLibrary()->getTitle(),library_node);
        this->updateTextRenderer(library_author,media_cycle->getLibrary()->getAuthor(),library_node);
        //this->updateTextRenderer(library_year,media_cycle->getLibrary()->getYear(),library_node);
        this->updateTextRenderer(library_publisher,media_cycle->getLibrary()->getPublisher(),library_node);
        //this->updateTextRenderer(library_license,media_cycle->getLibrary()->getLicense(),library_node);
        //this->updateTextRenderer(library_website,media_cycle->getLibrary()->getWebsite(),library_node);

std:stringstream library_medias_number_info;
        if(media_cycle->getBrowserMode() == AC_MODE_NEIGHBORS)
            library_medias_number_info << media_cycle->getNumberOfMediaNodes() << "/";
        library_medias_number_info << media_cycle->getLibrarySize();
        /*int files_to_import = media_cycle->getLibrary()->getNumberOfFilesToImport();
        if(files_to_import != 0){
            int files_processed = media_cycle->getLibrary()->getNumberOfFilesProcessed();
            library_medias_number_info << " (importing " << files_processed << "/" << files_to_import << ")";
        }*/
        if(progress<1.0f){
            int files_to_import = media_cycle->getLibrary()->getNumberOfFilesToImport();
            int files_processed = media_cycle->getLibrary()->getNumberOfFilesProcessed();
            library_medias_number_info << " (importing " << files_processed+1 << "/" << files_to_import << ": " << progress*100 << "%)";
        }

        this->updateTextRenderer(library_medias_number,library_medias_number_info.str(),library_node);
        // cout<<"test:"<<library_medias_number.text->getFont()->getFileName()<<"\t"<<library_medias_number.text->getText().createUTF8EncodedString ()<<endl;
        this->updateImageRenderer(curator_picture,media_cycle->getLibrary()->getCuratorPicture(),curator_node);

        if(this->library_cover.file!=""){
            curator_name.x = curator_email.x = curator_website.x = curator_location.x = max_side_size/2;
        }

        //CF be careful with positionning, overlapping text geodes explode CPU usage
        curator_name.y = 3*line_sep-max_side_size/2;
        curator_email.y = 2*line_sep-max_side_size/2;
        curator_website.y = 1*line_sep-max_side_size/2;
        curator_location.y = 0*line_sep-max_side_size/2;

        this->updateTextRenderer(curator_name,media_cycle->getLibrary()->getCuratorName(),curator_node);
        this->updateTextRenderer(curator_email,media_cycle->getLibrary()->getCuratorEmail(),curator_node);
        this->updateTextRenderer(curator_website,media_cycle->getLibrary()->getCuratorWebsite(),curator_node);
        this->updateTextRenderer(curator_location,media_cycle->getLibrary()->getCuratorLocation(),curator_node);
    }

    float library_x,library_y,library_z;
    library_x = max_side_size/2;
    library_y = max_side_size/2+line_sep;
    library_z = 0.0f;

    float curator_x,curator_y,curator_z;
    if(setting==AC_SETTING_INSTALLATION)
        curator_x = 2.0f*width/4.0f; //CF should check the max text length of curator/library lines instead
    else
        curator_x = 3.0f*width/4.0f;
    curator_y = max_side_size/2+line_sep;
    curator_z = 0.0f;

#ifdef AUTO_TRANSFORM
    library_node->setPosition(Vec3(library_x,library_y,library_z));
    curator_node->setPosition(Vec3(curator_x,curator_y,curator_z));
    //library_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
    //library_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else

    Matrix library_matrix,curator_matrix;
    library_matrix.makeTranslate(Vec3(library_x, library_y, library_z)); // omr*p.z + ratio*p2.z));
    /*T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0))
   * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom)
                        * T;*/
    library_node->setMatrix(library_matrix);
    curator_matrix.makeTranslate(Vec3(curator_x, curator_y, curator_z)); // omr*p.z + ratio*p2.z));
    curator_node->setMatrix(curator_matrix);

#endif //AUTO_TRANSFORM
}

void ACOsgLibraryRenderer:: updateSize(int w, int h){
    this->width = w;
    this->height = h;
}
