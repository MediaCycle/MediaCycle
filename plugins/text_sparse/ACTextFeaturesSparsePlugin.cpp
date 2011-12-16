/*
 *  ACTextFeaturesSparsePlugin.cpp
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

#include "ACTextFeaturesSparsePlugin.h"

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


using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;

#include "frenchStopWord.h"

void ACTextFeaturesSparsePlugin::createIndex(void) {
	

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

void ACTextFeaturesSparsePlugin::addMedia(ACMediaData* text_data, ACText* theMedia) {
		// make a new, empty document
	
	int lIndex=mIndex->docCount();
	string test("test");
	string* lData=static_cast<string*>(text_data->getData());
	Document* doc = _CLNEW Document();
	wchar_t *strPath=new wchar_t[text_data->getFileName().size()+1];
	//cout << (*lData);
	
#if defined(_ASCII)
	strcpy(strPath,text_data->getFileName().c_str());
#else
	mc_utf8towcs(strPath,text_data->getFileName().c_str(),text_data->getFileName().size()+1);
#endif
	strPath[text_data->getFileName().size()]=0;
	
	
	wchar_t *strData=new wchar_t[lData->size()+1];
	
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
	_CLDELETE(doc);
	theMedia->setDocIndex(lIndex);
	delete strPath;
	delete strData;
	strPath=NULL;
	strData=NULL;
	
	
}

void ACTextFeaturesSparsePlugin::closeIndex(void) {
	//_CLDELETE(reader);
	//reader->close();
	if (mIndex!=NULL){ 
		mIndex->optimize();
		_CLDELETE(mIndex);
	}
}

ACTextFeaturesSparsePlugin::ACTextFeaturesSparsePlugin() {
    this->mMediaType = MEDIA_TYPE_TEXT;
    //this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "TextFeaturesSparse";
    this->mDescription = "TextFeatures with Sparse Matrix plugin";
    this->mId = "";
	this->mIndex=NULL;
	this->mIndexValid=false;
	//an=new lucene::analysis::SimpleAnalyzer();//
	an =new lucene::analysis::StopAnalyzer(FRENCH_STOP_WORDS);

	
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

ACTextFeaturesSparsePlugin::~ACTextFeaturesSparsePlugin() {
	clearIndexTerm(indexTerms);
	closeIndex();
	if (an!=0)
		delete an;
}

std::vector<wchar_t*> ACTextFeaturesSparsePlugin::indexTermsExtraction(){
	std::vector<wchar_t*> desc;
	
	wchar_t**  charTerms;
	int nbTerms;
	extractIndexTerms(charTerms,nbTerms,mIndex);
	for (int i=0;i<nbTerms;i++){
		desc.push_back(charTerms[i]);	
	}
	delete charTerms;
	return desc;
}

std::vector<float> ACTextFeaturesSparsePlugin::indexIdfExtraction(){
	std::vector<float> ret;
	int nbDoc=mIndex->docCount();
	int nbTerms=indexTerms.size();
	for (int i=0;i<nbTerms;i++){
		Term *tempTerm = _CLNEW Term(_T("contents"),indexTerms[i]);
		int tempInt=mIndex->docFreq(tempTerm);
		if (mIndex->docFreq(tempTerm)>0)
			ret.push_back(log((float)nbDoc/mIndex->docFreq(tempTerm)));
		else
			ret.push_back(0.f);
			
		_CLDELETE( tempTerm);
	}
	nbDoc=nbDoc;
	return ret;
}

vector<ACMediaFeatures*> ACTextFeaturesSparsePlugin::tfCalculate(ACText* pMedia){
	vector<ACMediaFeatures*> desc;
	std::vector<float> featureTest;
	int nbTerms=indexTerms.size();
	//wchar_t**  charTerms= new TCHARPTR[nbTerms];
	//for (int i=0;i<nbTerms;i++)
	//	charTerms[i]=indexTerms[i];			
	extractLuceneFeature(featureTest,pMedia->getDocIndex(),mIndex,indexTerms,nbTerms);
	//for (int i=0;i<nbTerms;i++)
	//	charTerms[i]=NULL;
	//delete charTerms;
	//charTerms=NULL;
	std::vector<float> nbTotTerms,tfValues,tfIndex;
	if (indexIdf.size()!=nbTerms)
		return desc;
	nbTotTerms.push_back(nbTerms);
	int nbFeat=0;
	int cptZero=0;
	for (int i=0;i<nbTerms;i++)		
	{
		if ((featureTest[i]!=0.f)&&(indexIdf[i]!=0.f)){
			tfValues.push_back(featureTest[i]*indexIdf[i]);
			tfIndex.push_back((float)i);
			nbFeat++;
			char *tempChar=new char[wcslen(indexTerms[i])+2];		
			mc_wcstoutf8(tempChar,indexTerms[i],wcslen(indexTerms[i])+2);
			//wprintf(_T("%s"),indexTerms[i]);
			//cout <<tempChar<<"\t"<<*(tfIndex.end()-1)<<"\t"<<*(tfValues.end()-1)<<endl;
			delete tempChar;
		}
		else {
			cptZero++;
		}

		
	}
	if (tfIndex.size()==0)
	{
		tfIndex.push_back(0);
		tfValues.push_back(0);		
	}
	ACMediaFeatures* desc0=new ACMediaFeatures(nbTotTerms,"Term Frequency-Inverse Document Frequency: number of terms");
	desc0->setNeedsNormalization(0);
	ACMediaFeatures* desc1=new ACMediaFeatures(tfIndex,"Term Frequency-Inverse Document Frequency: Sparse Matrix index");
	desc1->setNeedsNormalization(0);	
	ACMediaFeatures* desc2=new ACMediaFeatures(tfValues,"Term Frequency-Inverse Document Frequency: Sparse Matrix Values");
	desc2->setNeedsNormalization(1);
	cout<<pMedia->getFileName()<<"\t"<<nbTerms<<"\t"<<tfIndex.size()<<"\t"<<tfValues.size()<<endl;
	desc.push_back(desc0);
	desc.push_back(desc1);
	desc.push_back(desc2);
	return desc;
}

std::vector<ACMediaFeatures*> ACTextFeaturesSparsePlugin::calculate(ACMediaData* text_data, ACMedia* theMedia, bool _save_timed_feat) {	
	ACText* lMedia=(ACText*)theMedia;
	addMedia(text_data,lMedia);
	if (indexTerms.size()==0){
		//create the vector with all terms
		indexTerms=indexTermsExtraction();		
		indexIdf=indexIdfExtraction();
		mIndexValid=true;
	}		
	else 
		mIndexValid=false;
	//sleep(1);
	std::vector<ACMediaFeatures*> desc;
	std::vector<float> descVect;
	descVect.push_back(1.f);
	ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,"Term Frequency-Inverse Document Frequency");
	desc.push_back(descFeat);
	//desc.push_back(this->tfCalculate(lMedia));
	return desc;
}

void ACTextFeaturesSparsePlugin::clearIndexTerm(std::vector<wchar_t*> &pIndexTerms){
	for (int i=0;i<pIndexTerms.size();i++){
		_CLDELETE(pIndexTerms[i]);
	}
	pIndexTerms.clear();
}





preProcessInfo ACTextFeaturesSparsePlugin::update(std::vector<ACMedia*> media_library){
	
	clearIndexTerm(indexTerms);
	indexTerms=indexTermsExtraction();
	indexIdf.clear();
	indexIdf=indexIdfExtraction();
	mIndexValid=true;
	return NULL;
}

std::vector<ACMediaFeatures*> ACTextFeaturesSparsePlugin::apply(preProcessInfo info,ACMedia* theMedia){
	//usleep(100000);
	
	ACText* lMedia=(ACText*)theMedia;
	
	cout <<theMedia->getFileName()<<endl;
	std::vector<ACMediaFeatures*> desc;
	desc=this->tfCalculate(lMedia);
	
	return desc;

}


void ACTextFeaturesSparsePlugin::freePreProcessInfo(preProcessInfo &info){
	
}

