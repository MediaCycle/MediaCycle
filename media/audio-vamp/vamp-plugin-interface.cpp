/*
 *  vamp-plugin-interface.cpp
 *  MediaCycle
 *
 *  Adapted by Damien Tardieu on 08/06/2010 from the VAMP SDK (BSD).
 *  Updated by Christian Frisson on 13/08/2012.
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

/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Vamp

    An API for audio analysis and feature extraction plugins.

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2006 Chris Cannam, copyright 2007-2008 QMUL.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/


/*
 * This "simple" Vamp plugin host is no longer as simple as it was; it
 * now has a lot of options and includes a lot of code to handle the
 * various useful listing modes it supports.
 *
 * However, the runPlugin function still contains a reasonable
 * implementation of a fairly generic Vamp plugin host capable of
 * evaluating a given output on a given plugin for a sound file read
 * via libsndfile.
 */


#include "vamp-plugin-interface.h"

/*ACMediaTimedFeature* runPlugin(string soname, string id, string output, int outputNo, float* data, int nframes, int sr_hz, int channels, bool useFrames){
    ACMediaTimedFeature* descmf;
    PluginLoader *loader = PluginLoader::getInstance();
    string outfilename = "";
    PluginLoader::PluginKey key = loader->composePluginKey(soname, id);
    

    Plugin *plugin = loader->loadPlugin
        (key, sr_hz, PluginLoader::ADAPT_ALL_SAFE);
    if (!plugin) {
        cerr << "vamp interface : ERROR: Failed to load plugin \"" << id
             << "\" from library \"" << soname << "\"" << endl;
        return descmf;
    }

    cerr << "Running plugin: \"" << plugin->getIdentifier() << "\"..." << endl;

    // Note that the following would be much simpler if we used a
    // PluginBufferingAdapter as well -- i.e. if we had passed
    // PluginLoader::ADAPT_ALL to loader->loadPlugin() above, instead
    // of ADAPT_ALL_SAFE.  Then we could simply specify our own block
    // size, keep the step size equal to the block size, and ignore
    // the plugin's bleatings.  However, there are some issues with
    // using a PluginBufferingAdapter that make the results sometimes
    // technically different from (if effectively the same as) the
    // un-adapted plugin, so we aren't doing that here.  See the
    // PluginBufferingAdapter documentation for details.

    int blockSize = plugin->getPreferredBlockSize();
    int stepSize = plugin->getPreferredStepSize();

    if (blockSize == 0) {
        blockSize = 1024;
    }
    if (stepSize == 0) {
        if (plugin->getInputDomain() == Plugin::FrequencyDomain) {
            stepSize = blockSize/2;
        } else {
            stepSize = blockSize;
        }
    } else if (stepSize > blockSize) {
        cerr << "WARNING: stepSize " << stepSize << " > blockSize " << blockSize << ", resetting blockSize to ";
        if (plugin->getInputDomain() == Plugin::FrequencyDomain) {
            blockSize = stepSize * 2;
        } else {
            blockSize = stepSize;
        }
        cerr << blockSize << endl;
    }


    float **plugbuf = new float*[channels];
    for (int c = 0; c < channels; ++c) plugbuf[c] = new float[blockSize + 2];

    cerr << "Using block size = " << blockSize << ", step size = "
              << stepSize << endl;

    // The channel queries here are for informational purposes only --
    // a PluginChannelAdapter is being used automatically behind the
    // scenes, and it will take case of any channel mismatch

    int minch = plugin->getMinChannelCount();
    int maxch = plugin->getMaxChannelCount();
    cerr << "Plugin accepts " << minch << " -> " << maxch << " channel(s)" << endl;
    cerr << "Sound file has " << channels << " (will mix/augment if necessary)" << endl;

    Plugin::OutputList outputs = plugin->getOutputDescriptors();
    Plugin::OutputDescriptor od;
    
    for(Plugin::OutputList::iterator out = outputs.begin();out!=outputs.end();out++)
        std::cout << "Output: " << out->name << std::endl;


    //n_cols

    int returnValue = 1;
    int progress = 0;

    RealTime rt;
    PluginWrapper *wrapper = 0;
    RealTime adjustment = RealTime::zeroTime;

 long nbFrames;

    if (outputs.empty()) {
        cerr << "ERROR: Plugin has no outputs!" << endl;
        goto done;
    }

    if (outputNo < 0) {

        for (size_t oi = 0; oi < outputs.size(); ++oi) {
            if (outputs[oi].identifier == output) {
                outputNo = oi;
                break;
            }
        }

        if (outputNo < 0) {
            cerr << "ERROR: Non-existent output \"" << output << "\" requested" << endl;
            goto done;
        }

    } else {

        if (int(outputs.size()) <= outputNo) {
            cerr << "ERROR: Output " << outputNo << " requested, but plugin has only " << outputs.size() << " output(s)" << endl;
            goto done;
        }        
    }

    od = outputs[outputNo];
    cerr << "Output is: \"" << od.identifier << "\"" << endl;

    if (!plugin->initialise(channels, stepSize, blockSize)) {
        cerr << "ERROR: Plugin initialise (channels = " << channels
             << ", stepSize = " << stepSize << ", blockSize = "
             << blockSize << ") failed." << endl;
        goto done;
    }

    wrapper = dynamic_cast<PluginWrapper *>(plugin);
    if (wrapper) {
        // See documentation for
        // PluginInputDomainAdapter::getTimestampAdjustment
        PluginInputDomainAdapter *ida =
            wrapper->getWrapper<PluginInputDomainAdapter>();
        if (ida) adjustment = ida->getTimestampAdjustment();
    }


    nbFrames = (long)(nframes)/stepSize+1;
    descmf = new ACMediaTimedFeature(nbFrames, outputs[0].binCount, outputs[0].name); 
    
    for (sf_count_t i = 0; i < nframes; i += stepSize) {

        int count;
        count = nframes - i;
        
        for (int c = 0; c < channels; ++c) {
            int j = 0;
            while (j < count & j < blockSize) {
                plugbuf[c][j] = data[j * channels + c + i*channels];
                ++j;
            }
            while (j < blockSize) {
                plugbuf[c][j] = 0.0f;
                ++j;
            }
        }

        rt = RealTime::frame2RealTime(i, sr_hz);
        
        RealTime currentTime;
        //.values
        Plugin::FeatureSet fs;
        fs = plugin->process(plugbuf, rt);
            //descmf->setTimeAndValueForIndex((long)(i/stepSize), (float) rt.msec()/1000, fs.begin()->second.begin()->values);


         //printFeatures(RealTime::realTime2Frame(rt + adjustment, sr_hz),
         //     sr_hz, outputNo, plugin->process(plugbuf, rt),
         //     0, useFrames);  //out = 0;

        progress = lrintf((float(i) / nframes) * 100.f);
    }

    rt = RealTime::frame2RealTime(nframes, sr_hz);


    //printFeatures(RealTime::realTime2Frame(rt + adjustment, sr_hz),
    //               sr_hz, outputNo,
    //               plugin->getRemainingFeatures(), 0, useFrames); // out = 0;

    returnValue = 0;
done:
    delete plugin;
    return descmf;
}*/

