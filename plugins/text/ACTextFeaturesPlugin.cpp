/*
 *  ACTextFeaturesPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 22/10/10
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

#include "ACTextFeaturesPlugin.h"
#include "ACTextSTLData.h"

#include "CLucene.h"
//#include "CLucene/util/Reader.h"
//#include "CLucene/util/Misc.h"
//#include "CLucene/util/dirent.h"
#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/StdHeader.h"

#include "utf8.h"
#include <vector>
#include <string>

#ifdef _CL_HAVE_NAMESPACES
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;
#endif

#include "frenchStopWord.h"

void ACTextFeaturesPlugin::createIndex(void) {


    IndexWriter *lIndex;
    bool clearIndex=true;//if we want to keep the index
    if ( !clearIndex && IndexReader::indexExists(pathIndex.c_str()) ){
        if ( IndexReader::isLocked(pathIndex.c_str()) ){
            printf("Index was locked... unlocking it.\n");
            IndexReader::unlock(pathIndex.c_str());
        }
        lIndex = _CLNEW IndexWriter( pathIndex.c_str(), an, false);
    }else{
        lIndex = _CLNEW IndexWriter( pathIndex.c_str() ,an, true);
    }
    lIndex->setMaxFieldLength(IndexWriter::DEFAULT_MAX_FIELD_LENGTH);
    lIndex->optimize();
    lIndex->close();

    _CLDELETE(lIndex);
    //	sleep(1);
    mIndex= _CLNEW ACIndexModifier( pathIndex.c_str(), an, false);

    //IndexReader* reader = IndexReader::open(pathIndex);
    //int64_t ver = mIndex->getCurrentVersion(pathIndex);

    //printf("Max Docs: %d\n", reader->maxDoc() );
    //printf("Num Docs: %d\n", reader->numDocs() );
}

void ACTextFeaturesPlugin::addMedia(ACText* theMedia) {
    ACMediaData* media_data = theMedia->getMediaData();
    // make a new, empty document
    ACTextSTLDataContainer* text_data = dynamic_cast<ACTextSTLDataContainer*>(media_data->getData());

    int lIndex=mIndex->docCount();
    string test("test");
    string* lData=static_cast<string*>(text_data->getData());
    Document* doc = _CLNEW Document();
    TCHAR *strPath=new TCHAR[theMedia->getFileName().size()+1];
    //cout << (*lData);

#if defined(_ASCII)
    strcpy(strPath,theMedia->getFileName().c_str());
#else
    mc_utf8towcs(strPath,theMedia->getFileName().c_str(),theMedia->getFileName().size()+1);
#endif
    strPath[theMedia->getFileName().size()]=0;


    TCHAR *strData=new TCHAR[lData->size()+1];

#if defined(_ASCII)
    strcpy(strData,lData->c_str());
#else
    mc_utf8towcs(strData,lData->c_str(),lData->size()+1);
#endif
    strData[lData->size()]=0;
    //cout << strData << "\n";
    //doc->add( *_CLNEW Field(_T("path"),strPath, Field::STORE_YES | Field::INDEX_UNTOKENIZED ) );
    doc->add( *_CLNEW Field(_T("contents"),strData, Field::STORE_YES | Field::INDEX_TOKENIZED | Field::TERMVECTOR_WITH_POSITIONS) );
    mIndex->addDocument( doc );
    mIndex->flush();
    _CLDELETE(doc);
    theMedia->setDocIndex(lIndex);
    delete strPath;
    delete strData;
    strPath=NULL;
    strData=NULL;


}

void ACTextFeaturesPlugin::closeIndex(void) {
    //_CLDELETE(reader);
    //reader->close();
    if (mIndex!=NULL){
        mIndex->optimize();
        _CLDELETE(mIndex);
    }
}

ACTextFeaturesPlugin::ACTextFeaturesPlugin() : ACFeaturesPlugin(), ACPreProcessPlugin() {
    this->mMediaType = MEDIA_TYPE_TEXT;
    //this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Text Features";
    this->mDescription = "Text Features plugin";
    this->mId = "";
    this->mIndex=NULL;
    this->mIndexValid=false;
    //an=new CL_NS(analysis)::SimpleAnalyzer();//
    an =new CL_NS(analysis)::StopAnalyzer(FRENCH_STOP_WORDS);


#if defined(__APPLE__)
    //pathIndex=string("/Users/ravet/Desktop/navimed/textMining/lucene/testIndex/");
    pathIndex=string("../temp");
#elif defined (__WIN32__)

    pathIndex=string("..\temp");
#else
    pathIndex=string("../temp");
#endif
    createIndex();

}

ACTextFeaturesPlugin::~ACTextFeaturesPlugin() {
    clearIndexTerm(indexTerms);
    closeIndex();
    if (an!=0)
        delete an;
}

ACFeatureDimensions ACTextFeaturesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Term Frequency-Inverse Document Frequency"] = 1;
    return featureDimensions;
}

std::vector<TCHAR*> ACTextFeaturesPlugin::indexTermsExtraction(){
    std::vector<TCHAR*> desc;

    TCHAR**  charTerms;
    int nbTerms;
    extractIndexTerms(charTerms,nbTerms,mIndex);
    for (int i=0;i<nbTerms;i++){
        desc.push_back(charTerms[i]);
    }
    delete charTerms;
    return desc;
}

std::vector<float> ACTextFeaturesPlugin::indexIdfExtraction(){
    std::vector<float> ret;
    int nbDoc=mIndex->docCount();
    int nbTerms=indexTerms.size();
    for (int i=0;i<nbTerms;i++){
        Term *tempTerm = _CLNEW Term(_T("contents"),indexTerms[i]);
        int tempInt=mIndex->docFreq(tempTerm);
        if (mIndex->docFreq(tempTerm)>0)
        {
            ret.push_back(log((float)nbDoc/mIndex->docFreq(tempTerm)));
#if defined(_ASCII)
            cout<<indexTerms[i]<<"\tdf:"<<tempInt<<"\tidf:"<<log((float)nbDoc/mIndex->docFreq(tempTerm))<<"\n";
#else
            char *tempChar=new char[1024];
            mc_wcstoutf8(tempChar,indexTerms[i],1024);
            cout<<tempChar<<"\tdf:"<<tempInt<<"\tidf:"<<log((float)nbDoc/mIndex->docFreq(tempTerm))<<"\n";
            delete tempChar;
#endif

        }
        else
            ret.push_back(0.f);

        _CLDELETE( tempTerm);
    }
    nbDoc=nbDoc;
    return ret;
}

ACMediaFeatures* ACTextFeaturesPlugin::tfCalculate(ACText* pMedia){
    std::vector<float> featureTest;
    int nbTerms=indexTerms.size();
    //TCHAR**  charTerms= new TCHARPTR[nbTerms];
    //for (int i=0;i<nbTerms;i++)
    //	charTerms[i]=indexTerms[i];

    FeaturesVector  tfValues(true,FeaturesVector::Cosinus);
    try{
        extractLuceneFeature(featureTest,pMedia->getDocIndex(),mIndex,indexTerms,nbTerms);
        //for (int i=0;i<nbTerms;i++)
        //	charTerms[i]=NULL;
        //delete charTerms;
        //charTerms=NULL;
        if (indexIdf.size()!=nbTerms)
            return NULL;
        for (int i=0;i<nbTerms;i++)
        {
            tfValues.push_back(featureTest[i]*indexIdf[i]);
            //char *tempChar=new char[wcslen(indexTerms[i])+2];
            //mc_wcstoutf8(tempChar,indexTerms[i],wcslen(indexTerms[i])+2);
            //wprintf(_T("%s"),indexTerms[i]);
            //delete tempChar;
        }

    }
    catch(CLuceneError e){
        for (int i=0;i<nbTerms;i++){
            tfValues.push_back(0.f);

        }
    }
    ACMediaFeatures* desc=new ACMediaFeatures(tfValues,"Term Frequency-Inverse Document Frequency");
    desc->setNeedsNormalization(1);
    return desc;
}

std::vector<ACMediaFeatures*> ACTextFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    ACText* lMedia=(ACText*)theMedia;
    addMedia(lMedia);
    //if (indexTerms.size()==0){
    //create the vector with all terms
    //indexTerms=indexTermsExtraction();
    //indexIdf=indexIdfExtraction();
    //mIndexValid=true;
    //}
    //	else
    //		mIndexValid=false;
    std::vector<ACMediaFeatures*> desc;
    FeaturesVector descVect(true,FeaturesVector::Cosinus);
    descVect.push_back(1.f);
    ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,"Term Frequency-Inverse Document Frequency");
    desc.push_back(descFeat);
    //desc.push_back(this->tfCalculate(lMedia));
    return desc;
}

void ACTextFeaturesPlugin::clearIndexTerm(std::vector<TCHAR*> &pIndexTerms){
    for (int i=0;i<pIndexTerms.size();i++){
        _CLDELETE(pIndexTerms[i]);
    }
    pIndexTerms.clear();
}





preProcessInfo ACTextFeaturesPlugin::update(ACMedias media_library){

    clearIndexTerm(indexTerms);
    indexTerms=indexTermsExtraction();
    indexIdf.clear();
    indexIdf=indexIdfExtraction();
    mIndexValid=true;
    return NULL;
}

std::vector<ACMediaFeatures*> ACTextFeaturesPlugin::apply(preProcessInfo info,ACMedia* theMedia){

    bool flag= false;
    std::vector<ACMediaFeatures*> desc;
    while (flag==false){
        try{
            ACText* lMedia=(ACText*)theMedia;

            cout <<theMedia->getFileName()<<"id"<<lMedia->getDocIndex()<<endl;
            desc.push_back(this->tfCalculate(lMedia));
            flag=true;
        }
        catch (CLuceneError e) {
            cout<<"number of doc:"<<mIndex->docCount()<<endl;
            FeaturesVector descVect(true,FeaturesVector::Cosinus);
            descVect.push_back(1.f);
            ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,"Term Frequency-Inverse Document Frequency");
            desc.push_back(descFeat);

        }
    }

    return desc;

}


void ACTextFeaturesPlugin::freePreProcessInfo(preProcessInfo &info){

}

