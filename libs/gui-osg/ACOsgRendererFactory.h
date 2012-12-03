/*
 *  ACOsgRendererFactory.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 2/11/2012
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

#ifndef _ACOsgRendererFactory_H
#define _ACOsgRendererFactory_H

#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

#include "ACEventListener.h"

#include "ACOsgMediaRenderer.h"
#include "ACOsgTrackRenderer.h"
#include "ACOsgBrowserRenderer.h"
#include "ACOsgTimelineRenderer.h"

#include "ACMediaFactory.h"

// Possibly thread-safe boost-powered singleton inspired from:
// http://www.boostcookbook.com/Recipe:/1235044

typedef std::map<std::string, ACMediaType> ACOsgRendererTypes;

class ACOsgRendererFactory: public ACEventListener, private boost::noncopyable {
protected:
    std::list<std::string> checked_renderers;
    ACMediaReaderExtensions renderable_extensions;
    ACOsgRendererTypes renderer_types;
public:
    static ACOsgRendererFactory & getInstance(){
        boost::call_once(call_once,once_flag);
        return get_instance();
    }

protected:
    static boost::once_flag once_flag;
    static ACOsgRendererFactory & get_instance(){
        static ACOsgRendererFactory instance;
        return instance;
    }

    static void call_once(){
        get_instance();
    }

protected:
    ///
    /// The constructor isn't public to ensure we keep one instance only of this class (singleton)
    /// that shouldn't member of other classes, instead:
    /// - call ACOsgRendererFactory::getInstance(); to initialize available extensions at a strategic point
    ///  (application or media library init),
    ///  or these will be initialized at the first public member query.
    /// - access ACOsgRendererFactory each public method method() thru ACOsgRendererFactory::getInstance()->method()
    ///
    ACOsgRendererFactory();
    virtual ~ACOsgRendererFactory();
    //static ACOsgRendererFactory* instance;
    MediaCycle* media_cycle;
    ACOsgBrowserRenderer* browser;
    ACOsgTimelineRenderer* timeline;

public:
    void setMediaCycle(MediaCycle* _media_cycle);
    void setBrowserRenderer(ACOsgBrowserRenderer* _browser);
    void setTimelineRenderer(ACOsgTimelineRenderer* _timeline);
    //Callbacks
    void pluginLoaded(std::string name);

    ACOsgMediaRenderer* createMediaRenderer(ACMediaType media_type);
    ACOsgTrackRenderer* createTrackRenderer(ACMediaType media_type);
};

#endif // _ACOsgRendererFactory_H
