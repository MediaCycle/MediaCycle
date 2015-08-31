/**
 * @brief Essentia class to save features (pool representations) as mtf files
 * @author Christian Frisson
 * @date 27/08/2015
 * @copyright (c) 2015 – UMONS - Numediart
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

/// Adapted from Essentia's YamlOutput class (src/algorithms/io/yamloutput.*)

#ifndef ESSENTIA_MTF_OUTPUT_H
#define ESSENTIA_MTF_OUTPUT_H

#include <essentia/algorithm.h>
#include <essentia/pool.h>


#include "MediaCycle.h"

// A MtfNode represents a node in the MTF tree. A MtfNode without any value
// is valid, it is simply a namespace identifier. It is required that every
// *leaf* node in a MTF tree have a defined value though.
struct MtfNode {
    std::string name;
    essentia::Parameter* value;
    std::vector<MtfNode*> children;
    
    MtfNode(const std::string& n) : name(n), value(0) {}
    
    ~MtfNode () {
        delete value;
        for (int i=0; i<(int)children.size(); ++i) {
            delete children[i];
        }
    }
};


class ACAudioEssentiaMtfOutput : public essentia::standard::Algorithm {
    
protected:
    essentia::standard::Input<essentia::Pool> _pool;
    std::string _filename;
    bool _doubleCheck;
    bool _outputJSON;
    int _indent;
    bool _writeVersion;
    
    void outputToStream(std::ostream* out);
    
public:
    
    ACAudioEssentiaMtfOutput() {
        declareInput(_pool, "pool", "Pool to serialize into a mtf formatted file");
    }
    
    void declareParameters() {
        declareParameter("filename", "output filename (use '-' to emit to stdout)", "", "-");
        declareParameter("indent", "(json only) how many characters to indent each line, or 0 for no newlines", "", 4);
        declareParameter("writeVersion", "whether to write the essentia version to the output file", "", true);
        declareParameter("doubleCheck", "whether to double-check if the file has been correctly written to the disk", "", false);
        declareParameter("format", "whether to output data in mtf format", "{json,yaml}", "yaml");
    }
    
    
    void compute();
    void configure();
    
    static const char* name;
    static const char* description;
    
    void clearFeatures();
    void clearTimedFeatures();
    std::map<std::string,ACMediaFeatures*> getFeatures();
    std::map<std::string,ACMediaTimedFeature*> getTimedFeatures();
    
    
protected:
    template <typename StreamType> void emitMtf(StreamType* s, MtfNode* n, const std::string& indent);
    void outputMtfToStream(MtfNode& root, std::ostream* out);
    
    std::map<std::string,ACMediaTimedFeature*> descmtf;
    std::map<std::string,ACMediaFeatures*> descmf;
  
    
};

#endif // ESSENTIA_MTF_OUTPUT_H
