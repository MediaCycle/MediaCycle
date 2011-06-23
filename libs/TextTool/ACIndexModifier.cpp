/*
 *  ACIndexModifier.cpp
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
#include "CLucene/StdHeader.h"
#include "ACIndexModifier.h"

CL_NS_DEF(index)

//ACIndexModifier::ACIndexModifier(Directory* directory, Analyzer* analyzer, bool create) : IndexModifier( directory, analyzer, create) {
//}

ACIndexModifier::ACIndexModifier(const char* dirName, Analyzer* analyzer, bool create): IndexModifier(dirName,  analyzer, create) {
}


ACIndexModifier::~ACIndexModifier(void){
//	if (open) {
//		close();
//	}
//	if (directory!=NULL)
//		_CLDELETE(directory);
}

#ifdef OLD_CLUCENE
bool ACIndexModifier::getTermFreqVectors(int32_t docNumber, Array<TermFreqVector*>& result){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	
	return indexReader->getTermFreqVectors( docNumber, result);
}
#else
CL_NS(util)::ArrayBase<TermFreqVector*>* ACIndexModifier::getTermFreqVectors(int32_t docNumber) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->getTermFreqVectors( docNumber);
}
#endif

TermFreqVector* ACIndexModifier::getTermFreqVector(int32_t docNumber, const TCHAR* field){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->getTermFreqVector( docNumber, field);
}


//bool ACIndexModifier::isOptimized(void){
//	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
//	assureOpen();
//	createIndexReader();
//	return indexReader->isOptimized();
//}

int32_t ACIndexModifier::docFreq(const Term* t){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->docFreq(t);
}
TermPositions* ACIndexModifier::termPositions(){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->termPositions();
}
TermPositions* ACIndexModifier::termPositions(Term* term){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->termPositions(term);
}


CL_NS_END