void
printPluginPath(bool verbose)
{
    if (verbose) {
        cout << "\nVamp plugin search path: ";
    }

    vector<string> path = PluginHostAdapter::getPluginPath();
    for (size_t i = 0; i < path.size(); ++i) {
        if (verbose) {
            cout << "[" << path[i] << "]";
        } else {
            cout << path[i] << endl;
        }
    }

    if (verbose) cout << endl;
}

static
string
header(string text, int level)
{
    string out = '\n' + text + '\n';
    for (size_t i = 0; i < text.length(); ++i) {
        out += (level == 1 ? '=' : level == 2 ? '-' : '~');
    }
    out += '\n';
    return out;
}

void
enumeratePlugins(Verbosity verbosity)
{
    PluginLoader *loader = PluginLoader::getInstance();

    if (verbosity == PluginInformation) {
        cout << "\nVamp plugin libraries found in search path:" << endl;
    }

    vector<PluginLoader::PluginKey> plugins = loader->listPlugins();
    typedef multimap<string, PluginLoader::PluginKey>
            LibraryMap;
    LibraryMap libraryMap;

    for (size_t i = 0; i < plugins.size(); ++i) {
        string path = loader->getLibraryPathForPlugin(plugins[i]);
        libraryMap.insert(LibraryMap::value_type(path, plugins[i]));
    }

    string prevPath = "";
    int index = 0;

    for (LibraryMap::iterator i = libraryMap.begin();
         i != libraryMap.end(); ++i) {

        string path = i->first;
        PluginLoader::PluginKey key = i->second;

        if (path != prevPath) {
            prevPath = path;
            index = 0;
            if (verbosity == PluginInformation) {
                cout << "\n  " << path << ":" << endl;
            } else if (verbosity == PluginInformationDetailed) {
                string::size_type ki = i->second.find(':');
                string text = "Library \"" + i->second.substr(0, ki) + "\"";
                cout << "\n" << header(text, 1);
            }
        }

        Plugin *plugin = loader->loadPlugin(key, 48000);
        if (plugin) {

            char c = char('A' + index);
            if (c > 'Z') c = char('a' + (index - 26));

            PluginLoader::PluginCategoryHierarchy category =
                    loader->getPluginCategory(key);
            string catstr;
            if (!category.empty()) {
                for (size_t ci = 0; ci < category.size(); ++ci) {
                    if (ci > 0) catstr += " > ";
                    catstr += category[ci];
                }
            }

            if (verbosity == PluginInformation) {

                cout << "    [" << c << "] [v"
                     << plugin->getVampApiVersion() << "] "
                     << plugin->getName() << ", \""
                     << plugin->getIdentifier() << "\"" << " ["
                     << plugin->getMaker() << "]" << endl;

                if (catstr != "") {
                    cout << "       > " << catstr << endl;
                }

                if (plugin->getDescription() != "") {
                    cout << "        - " << plugin->getDescription() << endl;
                }

            } else if (verbosity == PluginInformationDetailed) {

                cout << header(plugin->getName(), 2);
                cout << " - Identifier:         "
                     << key << endl;
                cout << " - Plugin Version:     "
                     << plugin->getPluginVersion() << endl;
                cout << " - Vamp API Version:   "
                     << plugin->getVampApiVersion() << endl;
                cout << " - Maker:              \""
                     << plugin->getMaker() << "\"" << endl;
                cout << " - Copyright:          \""
                     << plugin->getCopyright() << "\"" << endl;
                cout << " - Description:        \""
                     << plugin->getDescription() << "\"" << endl;
                cout << " - Input Domain:       "
                     << (plugin->getInputDomain() == Vamp::Plugin::TimeDomain ?
                             "Time Domain" : "Frequency Domain") << endl;
                cout << " - Default Step Size:  "
                     << plugin->getPreferredStepSize() << endl;
                cout << " - Default Block Size: "
                     << plugin->getPreferredBlockSize() << endl;
                cout << " - Minimum Channels:   "
                     << plugin->getMinChannelCount() << endl;
                cout << " - Maximum Channels:   "
                     << plugin->getMaxChannelCount() << endl;

            } else if (verbosity == PluginIds) {
                cout << "vamp:" << key << endl;
            }

            Plugin::OutputList outputs =
                    plugin->getOutputDescriptors();

            if (verbosity == PluginInformationDetailed) {

                Plugin::ParameterList params = plugin->getParameterDescriptors();
                for (size_t j = 0; j < params.size(); ++j) {
                    Plugin::ParameterDescriptor &pd(params[j]);
                    cout << "\nParameter " << j+1 << ": \"" << pd.name << "\"" << endl;
                    cout << " - Identifier:         " << pd.identifier << endl;
                    cout << " - Description:        \"" << pd.description << "\"" << endl;
                    if (pd.unit != "") {
                        cout << " - Unit:               " << pd.unit << endl;
                    }
                    cout << " - Range:              ";
                    cout << pd.minValue << " -> " << pd.maxValue << endl;
                    cout << " - Default:            ";
                    cout << pd.defaultValue << endl;
                    if (pd.isQuantized) {
                        cout << " - Quantize Step:      "
                             << pd.quantizeStep << endl;
                    }
                    if (!pd.valueNames.empty()) {
                        cout << " - Value Names:        ";
                        for (size_t k = 0; k < pd.valueNames.size(); ++k) {
                            if (k > 0) cout << ", ";
                            cout << "\"" << pd.valueNames[k] << "\"";
                        }
                        cout << endl;
                    }
                }

                if (outputs.empty()) {
                    cout << "\n** Note: This plugin reports no outputs!" << endl;
                }
                for (size_t j = 0; j < outputs.size(); ++j) {
                    Plugin::OutputDescriptor &od(outputs[j]);
                    cout << "\nOutput " << j+1 << ": \"" << od.name << "\"" << endl;
                    cout << " - Identifier:         " << od.identifier << endl;
                    cout << " - Description:        \"" << od.description << "\"" << endl;
                    if (od.unit != "") {
                        cout << " - Unit:               " << od.unit << endl;
                    }
                    if (od.hasFixedBinCount) {
                        cout << " - Default Bin Count:  " << od.binCount << endl;
                    }
                    if (!od.binNames.empty()) {
                        bool have = false;
                        for (size_t k = 0; k < od.binNames.size(); ++k) {
                            if (od.binNames[k] != "") {
                                have = true; break;
                            }
                        }
                        if (have) {
                            cout << " - Bin Names:          ";
                            for (size_t k = 0; k < od.binNames.size(); ++k) {
                                if (k > 0) cout << ", ";
                                cout << "\"" << od.binNames[k] << "\"";
                            }
                            cout << endl;
                        }
                    }
                    if (od.hasKnownExtents) {
                        cout << " - Default Extents:    ";
                        cout << od.minValue << " -> " << od.maxValue << endl;
                    }
                    if (od.isQuantized) {
                        cout << " - Quantize Step:      "
                             << od.quantizeStep << endl;
                    }
                    cout << " - Sample Type:        "
                         << (od.sampleType ==
                             Plugin::OutputDescriptor::OneSamplePerStep ?
                                 "One Sample Per Step" :
                                 od.sampleType ==
                                 Plugin::OutputDescriptor::FixedSampleRate ?
                                     "Fixed Sample Rate" :
                                     "Variable Sample Rate") << endl;
                    if (od.sampleType !=
                            Plugin::OutputDescriptor::OneSamplePerStep) {
                        cout << " - Default Rate:       "
                             << od.sampleRate << endl;
                    }
                    cout << " - Has Duration:       "
                         << (od.hasDuration ? "Yes" : "No") << endl;
                }
            }

            if (outputs.size() > 1 || verbosity == PluginOutputIds) {
                for (size_t j = 0; j < outputs.size(); ++j) {
                    if (verbosity == PluginInformation) {
                        cout << "         (" << j << ") "
                             << outputs[j].name << ", \""
                             << outputs[j].identifier << "\"" << endl;
                        if (outputs[j].description != "") {
                            cout << "             - "
                                 << outputs[j].description << endl;
                        }
                    } else if (verbosity == PluginOutputIds) {
                        cout << "vamp:" << key << ":" << outputs[j].identifier << endl;
                    }
                }
            }

            ++index;

            delete plugin;
        }
    }

    if (verbosity == PluginInformation ||
            verbosity == PluginInformationDetailed) {
        cout << endl;
    }
}

