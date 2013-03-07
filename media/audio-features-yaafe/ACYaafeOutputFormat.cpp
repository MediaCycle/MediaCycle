/**
 * @brief Class to use the output format facility of Yaafe to get features in a MediaCycle plugin
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

//#define SAVE_CSV

#include "ACYaafeOutputFormat.h"
#include "ACAudio.h"
#include <vector>
#include <string>

using namespace YAAFE;
using namespace std;

#include <yaafe-io/io/FileUtils.h>
#include <sstream>

#include "yaafe-core/ComponentFactory.h"

#include "yaafe-io/io/CSVWriter.h"
//#ifdef WITH_SNDFILE
#include "yaafe-io/io/AudioFileReader.h"
//#endif
//#ifdef WITH_MPG123
#include "yaafe-io/io/MP3FileReader.h"
//#endif
#ifdef WITH_HDF5
#include "yaafe-io/io/H5DatasetWriter.h"
#endif

#define BUFSIZED 64

#include "ACYaafeWriter.h"

using namespace std;

namespace YAAFE {

bool ACYaafeOutputFormat::available() const {
    return ComponentFactory::instance()->exists("ACYaafeWriter");
}

const ParameterDescriptorList ACYaafeOutputFormat::getParameters() const {
    const Component* ACw = ComponentFactory::instance()->getPrototype("ACYaafeWriter");
    if (ACw==NULL) {
        cerr << "WARNING: ACYaafeWriter not available ! cannot retrieve parameter list" << endl;
        return ParameterDescriptorList();
    }
    ParameterDescriptorList pList = ACw->getParameterDescriptorList();
    eraseParameterDescriptor(pList,"File");
    eraseParameterDescriptor(pList,"Attrs");
    return pList;
}

Component* ACYaafeOutputFormat::createWriter(
        const std::string& inputfile,
        const std::string& feature,
        const ParameterMap& featureParams,
        const Ports<StreamInfo>& featureStream)
{
    Component* writer = ComponentFactory::instance()->createComponent("ACYaafeWriter");
    ParameterMap writerParams = m_params;
    writerParams["Feature"] = feature;
    writerParams["File"] = filenameConcat(m_outDir,inputfile,"." + feature + ".csv");
    writerParams["Attrs"] = encodeParameterMap(featureParams);
    if (!writer->init(writerParams,featureStream)) {
        delete writer;
        cerr << "ERROR: cannot initialize ACYaafeWriter !" << endl;
        return NULL;
    }

    ACYaafeWriter* _writer = 0;
    //try {
    _writer = static_cast <ACYaafeWriter*> (writer);
    if(!_writer)
        //      throw runtime_error("<ACYaafeOutputFormat::createWriter> problem with ACYaafeWriter cast");
        //}
        //catch (const exception& e)
    {
        //        cerr << e.what() << endl;
        cerr <<"<ACYaafeOutputFormat::createWriter> problem with ACYaafeWriter cast"<< endl;
    }
    if(_writer){
        _writer->setMediaCyclePlugin(m_plugin);
    }

    return writer;
}

}
