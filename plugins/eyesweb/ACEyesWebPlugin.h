/* 
 * File:   ACEyesWebPlugin.h
 * Author: Julien Dubois
 *
 * @date 29 juillet 2009
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

#ifndef _ACEYESWEBPLUGIN_H
#define	_ACEYESWEBPLUGIN_H

#ifdef __WIN32__
#define FILE_SEP = "\\"
#else
#define FILE_SEP = "/"
#endif

#include <armadillo>

#include "ACPlugin.h"
#include "ACMediaTimedFeature.h"
#include "ACMediaData.h"

class ACEyesWebPlugin : public ACFeaturesPlugin {
public:
    ACEyesWebPlugin();
    ~ACEyesWebPlugin();
		
//	virtual std::vector<ACMediaFeatures*> calculate(std::string aFileName, bool _save_timed_feat=false);
	virtual std::vector<ACMediaFeatures*> calculate(ACMediaData* _data, ACMedia* theMedia, bool _save_timed_feat=false);
    std::string extractDirectory(std::string path);
    std::string extractFilename(std::string path);
    std::string changeExtension(std::string path, std::string ext);
    std::string changeLastFolder(std::string path, std::string folder);
protected:
};

#endif	/* _ACEYESWEBPLUGIN_H */
