/*
 *  ACIndexModifier.h
 *  clucene
 *
 *  @author Thierry Ravet
 *  @date 19/11/10
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

#ifndef _ACINDEXMODIFIER_H
#define	_ACINDEXMODIFIER_H

#include "CLucene/index/IndexModifier.h" 

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif
#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/index/Term.h"
#include "CLucene/index/IndexWriter.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/index/TermVector.h"

using namespace lucene::analysis;
using namespace lucene::index;
using namespace lucene::util;
using namespace lucene::search;
using namespace std;
CL_NS_DEF(index)


class ACIndexModifier : public IndexModifier {
public:
	
	ACIndexModifier(const char* dirName, Analyzer* analyzer, bool create);
	~ACIndexModifier(void);
//	bool getTermFreqVectors(int32_t docNumber, Array<TermFreqVector*>& result);
	
	CL_NS(util)::ArrayBase<TermFreqVector*>* getTermFreqVectors(int32_t docNumber); 
	TermFreqVector* getTermFreqVector(int32_t docNumber, const TCHAR* field);
	int32_t docFreq(const Term* t);
	TermPositions* termPositions();
	TermPositions* termPositions(Term* term);
};

CL_NS_END

#endif