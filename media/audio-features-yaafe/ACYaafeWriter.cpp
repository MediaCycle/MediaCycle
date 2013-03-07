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

//#define SAVE_CSV

#include "ACYaafeWriter.h"
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

using namespace std;

namespace YAAFE {

ACYaafeWriter::ACYaafeWriter() {
    m_feature_name = "";
    m_plugin = 0;
    m_file = "";
}

ACYaafeWriter::~ACYaafeWriter() {
    if (m_fout.is_open())
        m_fout.close();
    m_plugin = 0;
}

ParameterDescriptorList ACYaafeWriter::getParameterDescriptorList() const
{
    ParameterDescriptorList pList;
    ParameterDescriptor p;

    p.m_identifier = "File";
    p.m_description = "MC output filename";
    p.m_defaultValue = "";
    pList.push_back(p);

    p.m_identifier = "Attrs";
    p.m_description = "Metadata to be written to the output";
    p.m_defaultValue = "";
    pList.push_back(p);

    p.m_identifier = "Metadata";
    p.m_description = "If 'True' then write metadata as comments at the beginning of the MC file. If 'False', do not write metadata";
    p.m_defaultValue = "True";
    pList.push_back(p);

    p.m_identifier = "Precision";
    p.m_description = "precision of output floating point number.";
    p.m_defaultValue = "6";
    pList.push_back(p);

    return pList;
}

bool ACYaafeWriter::init(const ParameterMap& params, const Ports<StreamInfo>& inp)
{
    //assert(inp.size()==1);
    if(inp.size()!=1){
        std::cerr << "ACYaafeWriter::init: couldn't init" << std::endl;
        return false;
    }
    const StreamInfo& in = inp[0].data;

    m_file = getStringParam("File", params);
    m_feature_name = getStringParam("Feature", params);

    // Replace underscores by spaces, human-readable, conforming with ACAudioFeatures
    size_t underscore = 0;
    while(underscore != std::string::npos){
        underscore = m_feature_name.find_first_of("_",underscore);
        if(underscore != std::string::npos)
            m_feature_name = m_feature_name.replace(underscore,1," ");
    }

    //std::cout << "ACYaafeWriter::init " << m_feature_name << " on " << m_file << std::endl;
    m_precision = getIntParam("Precision",params);
    if (m_precision > (BUFSIZED-10)) {
        cerr << "WARNING: precision is too large ! use precision " << BUFSIZE - 10 << endl;
        m_precision = BUFSIZED - 10;
    }

#ifdef SAVE_CSV
    int res = preparedirs(m_file.c_str());
    if (res!=0)
        return false;

    m_fout.open(m_file.c_str(), ios_base::trunc);
    if (!m_fout.is_open() || m_fout.bad())
        return false;

    if (getStringParam("Metadata",params)=="True") {
        // write metadata at the beginnig of the file
        string paramStr = getStringParam("Attrs",params);
        map<string,string> _params = decodeAttributeStr(paramStr);
        ostringstream oss;
        for (map<string,string>::const_iterator it=_params.begin();it!=_params.end();it++){
            oss << "% " << it->first << "=" << it->second << endl;
        }
        m_fout.write(oss.str().c_str(),oss.str().size());
    }
#endif

    return true;
}

void ACYaafeWriter::reset() {
    // nothing to do
    //std::cout << "ACYaafeWriter::reset" << std::endl;
}

bool ACYaafeWriter::process(Ports<InputBuffer*>& inp, Ports<OutputBuffer*>& outp)
{
    //std::cout << "ACYaafeWriter::process " << m_feature_name << " on " << m_file << std::endl;

    float time = 0;
    std::vector<float> times;
    std::vector< std::vector<float> > values;
    ACMediaTimedFeature* feat = 0;

    if(m_plugin){
        if(m_plugin->isMediaTimedFeatureStored(m_feature_name,m_file)){
            time = m_plugin->getMediaTimedFeatureStored(m_feature_name,m_file)->getTime( m_plugin->getMediaTimedFeatureStored(m_feature_name,m_file)->getLength()-1);
        }
    }

    assert(inp.size()==1);
    InputBuffer* in = inp[0].data;
    assert(outp.size()==0);
    char buf[BUFSIZED];
    //std::cout << "ACYaafeWriter::process starts" << std::endl;
    while (!in->empty())
    {
        values.push_back( std::vector<float>(in->info().size) );
        double* data = in->readToken();
        int strSize = sprintf(buf,"%0.*e",m_precision,data[0]);
        (values.back())[0] = ((float)(data[0]));
        //std::cout << " data[0] " << data[0] << std::endl;

#ifdef SAVE_CSV
        m_fout.write(buf,strSize);
#endif

        time += (float)in->info().sampleStep / (float)in->info().sampleRate;
        times.push_back(time);

        for (int i=1;i<in->info().size;i++)
        {
            strSize = sprintf(buf,",%0.*e",m_precision,data[i]);
#ifdef SAVE_CSV
            m_fout.write(buf,strSize);
#endif
            (values.back())[i] = ((float)(data[i]));
            //std::cout << "i " << i << " data[i] " << data[i] << std::endl;
        }
#ifdef SAVE_CSV
        m_fout << endl;
#endif
        in->consumeToken();
    }
    if(times.size()>0 && values.size()>0){
        feat = new ACMediaTimedFeature(times,values,m_feature_name);
        //std::cout << "ACYaafeWriter::process: feature " << m_feature_name << " times " << times.size() << " dims " << values.size() << "x" << values.front().size() << std::endl;
    }
    else{
        std::cerr <<  "ACYaafeWriter::process: empty times/values" << std::endl;
        return false;
    }

    if(m_plugin && feat){
        return m_plugin->addMediaTimedFeature(feat,m_file);
    }
    return true;
}

void ACYaafeWriter::flush(Ports<InputBuffer*>& in, Ports<OutputBuffer*>& out)
{
    //std::cout << "ACYaafeWriter::flush " << m_feature_name << " on " << m_file << std::endl;
    process(in,out);
#ifdef SAVE_CSV
    m_fout.close();
#endif
}

}
