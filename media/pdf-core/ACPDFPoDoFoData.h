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

#ifndef ACPDFPODOFODATA_H
#define ACPDFPODOFODATA_H

#include "ACPDFData.h"

#include <ostream>
#include <podofo/podofo.h>

class ACPDFPoDoFoDataContainer : public ACMediaDataContainer, public ACSpatialData {
public:
    ACPDFPoDoFoDataContainer()
        : ACMediaDataContainer(), ACSpatialData(),data(0)
    {}
    virtual ~ACPDFPoDoFoDataContainer(){
        if(data)
            delete data;
        data = 0;
    }
    void setData(PoDoFo::PdfDocument* _data){
        if(data)
            delete data;
        data = _data;
    }
    PoDoFo::PdfDocument* getData(){return data;}
protected:
    PoDoFo::PdfDocument* data;
};

class ACPDFPoDoFoData: public ACPDFData {
public:
    ACPDFPoDoFoData();
    virtual ~ACPDFPoDoFoData();
    //ACPDFPoDoFoData(std::string _fname);

    virtual bool readData(std::string _fname);
    virtual std::string getName(){return "PoDoFo PDF";}

    virtual std::string getAuthor();
    virtual std::string getCreator();
    virtual std::string getSubject();
    virtual std::string getTitle();
    virtual std::string getKeywords();
    virtual int getNumberOfPages();
    virtual int getWidth(){return width;}
    virtual int getHeight(){return height;}

protected:
    PoDoFo::PdfDocument* pdf_ptr;
    int width,height;
};

#endif // ACPDFPODOFODATA_H
