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

#include "ACAudioEssentiaMtfOutput.h"
#include <essentia/essentia.h>
//#include <essentia/utils/output.h>
#include <fstream>
#include <sstream> // escapeJsonString

using namespace std;
using namespace essentia;
using namespace standard;

const char* ACAudioEssentiaMtfOutput::name = "MtfOutput";
const char* ACAudioEssentiaMtfOutput::description = DOC("This algorithm emits a MediaCycle mtf representation of a Pool.\n"
                                                        "\n"
                                                        "Each descriptor key in the Pool is decomposed into different nodes of the mtf format by "
                                                        "dotsplitting on the '.' character. For example a Pool that looks like this:\n"
                                                        "\n"
                                                        "    foo.bar.some.thing: [23.1, 65.2, 21.3]\n"
                                                        "\n"
                                                        "will be emitted as:\n"
                                                        "\n"
                                                        "    metadata:\n"
                                                        "        essentia:\n"
                                                        "            version: <version-number>\n"
                                                        "\n"
                                                        "    foo:\n"
                                                        "        bar:\n"
                                                        "            some:\n"
                                                        "                thing: [23.1, 65.2, 21.3]");

// TODO arrange keys in alphabetical order and make sure to add that to the
// dictionary, when implementing this, it should be made general enough to
// add other sorting mechanisms (eg numerically, by size, custom ordering).

// Defined here because the json helper functions are
// not part of the ACAudioEssentiaMtfOutput class
/*string _jsonN;*/

void ACAudioEssentiaMtfOutput::configure() {
    _filename = parameter("filename").toString();
    _doubleCheck = parameter("doubleCheck").toBool();
    _outputJSON = (parameter("format").toLower() == "json");
    _indent = parameter("indent").toInt();
    /*if (_indent > 0) {
    _jsonN = "\n";
  } else {
    _jsonN = "";
  }*/
    _writeVersion = parameter("writeVersion").toBool();
    
    if (_filename == "") throw EssentiaException("please provide a valid filename");
}

// this function dotsplits a string up on the '.' char and returns a vector of
// strings where each element represents a string between dots.
vector<string> dotsplit(const string& s) {
    string::size_type dotpos = s.find('.');
    string::size_type prevdotpos = 0;
    vector<string> result;
    
    if (dotpos != string::npos) {
        result.push_back(s.substr(0, dotpos));
    }
    else {
        result.push_back(s);
        return result;
    }
    
    if (dotpos+1 == string::npos) {
        return result;
    }
    
    prevdotpos = dotpos;
    dotpos = s.find('.', prevdotpos+1);
    
    while (dotpos != string::npos) {
        if (prevdotpos+1 == string::npos) return result;
        
        result.push_back( s.substr(prevdotpos + 1, dotpos - (prevdotpos + 1)) );
        prevdotpos = dotpos;
        dotpos = s.find('.', prevdotpos+1);
    }
    
    // add last bit
    result.push_back( s.substr(prevdotpos+1) );
    
    return result;
}

/*
// this function escapes utf-8 string to be compatible with JSON standard,
// but it does not handle invalid utf-8 characters. Values in the pool are
// expected to be correct utf-8 strings, and it is up to the user to provide
// correct utf-8 strings for the names of descriptors in the Pool. This
// function is called for both Pool descriptor names and string values.
string escapeJsonString(const string& input) {
  ostringstream escaped;
  for (string::const_iterator i = input.begin(); i != input.end(); i++) {
    switch (*i) {
      case '\n': escaped << "\\n"; break;
      case '\r': escaped << "\\r"; break;
      case '\t': escaped << "\\t"; break;
      case '\f': escaped << "\\f"; break;
      case '\b': escaped << "\\b"; break;
      case '"': escaped << "\\\""; break;
      case '/': escaped << "\\/"; break;
      case '\\': escaped << "\\\\"; break;
      default: escaped << *i; break;
    }
  }
  return escaped.str();
}
*/


