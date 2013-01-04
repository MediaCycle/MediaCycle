/**
 * @brief 3D model data and container, implemented with OSG
 * @author Xavier Siebert, Christian Frisson
 * @date 7/04/2011
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

#ifndef AC3DMODELOSGDATA_H
#define AC3DMODELOSGDATA_H

#include "AC3DModelData.h"
#include <osg/Node>

class AC3DModelOsgDataContainer : public ACMediaDataContainer, public ACSpatialData {
public:
    AC3DModelOsgDataContainer();
    virtual ~AC3DModelOsgDataContainer();
    void setData(osg::ref_ptr<osg::Node> _data){
        this->data = 0;
        this->data = _data;
    }
    osg::ref_ptr<osg::Node> getData(){return data;}
protected:
    osg::ref_ptr<osg::Node> data;
};

class AC3DModelOsgData: public AC3DModelData {
public:
    AC3DModelOsgData();
    virtual ~AC3DModelOsgData();
    virtual bool readData(std::string _fname);
    virtual bool closeFile(){}
    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();
    virtual std::string getName(){return "OSG 3D model";}
protected:
//    virtual void init();
    osg::ref_ptr<osg::Node> model_ptr;
    osg::Vec3 dimensions;
};


#endif // AC3DMODELOSGDATA_H
