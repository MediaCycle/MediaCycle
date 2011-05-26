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
#include "CLucene/clucene-config.h"
#include <iostream>
//#include <TermVector.h>

#include "utf8.h"
void getFreqTerm(Array<int32_t> &ret,std::vector<wchar_t *> termsToFind, int32_t nTerms,TermFreqVector* termsFreqBase){
	int i=0;
	ret.length = nTerms;
	ret.values = _CL_NEWARRAY(int32_t,nTerms);
	int32_t test;
	test=termsFreqBase->size();
	
	const ArrayBase< int32_t > * 	freqTargeted=termsFreqBase->getTermFrequencies ();
	const ArrayBase<const wchar_t*>* 	termTargeted=termsFreqBase->getTerms ();
	int nTermFreq=termsFreqBase->getTermFrequencies()->length;
	for (int j=0;j<nTermFreq;j++){
		Term tempTermToFind(_T("contents"),termsToFind[i]);
		Term tempTermFreq(_T("contents"),termTargeted->values[j]);
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
	}
	for (i=i;i<nTerms;i++)
		ret.values[i]=0;
}

void extractIndexTerms(wchar_t**  &outTerms,int &nbOutTerms,ACIndexModifier* inputIndex){
	TermEnum* te1 = inputIndex->terms();
	int nterms;
	for (nterms = 0; te1->next() == true; nterms++) {		
	} 	
	nbOutTerms=nterms;
	TermEnum* te = inputIndex->terms();
	outTerms=new TCHARPTR[nterms];
	for (int i = 0; te->next() == true; i++) {
		Term *tempTerm=te->term(false);
#if defined(_ASCII)
		char *chartemp=new char[strlen(te->term()->text())+1];
		strcpy(chartemp,te->term()->text());		
#else
		wchar_t *chartemp=new wchar_t[wcslen(tempTerm->text())+1];
		wcsncpy(chartemp,tempTerm->text(),wcslen(tempTerm->text())+1);
#endif
		
		outTerms[i]=chartemp;
		chartemp=NULL;
		//_CLDELETE(tempTerm);
//		_tprintf(_T("%s\n"),te->term()->text());
		/* empty */		
	} 
	_CLDELETE(te);
	_CLDELETE(te1);	
	
}
void extractLuceneFeature(std::vector<float> &output,int32_t docIndex,ACIndexModifier* inputIndex,std::vector<wchar_t*>  terms,int nbTerms){
	Array<int32_t> tf;
	const wchar_t* fieldName=_T("contents");	
		
//		_tprintf(_T("Doc n %d\n"),docIndex);		
		TermFreqVector* testVect=inputIndex->getTermFreqVector(docIndex,fieldName);
		
	getFreqTerm(tf,terms,  nbTerms,testVect);
	int sum=0;
	for (int i=0;i<nbTerms; i++) {
		sum+=tf.values[i];
		
	}
	for (int i=0;i<nbTerms; i++) {
		output.push_back((float)tf.values[i]/sum);
		char charTemp[1024];
		mc_wcstoutf8(charTemp,terms[i],1024);
//		printf(("%s\n"),charTemp);
//		wprintf(_T("\ttf:%d\tdf:%d\n"),tf.values[i],inputIndex->docFreq(&Term(_T("contents"),terms[i])));
	}
#if defined(_ASCII)
//	printf("\n");
#else
//	wprintf(_T("\n"));	
#endif
	tf.deleteValues();
	_CLDELETE(testVect);
	}