template <typename IterType>
void fillMtfTreeHelper(MtfNode* root, const IterType it) {
    vector<string> pathparts = dotsplit(it->first);
    MtfNode* currNode = root;
    
    // iterate over each of the pieces of the path
    for (int i=0; i<(int)pathparts.size(); ++i) {
        bool newNode = true;
        
        // search to see if the path part is already in the tree
        for (int j=0; j<(int)currNode->children.size(); ++j) {
            if (currNode->children[j]->name == pathparts[i]) { // already in the tree
                newNode = false;
                currNode = currNode->children[j];
                break;
            }
        }
        
        if (newNode) { // path part was not found in the tree
            MtfNode* newNode = new MtfNode(pathparts[i]);
            currNode->children.push_back(newNode);
            currNode = newNode;
        }
    }
    
    // end of the path
    currNode->value = new Parameter(it->second);
}

/*
 fillMtfTree (Pool, root MtfNode):
 Places all the values from the pool <string, val> into a tree
 e.g. a pool like this:
   foo1.bar  [134.2, 343.234]
   foo2.bar  ["hello"]
   
   
 will get translated to a tree like this:
 
                     __root
                    /      \
                   /        \
                  /          \
                 /            \
                /              \
              foo1             foo2
              /                  \
             /                    \
   bar ->[ 134.2, 343.234]    bar ->[ "hello"]
   
   
 And the YAML will look like:
 
 foo1:
     bar: [ 134.2, 343.234]
 foo2:
     bar: [ "hello"]
     
     
*/



void fillMtfTree (const Pool& p, MtfNode* root) {
#define FILL_MTF_TREE_MACRO(type, tname)                                    \
    for (map<string, type >::const_iterator it = p.get##tname##Pool().begin();   \
    it != p.get##tname##Pool().end(); ++it) {                               \
    fillMtfTreeHelper(root, it);                                              \
}
    
    FILL_MTF_TREE_MACRO(Real, SingleReal);
    FILL_MTF_TREE_MACRO(vector<Real>, Real);
    FILL_MTF_TREE_MACRO(vector<Real>, SingleVectorReal);
    FILL_MTF_TREE_MACRO(vector<vector<Real> >, VectorReal);
    
    FILL_MTF_TREE_MACRO(string, SingleString);
    FILL_MTF_TREE_MACRO(vector<string>, String);
    FILL_MTF_TREE_MACRO(vector<vector<string> >, VectorString);
    
    FILL_MTF_TREE_MACRO(vector<TNT::Array2D<Real> >, Array2DReal);
    FILL_MTF_TREE_MACRO(vector<StereoSample>, StereoSample);
    
#undef FILL_MTF_TREE_MACRO
}


