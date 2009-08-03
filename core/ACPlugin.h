/* 
 * File:   ACPlugin.h
 * Author: Julien Dubois
 *
 * @date 27 juillet 2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#ifndef _ACPLUGIN_H
#define	_ACPLUGIN_H

#include <iostream>
#include <string>
#include <vector>

#include "ACMediaTypes.h"
#include "ACMediaFeatures.h"
#include "ACMediaTimedFeature.h"

using namespace std;

enum ACPluginType {
	PLUGIN_TYPE_NONE,
	PLUGIN_TYPE_FEATURES,
	PLUGIN_TYPE_SERVER
};

class ACPlugin {
public:
    ACPlugin() {};
    virtual ~ACPlugin() {};
    virtual std::string getName() {return this->mName;};
    //virtual std::string getName() const = 0; -> error !
    virtual std::string getIdentifier() {return this->mId;};
    virtual std::string getDescription() {return this->mDescription;};
    virtual ACMediaType getMediaType() {return this->mMediaType;};
    virtual ACPluginType getPluginType() {return this->mPluginType;};
    virtual int initialize() = 0;
    virtual ACMediaFeatures *calculate() = 0;
    virtual ACMediaFeatures *calculate(std::string aFileName) = 0;
    virtual int start() = 0;
    virtual int stop() = 0;

    //virtual int readFile(std::string);
protected:
    string mName;
    string mId;
    string mDescription;
    ACMediaType mMediaType;
    ACPluginType mPluginType;
};

// the types of the class factories
typedef ACPlugin* createPluginFactory(std::string);
typedef void destroyPluginFactory(ACPlugin*);
typedef vector<string> listPluginFactory();

#endif	/* _ACPLUGIN_H */

