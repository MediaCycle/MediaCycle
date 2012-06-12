/*
 *  ArchipelMediaFactory.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 8/06/12
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ArchipelMediaFactory.h"
#include "ArchipelText.h"

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;

#include <string>

using namespace std;

ArchipelMediaFactory::ArchipelMediaFactory():ACMediaFactory(){}

ArchipelMediaFactory::~ArchipelMediaFactory(){}

ACMedia* ArchipelMediaFactory::create(string file_ext){
	boost::to_lower(file_ext);
	filext::iterator iter = used_file_extensions.find(file_ext);
	if( iter == used_file_extensions.end() ) {
		return 0;
	}
	ACMediaType m = iter->second;
	return ArchipelMediaFactory::create(m);
}


ACMedia* ArchipelMediaFactory::create(ACMediaType media_type){
	switch (media_type) {
		case MEDIA_TYPE_TEXT:
#if defined (SUPPORT_TEXT)
			return new ArchipelText();
#endif //defined (SUPPORT_TEXT)
			break;
		default:
			return ACMediaFactory::create(media_type);
			break;
	}
	return 0;
}