// Emits YAML given a MtfNode root to a specified stream.
// This is a recursive solution.
template <typename StreamType>
void ACAudioEssentiaMtfOutput::emitMtf(StreamType* s, MtfNode* p, MtfNode* n, const string& indent) {

    *s << indent << n->name << ":";
    ////std::cout << n->name << std::endl;
    
    //CF
    /*if(n->name == "configuration" || n->name == "metadata")
        return;*/

    if (n->children.empty()) { // if there are no children, emit the value here
        if (n->value != NULL) {
            *s << " " << *(n->value) << "\n";  // Parameters know how to be emitted to streams
            
            //std::string _filename;
            
            
            const Pool& _p = _pool.get();            
            
            std::vector<std::string> descNames = _p.descriptorNames();
            std::string category;
            std::string feature;
            std::string stat;
            std::string mc_feature;
            std::string mc_stat;
            
            bool isMetadata = false;
            bool isConfiguration = false;
            bool isFeature = false;
            bool isStat = false;
            
            for(std::vector<std::string>::iterator descName = descNames.begin(); descName != descNames.end(); descName++){
                std::vector<std::string> splits = dotsplit(*descName);
                //////std::cout << "descName " << *descName << " dots " << splits.size() << " " << std::endl; 
                if(splits.size()>2 && splits.back() == n->name && splits[splits.size()-2] == p->name){
                    if(splits.front() == "metadata"){
                        isMetadata = true;
                    }
                    else if(splits.front() == "configuration"){
                        isConfiguration = true;
                    }
                    else{
                        if(splits.size()==2){
                            isFeature = true;
                            feature = splits[splits.size()-1];
                            category = splits[splits.size()-2];
                            mc_feature = feature;
                        }
                        else if(splits.size()==3){
                            isStat = true;
                            stat = splits[splits.size()-1];
                            
                            // present in ACMediaTimedFeature, but missing from Essentia
                            // Centroid
                            // Weighted mean
                            // Spread
                            // Weighted standard deviation
                            // Standard deviation
                            // Cor
                            // modulation
                            // log Centroid
                            // log Spread
                            // log Skewness"
                            // Temporal Model
                            // present in ACMediaTimedFeature and Essentia
                            if(stat == "min") mc_stat = "Min"; //(minimum)
                            else if(stat == "max") mc_stat = "Max"; //(maximum)
                            else if(stat == "mean") mc_stat = "Mean"; //(mean)
                            else if(stat == "skew") mc_stat = "Skewness"; //(skewness)
                            else if(stat == "kurt") mc_stat = "Kurtosis"; //(kurtosis)
                            else if(stat == "cov") mc_stat = "Cov"; //(covariance)
                            // present in Essentia , but missing from ACMediaTimedFeature and 
                            else if(stat == "median") mc_stat = "Median"; //(median)
                            else if(stat == "var") mc_stat = "Variance"; //(variance)
                            else if(stat == "dmean") mc_stat = "Mean of Derivative"; //(mean of the derivative)
                            else if(stat == "dvar") mc_stat = "Variance of Derivative"; //(variance of the derivative)
                            else if(stat == "dmean2") mc_stat = "Mean of 2nd Derivative"; //(mean of the second derivative)
                            else if(stat == "dvar2") mc_stat = "Variance of 2nd Derivative"; //(variance of the second derivative)
                            else if(stat == "icov") mc_stat = "Inverse Covariance"; //(inverse covariance).
                            //else if(stat == "copy") mc_stat = ""; //(verbatim copy of descriptor, no aggregation; exclusive: cannot be performed with any other statistical units).
                            //else if(stat == "value") mc_stat = ""; //(copy of the descriptor, but the value is placed under the name '<descriptor name>.value')
                            else isStat = false;
                            
                            feature = splits[splits.size()-2];
                            category = splits[splits.size()-3];
                            mc_feature = feature + " " + mc_stat;
                            
                        }
                    }
                    break;
                }
            }
            
            //std::cout << " "<< _filename /*<< std::endl*/;
            
            if(isFeature){
                //std::cout << " Category " << category /*<< std::endl*/;
                //std::cout << " Feature " /*<< std::endl*/;
                //std::cout << " "<< n->name /*<< std::endl*/;
            }
            else if(isStat){
                //std::cout << " Category " << category /*<< std::endl*/;
                //std::cout << " Feature " << feature /*<< std::endl*/;
                //std::cout << " Stat " /*<< std::endl*/;
                //std::cout << " "<< n->name /*<< std::endl*/;
            }
            else{
                //std::cout << " Other " /*<< std::endl*/;
                //std::cout << " "<< n->name /*<< std::endl*/;
            }
            
            
            
            if(isFeature || isStat){
                
                float channels = _p.value<Real>("metadata.audio_properties.channels");
                float analysis_sample_rate = _p.value<Real>("metadata.audio_properties.analysis_sample_rate");
                float length = _p.value<Real>("metadata.audio_properties.length");          
                
                float frameSize = 0;
                bool hasFrameSize = _p.contains<Real>("configuration." + category + ".frameSize");
                if(hasFrameSize){
                    frameSize = _p.value<Real>("configuration." + category + ".frameSize");
                }
                
                //std::cout << " frameSize " << frameSize /*<< std::endl*/;
                
                float hopSize = 0;
                bool hasHopSize = _p.contains<Real>("configuration." + category + ".hopSize");
                if(hasHopSize){
                    hopSize = _p.value<Real>("configuration." + category + ".hopSize");
                }
                
                //std::cout << " hopSize " << hopSize /*<< std::endl*/;
                
                //std::cout << " channels " << channels /*<< std::endl*/;
                //std::cout << " analysis_sample_rate " << analysis_sample_rate /*<< std::endl*/;
                //std::cout << " length " << length /*<< std::endl*/;
                
                bool isTemporalFeature = false;
                
                ACMediaTimedFeature* timeFeat = 0;
                ACMediaFeatures* feat = 0;
                
                Parameter::ParamType nodeType = (*(n->value)).type();
                switch (nodeType) {
                case Parameter::REAL:
                {
                    //std::cout << " Real";
                    
                    Real _value = (*(n->value)).toReal();
                    std::vector<float> _values;
                    _values.push_back(_value);
                    FeaturesVector featVector (_values);
                    feat = new ACMediaFeatures(featVector,mc_feature);                    
                }
                    break;
                case Parameter::STRING:
                {
                    //std::cout << " String";
                }
                    break;
                case Parameter::BOOL:
                {
                    //std::cout << " Bool";
                }
                    break;
                case Parameter::INT:
                {
                    //std::cout << " Int";
                }
                    break;
                case Parameter::STEREOSAMPLE:
                {
                    //std::cout << " StereoSample";
                }
                    break;
                case Parameter::VECTOR_REAL:
                {
                    //std::cout << " VectorReal";
                    vector<Real> _values = (*(n->value)).toVectorReal();
                    
                    int _size = _values.size();
                    //std::cout << " size " << _values.size() /*<< std::endl*/;  
                    //std::cout << " frames  " << analysis_sample_rate*length/hopSize /*<< std::endl*/;
                    
                    int _frames = ceil(analysis_sample_rate*length/hopSize) + 1;
                    
                    if( frameSize > 0 && hopSize > 0 && _size > 0){
                        if ( isFeature && _frames > 0 && _frames == _size){
                            std::vector<float> _times;
                            float _time = 0;
                            float _step = length*hopSize/analysis_sample_rate;
                            for(int _t = 0; _t< _frames; _t++){
                                _times.push_back(_time);
                                _time += _step;
                            }
                            isTemporalFeature = true;
                            timeFeat = new ACMediaTimedFeature(_times,_values,mc_feature);
                        }
                        else{
                            FeaturesVector featVector (_values);
                            feat = new ACMediaFeatures(featVector,mc_feature);
                        }
                    }
                }
                    break;
                case Parameter::VECTOR_STRING:
                {
                    //std::cout << " VectorString";
                }
                    break;
                case Parameter::VECTOR_BOOL:
                {
                    //std::cout << " VectorBool";
                }
                    break;  
                case Parameter::VECTOR_INT:
                {
                    //std::cout << " VectorInt";
                }
                    break;
                case Parameter::VECTOR_STEREOSAMPLE:
                {
                    //std::cout << " VectorStereoSample";
                }
                    break;
                case Parameter::VECTOR_VECTOR_REAL:
                {
                    //std::cout << " VectorVectorReal";
                    vector<vector<Real> > _values = (*(n->value)).toVectorVectorReal();
                    
                    int _dim0 = _values.size();
                    int _dim1 = (_values.size() > 0 ? _values.front().size() : 0);
                    
                    //std::cout << " dim0 " << _dim0 /*<< std::endl*/;  
                    //std::cout << " dim1 " << _dim1 /*<< std::endl*/;  
                    
                    //std::cout << " frames  " << analysis_sample_rate*length/hopSize /*<< std::endl*/;
                    
                    int _frames = ceil(analysis_sample_rate*length/hopSize) + 1;
                    
                    if( isFeature && frameSize > 0 && hopSize > 0 && _dim0 > 0 && _dim1 > 0){
                        if ( _frames > 0 && _frames == _dim0){
                            std::vector<float> _times;
                            float _time = 0;
                            float _step = length*hopSize/analysis_sample_rate;
                            for(int _t = 0; _t< _frames; _t++){
                                _times.push_back(_time);
                                _time += _step;
                            }
                            isTemporalFeature = true;
                            timeFeat = new ACMediaTimedFeature(_times,_values,mc_feature);
                        }
                        else if ( _frames > 0 && _frames == _dim1){
                            std::vector<float> _values_flat;
                            for (auto vec : _values) {
                                for (auto el : vec) {
                                    _values_flat.push_back(el);
                                }
                            }
                            arma::fmat _arma_values(_values_flat);
                            inplace_trans(_arma_values);   
                            
                            std::vector<float> _times;
                            float _time = 0;
                            float _step = length*hopSize/analysis_sample_rate;
                            for(int _t = 0; _t< _frames; _t++){
                                _times.push_back(_time);
                                _time += _step;
                            }
                            isTemporalFeature = true;
                            timeFeat = new ACMediaTimedFeature(_times,_arma_values,mc_feature);
                        }
                    }
                }
                    break;
                case Parameter::VECTOR_VECTOR_STRING:
                {
                    //std::cout << " VectorVectorString";
                }
                    break;
                case Parameter::VECTOR_VECTOR_STEREOSAMPLE:
                {
                    //std::cout << " VectorVectorStereoSample";
                }
                    break;
                case Parameter::VECTOR_MATRIX_REAL:
                {
                    //std::cout << " VectorMatrixReal";
                }
                    break;
                case Parameter::MAP_VECTOR_REAL:
                {
                    //std::cout << " MapVectorReal";
                }
                    break;
                case Parameter::MAP_VECTOR_STRING:
                {
                    //std::cout << " MapVectorString";
                }
                    break;
                case Parameter::MAP_VECTOR_INT:
                {
                    //std::cout << " MapVectorInt";
                }
                    break;
                case Parameter::MAP_REAL:
                {
                    //std::cout << " MapReal";
                }
                    break;
                case Parameter::MATRIX_REAL:
                {
                    //std::cout << " MatrixReal";
                }
                    break;
                default:
                    break;
                }
                
                if(timeFeat && isTemporalFeature){
                    this->descmtf[mc_feature] = timeFeat;
                }
                if(feat){
                    this->descmf[mc_feature] = feat;
                }
            }
            
            //std::cout << std::endl;
            
        }
        else { // you should never have this case: a key without any children or associated value
            throw EssentiaException("ACAudioEssentiaMtfOutput: input pool is invalid, contains key with no associated value");
        }
    }
    else {
        
        
        
        // we can make the assumption that this node has no value because the pool
        // doesn't not allow parent nodes to have values
        if (n->value != NULL) {
            throw EssentiaException(
                        "ACAudioEssentiaMtfOutput: input pool is invalid, a parent key should not have a"
                        "value in addition to child keys");
        }
        
        *s << "\n";
        
        // and then emit the yaml for all of its children, recursive call
        for (int i=0; i<(int)n->children.size(); ++i) {
            emitMtf(s, n, n->children[i], indent+"    ");
        }
    }
}

