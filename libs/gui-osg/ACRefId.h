/*
 *  ACRefId.h
 *  AudioCycle
 *
 *  @author Raphael Sebbe
 *  @date 07/11/08
 *  @copyright (c) 2008 – UMONS - Numediart
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

#ifndef __REFERENCED_H__
#define __REFERENCED_H__

#include <osg/ref_ptr>
#include <string>

class ACRefId : public osg::Referenced
	{
	public:
		int object_id;
		std::string object_name;
		int element_id;
		// contains an identifier for each object (= node)

		ACRefId(int oid=-1, std::string name="",int eid=-1) : osg::Referenced(), object_id(oid),object_name(name),element_id(eid){};
		ACRefId(const ACRefId& src) : osg::Referenced(), object_id(src.object_id),object_name(src.object_name),element_id(src.element_id){};//{object_id = src.object_id;object_name = src.object_name;}
		~ACRefId() {};
		ACRefId& operator=(ACRefId &src){this->object_id = src.getRefId();this->object_name = src.getRefName();}
		int getRefId(){return object_id;}
		std::string getRefName(){return object_name;}
		int getElementId(){return element_id;}
	};

#endif