ACMediaTimedFeature* runPlugin(string soname, string id,
                               string output, int outputNo, string wavname,
                               bool useFrames)
{
    ACMediaTimedFeature* descmf(0);
    PluginLoader *loader = PluginLoader::getInstance();

    PluginLoader::PluginKey key = loader->composePluginKey(soname, id);

    SNDFILE *sndfile;
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(SF_INFO));

    sndfile = sf_open(wavname.c_str(), SFM_READ, &sfinfo);
    if (!sndfile) {
        cerr << ": ERROR: Failed to open input file \""
             << wavname << "\": " << sf_strerror(sndfile) << endl;
        return descmf;
    }


    Plugin *plugin = loader->loadPlugin
            (key, sfinfo.samplerate, PluginLoader::ADAPT_ALL_SAFE);
    if (!plugin) {
        cerr << ": ERROR: Failed to load plugin \"" << id
             << "\" from library \"" << soname << "\"" << endl;
        sf_close(sndfile);
        return descmf;
    }

    cerr << "Running plugin: \"" << plugin->getIdentifier() << "\"..." << endl;

    // Note that the following would be much simpler if we used a
    // PluginBufferingAdapter as well -- i.e. if we had passed
    // PluginLoader::ADAPT_ALL to loader->loadPlugin() above, instead
    // of ADAPT_ALL_SAFE.  Then we could simply specify our own block
    // size, keep the step size equal to the block size, and ignore
    // the plugin's bleatings.  However, there are some issues with
    // using a PluginBufferingAdapter that make the results sometimes
    // technically different from (if effectively the same as) the
    // un-adapted plugin, so we aren't doing that here.  See the
    // PluginBufferingAdapter documentation for details.

    int blockSize = plugin->getPreferredBlockSize();
    int stepSize = plugin->getPreferredStepSize();

    if (blockSize == 0) {
        blockSize = 1024;
    }
    if (stepSize == 0) {
        if (plugin->getInputDomain() == Plugin::FrequencyDomain) {
            stepSize = blockSize/2;
        } else {
            stepSize = blockSize;
        }
    } else if (stepSize > blockSize) {
        cerr << "WARNING: stepSize " << stepSize << " > blockSize " << blockSize << ", resetting blockSize to ";
        if (plugin->getInputDomain() == Plugin::FrequencyDomain) {
            blockSize = stepSize * 2;
        } else {
            blockSize = stepSize;
        }
        cerr << blockSize << endl;
    }
    int overlapSize = blockSize - stepSize;
    sf_count_t currentStep = 0;
    int finalStepsRemaining = max(1, (blockSize / stepSize) - 1); // at end of file, this many part-silent frames needed after we hit EOF

    int channels = sfinfo.channels;

    float *filebuf = new float[blockSize * channels];
    float **plugbuf = new float*[channels];
    for (int c = 0; c < channels; ++c) plugbuf[c] = new float[blockSize + 2];

    cerr << "Using block size = " << blockSize << ", step size = "
         << stepSize << endl;

    // The channel queries here are for informational purposes only --
    // a PluginChannelAdapter is being used automatically behind the
    // scenes, and it will take case of any channel mismatch

    int minch = plugin->getMinChannelCount();
    int maxch = plugin->getMaxChannelCount();
    cerr << "Plugin accepts " << minch << " -> " << maxch << " channel(s)" << endl;
    cerr << "Sound file has " << channels << " (will mix/augment if necessary)" << endl;

    Plugin::OutputList outputs = plugin->getOutputDescriptors();
    Plugin::OutputDescriptor od;

    int progress = 0;

    RealTime rt;
    PluginWrapper *wrapper = 0;
    RealTime adjustment = RealTime::zeroTime;
    int c = 0;
    Plugin::FeatureSet fs;

    if (outputs.empty()) {
        cerr << "ERROR: Plugin has no outputs!" << endl;
        goto done;
    }

    if (outputNo < 0) {

        for (size_t oi = 0; oi < outputs.size(); ++oi) {
            if (outputs[oi].identifier == output) {
                outputNo = oi;
                break;
            }
        }

        if (outputNo < 0) {
            cerr << "ERROR: Non-existent output \"" << output << "\" requested" << endl;
            goto done;
        }

    } else {

        if (int(outputs.size()) <= outputNo) {
            cerr << "ERROR: Output " << outputNo << " requested, but plugin has only " << outputs.size() << " output(s)" << endl;
            goto done;
        }
    }

    od = outputs[outputNo];
    cerr << "Output is: \"" << od.identifier << "\"" << endl;

    if (!plugin->initialise(channels, stepSize, blockSize)) {
        cerr << "ERROR: Plugin initialise (channels = " << channels
             << ", stepSize = " << stepSize << ", blockSize = "
             << blockSize << ") failed." << endl;
        goto done;
    }

    wrapper = dynamic_cast<PluginWrapper *>(plugin);
    if (wrapper) {
        // See documentation for
        // PluginInputDomainAdapter::getTimestampAdjustment
        PluginInputDomainAdapter *ida =
                wrapper->getWrapper<PluginInputDomainAdapter>();
        if (ida) adjustment = ida->getTimestampAdjustment();
    }

    // Here we iterate over the frames, avoiding asking the numframes in case it's streaming input.
    do {

        int count;

        if ((blockSize==stepSize) || (currentStep==0)) {
            // read a full fresh block
            if ((count = sf_readf_float(sndfile, filebuf, blockSize)) < 0) {
                cerr << "ERROR: sf_readf_float failed: " << sf_strerror(sndfile) << endl;
                break;
            }
            if (count != blockSize) --finalStepsRemaining;
        } else {
            //  otherwise shunt the existing data down and read the remainder.
            memmove(filebuf, filebuf + (stepSize * channels), overlapSize * channels * sizeof(float));
            if ((count = sf_readf_float(sndfile, filebuf + (overlapSize * channels), stepSize)) < 0) {
                cerr << "ERROR: sf_readf_float failed: " << sf_strerror(sndfile) << endl;
                break;
            }
            if (count != stepSize) --finalStepsRemaining;
            count += overlapSize;
        }

        for (int c = 0; c < channels; ++c) {
            int j = 0;
            while (j < count) {
                plugbuf[c][j] = filebuf[j * sfinfo.channels + c];
                ++j;
            }
            while (j < blockSize) {
                plugbuf[c][j] = 0.0f;
                ++j;
            }
        }

        rt = RealTime::frame2RealTime(currentStep * stepSize, sfinfo.samplerate);

        /*RealTime currentTime;
        //.values
        Plugin::FeatureSet fs;
        fs = plugin->process(plugbuf, rt);
        descmf->setTimeAndValueForIndex((long)(currentStep), (float) rt.msec()/1000, fs[0][0].values);*/


        fs = plugin->process(plugbuf, rt);
        for (unsigned int i = 0; i < fs[outputNo].size(); ++i) {
            if (fs[outputNo][i].hasTimestamp) {
                rt = fs[outputNo][i].timestamp;
                if(!descmf){
                    descmf = new ACMediaTimedFeature();
                    descmf->setName(output);
                }
                descmf->setTimeAndValueForIndex((long)(c++), (double) rt.msec()/1000, fs[outputNo][i].values);
            }
        }

        printFeatures
                (RealTime::realTime2Frame(rt + adjustment, sfinfo.samplerate),
                 sfinfo.samplerate, outputNo, fs,
                 0, useFrames, descmf); // out = 0

        if (sfinfo.frames > 0){
            int pp = progress;
            progress = lrintf((float(currentStep * stepSize) / sfinfo.frames) * 100.f);
            if (progress != pp) { // && out
                cerr << "\r" << progress << "%";
            }
        }

        ++currentStep;

    } while (finalStepsRemaining > 0);

    //if (out)
    cerr << "\rDone" << endl;

    rt = RealTime::frame2RealTime(currentStep * stepSize, sfinfo.samplerate);
    //

    fs = plugin->getRemainingFeatures();
    for (unsigned int i = 0; i < fs[outputNo].size(); ++i) {
        if (fs[outputNo][i].hasTimestamp) {
            rt = fs[outputNo][i].timestamp;
            if(!descmf){
                descmf = new ACMediaTimedFeature();
                descmf->setName(output);
            }
            descmf->setTimeAndValueForIndex((long)(c++), (double) rt.msec()/1000, fs[outputNo][i].values);
        }
    }
    c++;


    printFeatures(RealTime::realTime2Frame(rt + adjustment, sfinfo.samplerate),
                  sfinfo.samplerate, outputNo,
                  fs, 0, useFrames, descmf);  // out = 0