/*
template <typename StreamType>
void emitJson(StreamType* s, MtfNode* n, int indentsize, int indentincr) {
  const string indent = string(indentsize, ' ');
  *s << indent << "\"" << escapeJsonString(n->name) << "\": ";
  
  if (n->children.empty()) { // if there are no children, emit the value here
    if (n->value != NULL) {
    
      // Escape string or vector of strings values for json compatibility
      // FIXME Instead, is it possible to add an option to escape strings inside '<<'
      // implementation for Parameters themselves?
      Parameter::ParamType nodeType = (*(n->value)).type();
      if (nodeType == Parameter::STRING) {
        *s << "\"" << escapeJsonString((*(n->value)).toString()) << "\"";
      }
      else if (nodeType == Parameter::VECTOR_STRING) {
        vector<string> escaped = (*(n->value)).toVectorString();
        for (size_t i=0; i<escaped.size(); ++i) {
          escaped[i] = "\"" + escapeJsonString(escaped[i]) + "\"";
        }
        *s << escaped;
      }
      else {
        *s << *(n->value); // Parameters know how to be emitted to streams
      }
    }
    else { // you should never have this case: a key without any children or associated value
      throw EssentiaException("JsonOutput: input pool is invalid, contains key with no associated value");
    }
  }
  else {
    // we can make the assumption that this node has no value because the pool
    // doesn't not allow parent nodes to have values
    if (n->value != NULL) {
      throw EssentiaException(
          "JsonOutput: input pool is invalid, a parent key should not have a"
          "value in addition to child keys");
    }
    
    *s << "{" << _jsonN;
    
    // and then emit the json for all of its children, recursive call
    int childrensize = (int)n->children.size();
    for (int i=0; i<childrensize; ++i) {
      emitJson(s, n->children[i], indentsize + indentincr, indentincr);
      if (i < childrensize-1) {
          *s << ",";
      }
      *s << _jsonN;
    }
    
    *s << indent << "}";
  }
}
*/

