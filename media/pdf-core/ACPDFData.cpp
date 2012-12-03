/*
 *  ACPDFData.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 27/05/2011
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

#include "ACPDFData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

ACPDFData::ACPDFData() { 
	this->init();
}

ACPDFData::ACPDFData(std::string _fname){ 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

void ACPDFData::init() {
	media_type = MEDIA_TYPE_PDF;
	pdf_ptr = 0;
}

ACPDFData::~ACPDFData() {
	if (pdf_ptr != 0) delete pdf_ptr;
	pdf_ptr = 0;
}

bool ACPDFData::readData(string _fname){
	if(_fname=="") return false;
	pdf_ptr = new PoDoFo::PdfMemDocument( _fname.c_str() );		
	if( !pdf_ptr ) {
		// Either the PDF does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACPDFData::readData> Could not read document from file " << _fname << endl;
		return false;
	}
	return true;
}

void ACPDFData::setData(PoDoFo::PdfMemDocument* _data){
	pdf_ptr = new PoDoFo::PdfMemDocument();
	pdf_ptr->Append(*_data);
	if( !pdf_ptr ) {
		// Either the PDF does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACPDFData::setData> Could not set data" << endl;
	}	
}
