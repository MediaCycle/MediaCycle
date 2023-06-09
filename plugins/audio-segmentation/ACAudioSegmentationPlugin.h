/*
 *  ACAudioSegmentationPlugin.h
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 09/06/2009
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef _ACAUDIOSEGMENTATIONPLUGIN_H
#define	_ACAUDIOSEGMENTATIONPLUGIN_H

#include <MediaCycle.h>

#include <iostream>

class ACAudioSegmentationPlugin : public ACSegmentationPlugin {
public:
    ACAudioSegmentationPlugin();
    ~ACAudioSegmentationPlugin();

    virtual std::vector<ACMedia*> segment(ACMedia*);
    std::vector<ACMedia*> segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia);

private:
    std::vector<ACMedia*> _segment(ACMedia* _theMedia);
    std::vector<ACMedia*> _segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia);
    int method;
    std::vector<std::string> methods;
};

arma::icolvec FastBIC(arma::fmat audiofeatures_m, float lambda, int samplerate);

#endif	/* _ACAUDIOSEGMENTATIONPLUGIN_H */
