/**
 * @brief Class to write/process of Yaafe features into a MediaCycle plugin
 * @author Christian Frisson
 * @date 31/03/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#ifndef ACYaafeWriter_H
#define	ACYaafeWriter_H

#include "MediaCycle.h"

#include "ACAudioYaafeCorePlugin.h"

#include<iostream>

#include <algorithm>
#include <iostream>
#include <string>
#include "argtable2.h"
/*#include <yaafe-core/DataFlow.h>
#include <yaafe-core/AudioFileProcessor.h>
#include <yaafe-core/OutputFormat.h>
#include <yaafe-core/Engine.h>
#include <yaafe-core/ComponentFactory.h>*/

#define ACYAAFEWRITER_ID "ACYaafeWriter"

namespace YAAFE{

class ACYaafeWriter: public ComponentBase<ACYaafeWriter> {
public:
    ACYaafeWriter();
    virtual ~ACYaafeWriter();

    const std::string getIdentifier() const {
        return ACYAAFEWRITER_ID;
    }

    virtual bool stateLess() const { return false; }

    virtual ParameterDescriptorList getParameterDescriptorList() const;

    virtual bool init(const ParameterMap& params, const Ports<StreamInfo>& in);
    virtual void reset();
    virtual bool process(Ports<InputBuffer*>& in, Ports<OutputBuffer*>& out);
    virtual void flush(Ports<InputBuffer*>& in, Ports<OutputBuffer*>& out);
    void setMediaCyclePlugin(ACAudioYaafeCorePlugin* plugin){m_plugin = plugin;}

private:
    std::ofstream m_fout;
    std::string m_feature_name;
    int m_precision;
    std::string m_file;
    ACAudioYaafeCorePlugin* m_plugin;
};

}

#endif	/* ACYaafeWriter_H */