void ACAudioEssentiaMtfOutput::outputMtfToStream(MtfNode& root, ostream* out) {
    for (int i=0; i<(int)root.children.size(); ++i) {
        *out << "\n";
        emitMtf(out, &root, root.children[i], "");
    }
}

/*
void outputJsonToStream(MtfNode& root, ostream* out, int indentincr) {
  *out << "{" << _jsonN;
  for (int i=0; i<(int)root.children.size(); ++i) {
    emitJson(out, root.children[i], 0, indentincr);
    if (i < (int)root.children.size()-1) {
        *out << ",";
    }
    *out << _jsonN;
  }
  *out << "}";
}
*/

void ACAudioEssentiaMtfOutput::outputToStream(ostream* out) {
    // set precision to be high enough
    out->precision(12);
    
    const Pool& p = _pool.get();
    
    // create the MtfNode Tree
    MtfNode root("doesn't matter what I put here, it's not getting emitted");
    
    // add metadata.version.essentia to the tree
    if (_writeVersion) {
        MtfNode* essentiaNode = new MtfNode("essentia");
        
        essentiaNode->value = new Parameter(essentia::version);
        
        MtfNode* versionNode = new MtfNode("version");
        versionNode->children.push_back(essentiaNode);
        
        MtfNode* metadataNode = new MtfNode("metadata");
        metadataNode->children.push_back(versionNode);
        
        root.children.push_back(metadataNode);
    }
    
    // fill the YAML tree with the values form the pool
    fillMtfTree(p, &root);
    
    /*if (_outputJSON) {
      outputJsonToStream(root, out, _indent);
  } else {*/
    outputMtfToStream(root, out);
    /*}*/
}


