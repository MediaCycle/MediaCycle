/*
 *  ACTextFeatures.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 21/10/10
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

#include "ACTextFeatures.h"

#include <string.h>
#include <CLucene/clucene-config.h>
#include <iostream>
//#include <TermVector.h>

#include <utf8.h>

using namespace std;
//using namespace lucene::analysis;
using namespace lucene::index;
//using namespace lucene::util;
//using namespace lucene::search;
const static float seuil=0.8;


void getFreqTerm(vector<int32_t> &ret,std::vector<wchar_t *> termsToFind, int32_t nTerms,TermFreqVector* termsFreqBase){
	//int i=0;
	//ret.length = nTerms;
	//ret.values = _CL_NEWARRAY(int32_t,nTerms);
	ret.clear();
	int32_t test;
	test=termsFreqBase->size();
	
	const Array<int32_t>* ptrFreq=termsFreqBase->getTermFrequencies();
	for (int i=0;i<termsToFind.size();i++){	
		int tempCpt=termsFreqBase->indexOf(termsToFind[i]);
		if (tempCpt==-1)
			ret.push_back(0);
		else {
			int tfLoc=(*ptrFreq)[tempCpt];
			ret.push_back(tfLoc);
		}


	}
	/*
	#ifdef OLD_CLUCENE
		const Array< int32_t > * freqTargeted =termsFreqBase->getTermFrequencies ();
		const wchar_t** 	termTargeted=termsFreqBase->getTerms ();
	#else
		const ArrayBase< int32_t > * 	freqTargeted=termsFreqBase->getTermFrequencies ();
		const ArrayBase<const wchar_t*>* 	termTargeted=termsFreqBase->getTerms ();
	#endif
	int nTermFreq=termsFreqBase->getTermFrequencies()->length;
	for (int j=0;j<nTermFreq;j++){
		
		Term tempTermToFind(_T("contents"),termsToFind[i]);
		#ifdef OLD_CLUCENE
			Term tempTermFreq(_T("contents"),termTargeted[j]);
#else
			wprintf(termTargeted->values[j]);
			wprintf(_T("\ttf:%d\n"),freqTargeted->values[j]);
			Term tempTermFreq(_T("contents"),termTargeted->values[j]);
		#endif
		while (tempTermToFind.compareTo(&tempTermFreq)<0){
			ret.values[i]=0;			
			i++;
			if (i==nTerms)
				break;
			tempTermToFind.set(_T("contents"),termsToFind[i]);
		}		
		if (i==nTerms)
			break;
		if (tempTermToFind.compareTo(&tempTermFreq)==0){
			ret.values[i]=freqTargeted->values[j];			
			
			i++;
			
			if (i==nTerms)
				break;					
		}	
		//int tempInd=termsFreqBase->indexOf(termsToFind[j]);
		//float tempValue=freqTargeted->values[tempInd];
		//ret.values[j]=tempValue;
#ifdef OLD_CLUCENE
		//delete termTargeted[j];
		
#else
	
#endif
	}
#ifdef OLD_CLUCENE
	for (int j=0;j<nTermFreq;j++)
//		delete termTargeted[j];
	delete termTargeted;
	
#else
	
#endif
	for (i=i;i<nTerms;i++)
		ret.values[i]=0;*/
	
}

void extractIndexTerms(wchar_t**  &outTerms,int &nbOutTerms,ACIndexModifier* inputIndex){
	TermEnum* te1 = inputIndex->terms();
	int nterms;
	
	int nbDoc=inputIndex->docCount();
	for (nterms = 0; te1->next() == true; nterms++) {		
	} 	
	TermEnum* te = inputIndex->terms();
	outTerms=new TCHARPTR[nterms];
	nterms=0;
	int cptRejectedTerms=0;
	for (int i = 0; te->next() == true; i++) {
		
		Term *tempTerm=te->term(false);

		int tempInt=inputIndex->docFreq(tempTerm);
		if ((nbDoc>4)&&((float)tempInt/nbDoc>seuil))
		{
			wchar_t *chartemp=new wchar_t[wcslen(tempTerm->text())+1];
			wcsncpy(chartemp,tempTerm->text(),wcslen(tempTerm->text())+1);
			delete chartemp;
			cptRejectedTerms++;
			continue;
			nbDoc=nbDoc;
		}

#if defined(_ASCII)
		char *chartemp=new char[strlen(te->term()->text())+1];
		strcpy(chartemp,te->term()->text());		
#else
		wchar_t *chartemp=new wchar_t[wcslen(tempTerm->text())+1];
		wcsncpy(chartemp,tempTerm->text(),wcslen(tempTerm->text())+1);
		chartemp[wcslen(tempTerm->text())]=0;
#endif
		
		outTerms[nterms]=chartemp;
		chartemp=0;
		//_CLDELETE(tempTerm);
		_tprintf(_T("%s\n"),te->term()->text());
		/* empty */	
		
		nterms++;	
	} 
	cout<<"rejected terms:"<<cptRejectedTerms<<endl;
	nbOutTerms=nterms;
	_CLDELETE(te);
	_CLDELETE(te1);	
	
}
void extractLuceneFeature(std::vector<float> &output,int32_t docIndex,ACIndexModifier* inputIndex,std::vector<wchar_t*>  terms,int nbTerms){
	vector<int32_t> tf;
	const wchar_t* fieldName=_T("contents");	
		
	//bool test =inputIndex->isOptimized();
//		_tprintf(_T("Doc n %d\n"),docIndex);		
	TermFreqVector* testVect=inputIndex->getTermFreqVector(docIndex,fieldName);
	if 	(testVect==0)
	{
		for (int i=0;i<nbTerms; i++) {
			output.push_back(0.f);
		}
		return;
		
	}
	getFreqTerm(tf,terms,  nbTerms,testVect);
	int sum=0;
	for (int i=0;i<nbTerms; i++) {
		sum+=tf[i];
		
	}
	for (int i=0;i<nbTerms; i++) {
		output.push_back((float)tf[i]/sum);
		char charTemp[1024];
		mc_wcstoutf8(charTemp,terms[i],1024);
		if (tf[i]!=0.f)
		{	
		//	cout << charTemp<<"\ttf:\t"<<tf[i]<<"\tdf:\t"<<inputIndex->docFreq(&Term(_T("contents"),terms[i]))<<"\n";
			
		}
	}
#if defined(_ASCII)
//	printf("\n");
#else
	wprintf(_T("\n"));	
#endif
	#ifdef OLD_CLUCENE
		tf.clear();
	#endif
	_CLDELETE(testVect);
	}
