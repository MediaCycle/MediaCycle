/*
 *  SparseMatrixOperator.h
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 21/06/11
 *  @copyright (c) 2011 – UMONS - Numediart
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


#ifndef _ACSPARSEMATRIXOPERATOR_H
#define	_ACSPARSEMATRIXOPERATOR_H
#include <vector>

typedef std::vector<const std::vector<float> *> ACSparseVector;

class SparseMatrixOperator {
public:
	static ACSparseVector& addVectors(std::vector<float> a0,std::vector<float> a1,std::vector<float> a2,std::vector<float> b0,std::vector<float> b1,std::vector<float> b2);
	static void multipleVectorByScalar(std::vector<float> &out0,std::vector<float> &out1,std::vector<float> &out2,std::vector<float> a0,std::vector<float> a1,std::vector<float> a2,float mult);
	static float norm(const std::vector<float> &a0, const std::vector<float> &a1,const std::vector<float> &a2);
	static ACSparseVector& fullToSparseVector(std::vector<float> in);
	static std::vector<float>& SparseToFullVector(ACSparseVector in);
	static void freeSparseVector(ACSparseVector &in);
	

};

#endif