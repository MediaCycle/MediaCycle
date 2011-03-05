/*
 *  ACMediaFactoryGenerator.cpp
 *  MediaCycle
 *
 *  Created by Christian Frisson 04/03/11.
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

#include "ACMediaFactory.h"
#include "ACMediaTypes.h"

#include <iostream>
#include <string>

#include <fstream>

using namespace std;

int main(int argc, char **argv){
	
	// initialize available extensions
	ACMediaFactory::getInstance();
	ofstream generated_cpp_file;
	std::string file = std::string(argv[1]);
	std::cout << "Updating file: '" << argv[1] << "'" << std::endl;
	generated_cpp_file.open(argv[1]);
	if (!(generated_cpp_file.is_open()))
		std::cout << "Can't open file..." << std::endl;
	else {
		generated_cpp_file << "#include \"ACMediaFactory.h\"" << endl;
		generated_cpp_file << "const filext::value_type _mini[] = {";
		filext avail_ext = ACMediaFactory::getInstance()->getAvailableFileExtensions();
		filext::iterator avail_iter = avail_ext.begin();
		for(;avail_iter!=avail_ext.end();avail_iter++){
			mediaTypeToEnumNameConverter::const_iterator iterm = mediaTypeToEnumName.find(avail_iter->second);
			if( avail_iter != avail_ext.begin() )
				generated_cpp_file << ", \\";
			if( iterm != mediaTypeToEnumName.end() ) {
				generated_cpp_file << endl << "filext::value_type(\"" << avail_iter->first << "\", " << iterm->second << ")";
			}
		}
		generated_cpp_file << endl << "};" << endl << "filext ACMediaFactory::available_file_extensions(_mini, _mini + sizeof _mini / sizeof *_mini);" << endl; 
		generated_cpp_file << endl;
		generated_cpp_file << "const filext::value_type _uini[] = {";
		filext unchk_ext = ACMediaFactory::getInstance()->getUncheckedFileExtensions();
		filext::iterator unchk_iter = unchk_ext.begin();
		for(;unchk_iter!=unchk_ext.end();unchk_iter++){
			mediaTypeToEnumNameConverter::const_iterator iterm = mediaTypeToEnumName.find(unchk_iter->second);
			if( unchk_iter != unchk_ext.begin() )
				generated_cpp_file << ", \\";
			if( iterm != mediaTypeToEnumName.end() ) {
				generated_cpp_file << endl << "filext::value_type(\"" << unchk_iter->first << "\", " << iterm->second << ")";
			}
		}
		generated_cpp_file << endl << "};" << endl << "filext ACMediaFactory::unchecked_file_extensions(_uini, _uini + sizeof _uini / sizeof *_uini);" << endl; 
		generated_cpp_file.close();
	}
	return 0;
}