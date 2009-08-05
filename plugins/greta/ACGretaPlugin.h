/* 
 * File:   ACGretaPlugin.h
 * Author: Alexis Moinet
 *
 * @date 03/08/20
 * @copyright (c) 2020 – UMONS - Numediart
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

#ifndef _ACGRETAPLUGIN_H
#define	_ACGRETAPLUGIN_H

#include <Communicator.h>
#include <Component.h>
#include <CppAIRPlug.h>

#include "ACPlugin.h"

class ACGretaPlugin : public ACPlugin {
public:
    ACGretaPlugin();
    ~ACGretaPlugin();
    virtual int initialize();
    virtual ACMediaFeatures* calculate();
    virtual ACMediaFeatures* calculate(std::string aFileName);
    virtual int start() {return 0;};
    virtual int stop() {return 0;};
    static std::string generateBML(std::string basename);
protected:
    
};

using namespace cmlabs;

class CppPoster : public CppAIRPlug
{
public:
	CppPoster(JString name, JString host, int port);
    virtual ~CppPoster();
};

#endif	/* _ACGRETAPLUGIN_H */