done:
    delete plugin;
    /*if (out) {
        out->close();
        delete out;
    }*/
    sf_close(sndfile);
    return descmf;
}

void
printFeatures(int frame, int sr, int output,
              Plugin::FeatureSet features, ofstream *out, bool useFrames, ACMediaTimedFeature* descmf )
{
    for (unsigned int i = 0; i < features[output].size(); ++i) {
        cout << "Feature " << i;

        if (useFrames) {

            int displayFrame = frame;

            if (features[output][i].hasTimestamp) {
                displayFrame = RealTime::realTime2Frame
                        (features[output][i].timestamp, sr);
            }

            cout << displayFrame;

            if (features[output][i].hasDuration) {
                displayFrame = RealTime::realTime2Frame
                        (features[output][i].duration, sr);
                cout << "," << displayFrame;
            }

            cout  << ":";

        } else {

            RealTime rt = RealTime::frame2RealTime(frame, sr);

            if (features[output][i].hasTimestamp) {
                rt = features[output][i].timestamp;
            }

            cout << rt.toString();

            if (features[output][i].hasDuration) {
                rt = features[output][i].duration;
                cout << "," << rt.toString();
            }

            cout << ":";
        }

        for (unsigned int j = 0; j < features[output][i].values.size(); ++j) {
            cout << " " << features[output][i].values[j];
        }
        cout << " " << features[output][i].label;

        cout << endl;
    }
}

