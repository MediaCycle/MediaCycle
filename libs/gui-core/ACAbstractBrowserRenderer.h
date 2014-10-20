/**
 * @brief Abstract class to define a browser renderer
 * @author Christian Frisson
 * @date 21/04/2014
 * @copyright (c) 2014 – UMONS - Numediart
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

#ifndef ACAbstractBrowserRenderer_H
#define ACAbstractBrowserRenderer_H

class ACColor {
public:
    ACColor(float _r, float _g, float _b, float _a)
        :r(_r),g(_g),b(_b),a(_a)
    {
    }
public:
    float r,g,b,a;
};

class ACAbstractBrowserRenderer {
public:
    ACAbstractBrowserRenderer(){}
    ~ACAbstractBrowserRenderer(){}

public:
    virtual void changeNodeColor(int _node, ACColor _color)=0;
    virtual bool addLink(long in, long out, float width, ACColor color = ACColor(1.0f,1.0f,1.0f,1.0f))=0;
    virtual bool removeLinks()=0;
};

#endif // ACAbstractBrowserRenderer_H
