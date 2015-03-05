/**
 * @brief Main MediaCycle window wrapping an OpenSceneGraph view of a MediaCycle browser
 * @author Christian Frisson
 * @date 22/04/2014
 * @copyright (c) 2014 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#include "ACMultiMediaCycleOsgQt.h"
#include "ACOsgCompositeViewQt.h"

#ifdef OSG_LIBRARY_STATIC
#include <osgViewer/GraphicsWindow>
USE_GRAPHICSWINDOW()
#endif

#include <osgDB/Registry>

ACMultiMediaCycleOsgQt::ACMultiMediaCycleOsgQt(QWidget *parent)
    : ACMultiMediaCycleQt(new ACOsgCompositeViewQt())
{
    // Apple bundled *.app, just look for bundled osg plugins
#ifndef USE_DEBUG
#if defined __APPLE__ and not defined (XCODE)
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("PlugIns");
    std::cout << "OSG plugins path" << dir.absolutePath().toStdString() << std::endl;
    osgDB::Registry::instance()->setLibraryFilePathList(dir.absolutePath().toStdString());
#endif
#endif
}

#include <ACOsgCompositeViewQt.h>

ACMultiMediaCycleOsgQt::~ACMultiMediaCycleOsgQt(){

}

bool ACMultiMediaCycleOsgQt::loadDefaultConfig(ACAbstractDefaultConfig* _config)
{
    if(media_cycle->getCurrentConfig()){
        if(media_cycle->getCurrentConfig() != _config)
            this->clean();
    }

    // Check the config
    if(!(this->addDefaultConfig(_config)))
        return false;
    ACOsgAbstractDefaultConfig* _osg_config = dynamic_cast<ACOsgAbstractDefaultConfig*>(_config);

    // Check required OSG plugins:
    if(_osg_config){
        std::vector<std::string> osg_plugins = _osg_config->osgPlugins();
        for(std::vector<std::string>::iterator plugin = osg_plugins.begin();plugin != osg_plugins.end();++plugin){
            QString basename(plugin->c_str()),filename(plugin->c_str());
            basename = basename.remove("osgdb_");
#if defined (__MINGW32__) || defined (WIN32)
            filename.prepend("lib");
#endif
#ifdef OSG_LIBRARY_STATIC
            filename.append(".a");
#else
#if defined (__MINGW32__) || defined (WIN32)
            filename.append(".dll");
#else
            filename.append(".so");
#endif
#endif
            //TOOO load other osg plugins setLibraryFilePathList;
            // osgDB::appendPlatformSpecificLibraryFilePaths

            osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(basename.toStdString());
            if(!readerWriter){
                // Sometimes plugins are not named along file extensions, like for instance osgdb_imageio
                osgDB::Registry::LoadStatus status = osgDB::Registry::instance()->loadLibrary(filename.toStdString());
                if(status == osgDB::Registry::NOT_LOADED){
                    this->showError("OSG plugin '" + *plugin + "' is required but not accessible. Can't use config.");
                    return false;
                }
                else /*if(status == osgDB::Registry::LOADED)*/{ // last case could be osgDB::Registry::PREVIOUSLY_LOADED
                    osgDB::Registry::instance()->closeLibrary(filename.toStdString());
                }
            }
        }
    }

    return ACMultiMediaCycleQt::loadDefaultConfig(_config);
}

