/*
 *  ACMediaFactory.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
 *  Updated by Christian Frisson since then
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

#ifndef _ACMEDIAFACTORY_H
#define _ACMEDIAFACTORY_H

#include "ACMedia.h"
#include "ACMediaTypes.h"
//#include "ACPluginManager.h"

#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

class ACPluginLibrary;
class ACPluginManager;

typedef  std::map<std::string, ACMediaType> ACMediaFileExtensions; // extension, media type
typedef  std::pair<std::string, ACMediaType> ACMediaReaderType;
typedef  std::map<std::string, ACMediaReaderType> ACMediaReaderExtensions; // extensions , reader name

// Possibly thread-safe boost-powered singleton inspired from:
// http://www.boostcookbook.com/Recipe:/1235044

class ACMediaFactory : private boost::noncopyable {
protected:
    // for log(n) search through extensions:
    std::list<std::string> checked_readers,checked_renderers;
    ACMediaReaderExtensions readable_extensions,renderable_extensions;

public:
    static ACMediaFactory & getInstance(){
        boost::call_once(call_once,once_flag);
        return get_instance();
    }

protected:
    static boost::once_flag once_flag;
    static ACMediaFactory & get_instance(){
        static ACMediaFactory instance;
        return instance;
    }

    static void call_once(){
        get_instance();
    }

protected:
    ///
    /// The constructor isn't public to ensure we keep one instance only of this class (singleton)
    /// that shouldn't member of other classes, instead:
    /// - call ACMediaFactory::getInstance(); to initialize available extensions at a strategic point
    ///  (application or media library init),
    ///  or these will be initialized at the first public member query.
    /// - access ACMediaFactory each public method method() thru ACMediaFactory::getInstance()->method()
    ///
    ACMediaFactory();
    virtual ~ACMediaFactory();
    //static ACMediaFactory* instance;

public:
    // 3 ways to specify which new media to create:
    // 1) give file extension (and optional media type if ambiguous)
    virtual ACMedia* create(std::string file_ext, ACMediaType media_type = MEDIA_TYPE_NONE);
    // 2) directly specify which media (e.g. for openLibrary)
    virtual ACMedia* create(ACMediaType media_type);
    // 3) copy a media
    virtual ACMedia* create(ACMedia* media);
private:
    ACMedia* create(ACMediaType media_type,ACMedia* media);

public:
    ACMediaType getMediaTypeFromExtension(std::string file_ext);
    ACMediaType guessMediaTypeFromString(std::string keyword);
    std::string getNormalCaseStringFromMediaType(ACMediaType media_type);
    std::string getLowCaseStringFromMediaType(ACMediaType media_type);
    std::vector< std::string > listAvailableMediaTypes();
    bool isMediaTypeSegmentable(ACMediaType _media_type);
    std::vector<std::string> getExtensionsFromMediaType(ACMediaType media_type);
    void addMediaReaders(ACPluginLibrary *acpl);
    void removeMediaReaders(ACPluginLibrary *acpl);
    void setPluginManager(ACPluginManager* _plugin_manager){this->plugin_manager = _plugin_manager;}
    void useRendering(bool _rendering){this->use_rendering = _rendering;}

private:
    ACPluginManager* plugin_manager;
    bool use_rendering;
};

#endif // _ACMEDIAFACTORY_H