void ACAudioEssentiaMtfOutput::compute() {
    if (_filename == "-") {
        outputToStream(&cout);
    }
    else {
        ofstream out(_filename.c_str());
        outputToStream(&out);
        out.close();
        
        if (_doubleCheck) {
            ostringstream expected;
            outputToStream(&expected);
            
            // read the file we just wrote...
            ifstream f(_filename.c_str());
            if (!f.good()) {
                throw EssentiaException("ACAudioEssentiaMtfOutput: error when double-checking the output file; it doesn't look like it was written at all");
            }
            ostringstream written;
            // we need to compare using streambuffers or otherwise
            // the check fails on windows due to new lines
            written << f.rdbuf();
            if (written.str() != expected.str()) {
                throw EssentiaException("ACAudioEssentiaMtfOutput: error when double-checking the output file; it doesn't match the expected output");
            }
        }
    }
}

void ACAudioEssentiaMtfOutput::clearFeatures(){
std::map<std::string,ACMediaFeatures*>::iterator mf;
    /*for(mf=descmf.begin();mf!=descmf.end();mf++)
        (*mf).second = 0;//delete (*mf);*/
    descmf.clear();
}

void ACAudioEssentiaMtfOutput::clearTimedFeatures(){
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf;
    for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++)
        (*mtf).second = 0;//delete (*mtf);
    descmtf.clear();
}

std::map<std::string,ACMediaFeatures*> ACAudioEssentiaMtfOutput::getFeatures(){
    return descmf;
}

std::map<std::string,ACMediaTimedFeature*> ACAudioEssentiaMtfOutput::getTimedFeatures(){
    return descmtf;
}

