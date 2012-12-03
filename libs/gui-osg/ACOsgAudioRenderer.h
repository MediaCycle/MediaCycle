/*
 *  ACOsgBrowserRenderer.h
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

#ifndef __ACOSG_AUDIO_RENDERER_H__
#define __ACOSG_AUDIO_RENDERER_H__

#if defined (SUPPORT_AUDIO)

#include "ACOsgMediaRenderer.h"

class ACOsgAudioRenderer : public ACOsgMediaRenderer {

protected:
    osg::ref_ptr<osg::Geode> waveform_geode;
    osg::ref_ptr<osg::Geode> curser_geode;

#ifdef AUTO_TRANSFORM
    osg::ref_ptr<osg::AutoTransform> curser_transform;
#else
    osg::ref_ptr<osg::MatrixTransform> curser_transform;
#endif
    osg::ref_ptr<osg::Geode> entry_geode;
    osg::ref_ptr<osg::Geode> aura_geode;

    void waveformGeode();
    void curserGeode();
    void entryGeode();
    void auraGeode();


    ACBrowserAudioWaveformType waveform_type;

public:
    ACOsgAudioRenderer();
    ~ACOsgAudioRenderer();
    void prepareNodes();
    void updateNodes(double ratio=0.0);
    ACBrowserAudioWaveformType getWaveformType(){return waveform_type;}
    void setWaveformType(ACBrowserAudioWaveformType _type);
    void updateWaveformType(ACBrowserAudioWaveformType _type);
    virtual void changeSetting(ACSettingType _setting);
};

#endif //defined (SUPPORT_AUDIO)
#endif
