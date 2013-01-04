/**
 * @brief Text data and string container, implemented with the Standard Template Library (STL)
 * @author Thierry Ravet, Christian Frisson
 * @date 2/05/2011
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

#ifndef ACTEXTSTLDATA_H
#define ACTEXTSTLDATA_H

#include "ACTextData.h"
#include <cstdlib>
#include <cstring>
#include <string>

class ACTextSTLDataContainer : public ACMediaDataContainer {
public:
    ACTextSTLDataContainer() : ACMediaDataContainer(),data(0) {}
    virtual ~ACTextSTLDataContainer(){
        if(data){
            data->clear();
            delete data;
        }
        data = 0;
    }
    void setData(std::string* _data){
        if(data){
            data->clear();
            delete data;
        }
        this->data = _data;
    }
    std::string* getData(){return data;}

protected:
    std::string* data;
};

class ACTextSTLData: public ACTextData {
public:
    ACTextSTLData();
    ~ACTextSTLData();

    virtual bool readData(std::string _fname);
    virtual std::string getName(){return "STL text";}

protected:
    std::string* text_ptr;
};

#endif // ACTEXTSTLDATA_H
