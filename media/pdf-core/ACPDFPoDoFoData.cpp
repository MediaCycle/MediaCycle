/**
 * @brief PDF data and PoDoFo::PdfDocument container, implemented with PoDoFo
 * @author Christian Frisson
 * @date 27/05/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include "ACPDFPoDoFoData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

ACPDFPoDoFoData::ACPDFPoDoFoData() : ACPDFData() {
    pdf_ptr = 0;
    width = 0;
    height = 0;
}

ACPDFPoDoFoData::~ACPDFPoDoFoData() {
    /// Done by ~ACPDFPoDoFoDataContainer, pdf_ptr passed with ACPDFPoDoFoData::setData
//    if (pdf_ptr != 0){
//        delete pdf_ptr;
//    }
	pdf_ptr = 0;
}

bool ACPDFPoDoFoData::readData(string _fname){
	if(_fname=="") return false;
	pdf_ptr = new PoDoFo::PdfMemDocument( _fname.c_str() );		
	if( !pdf_ptr ) {
		cerr << "<ACPDFData::readData> Could not read document from file " << _fname << endl;
		return false;
	}

    typedef std::pair<double,double> Format;

    PoDoFo::PdfPage*  curPage;
    int	pgCount = this->getNumberOfPages();
    std::map<  Format , int > sizes;
    std::map<  Format , int >::iterator sIt;
    PoDoFo::PdfRect  rect;
    for ( int pg=0; pg<pgCount; pg++ )
    {
        curPage = pdf_ptr->GetPage( pg );
        rect = curPage->GetMediaBox();
        Format s( rect.GetWidth() - rect.GetLeft(), rect.GetHeight() - rect.GetBottom());
        sIt = sizes.find(s);
        if(sIt == sizes.end())
            sizes.insert(std::pair<Format,int>(s,1));
        else
            ++(sIt->second);
    }

    Format format;
//    std::stringstream ss;
    if(sizes.size() == 1)
    {
        format = sizes.begin()->first;
//        ss << format.first << " x " << format.second << " pts"  ;
//        format_unit = "pts";
    }
    else
    {
        // We're looking for the most represented format
        int max=0;
        for(sIt = sizes.begin();sIt != sizes.end(); ++sIt)
        {
            if(sIt->second > max)
            {
                max = sIt->second;
                format = sIt->first;
            }
        }
//        ss << format.first << " x " << format.second << " pts "<<std::string(sizes.size(), '*');
//        format_unit = std::string(sizes.size(), '*');
    }
    this->width = format.first;
    this->height = format.second;

    this->data = new ACPDFPoDoFoDataContainer();
    ((ACPDFPoDoFoDataContainer*)data)->setData(pdf_ptr);

	return true;
}

std::string ACPDFPoDoFoData::getAuthor(){
    if(pdf_ptr){
        if( !pdf_ptr->GetInfo() ){
            std::cerr << "ACPDFPoDoFoData: no info dictionary in this PDF file!" << std::endl;
            return "";
        }
        else
            return pdf_ptr->GetInfo()->GetAuthor().GetStringUtf8();
    }
    else
        return "";
}

std::string ACPDFPoDoFoData::getCreator(){
    if(pdf_ptr){
        if( !pdf_ptr->GetInfo() ){
            std::cerr << "ACPDFPoDoFoData: no info dictionary in this PDF file!" << std::endl;
            return "";
        }
        else
            return pdf_ptr->GetInfo()->GetCreator().GetStringUtf8();
    }
    else
        return "";
}

std::string ACPDFPoDoFoData::getSubject(){
    if(pdf_ptr){
        if( !pdf_ptr->GetInfo() ){
            std::cerr << "ACPDFPoDoFoData: no info dictionary in this PDF file!" << std::endl;
            return "";
        }
        else
            return pdf_ptr->GetInfo()->GetSubject().GetStringUtf8();
    }
    else
        return "";
}

std::string ACPDFPoDoFoData::getTitle(){
    if(pdf_ptr){
        if( !pdf_ptr->GetInfo() ){
            std::cerr << "ACPDFPoDoFoData: no info dictionary in this PDF file!" << std::endl;
            return "";
        }
        else
            return pdf_ptr->GetInfo()->GetTitle().GetStringUtf8();
    }
    else
        return "";
}

std::string ACPDFPoDoFoData::getKeywords(){
    if(pdf_ptr){
        if( !pdf_ptr->GetInfo() ){
            std::cerr << "ACPDFPoDoFoData: no info dictionary in this PDF file!" << std::endl;
            return "";
        }
        else
            return pdf_ptr->GetInfo()->GetKeywords().GetStringUtf8();
    }
    else
        return "";
}

int ACPDFPoDoFoData::getNumberOfPages(){
    if(pdf_ptr){
        return pdf_ptr->GetPageCount();
    }
    else
        return 0;
}
