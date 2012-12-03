/*
 *  ACArchipelReader.h
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 6/06/11
 *  Completed by Christian Frisson on 18/09/12.
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

#ifndef ACArchipelREADER_H
#define	ACArchipelREADER_H

#include <string>
#include <vector>
#define TIXML_USE_STL
#include <tinyxml.h>

class ACArchipelReader
{
public:
    ACArchipelReader(const std::string fileName);
    ~ACArchipelReader();
	
	bool isArchipel();
	std::vector<std::string> getIlot(void);
	std::vector<std::string> getGlossaire(void);
	std::string getText(void);
	std::string getArtist(void);
    std::string getArtistType(void); // info>type -> ex: solo
	std::string getAlbumName(void);
    std::string getReference(void); // getMediathequeReference
	std::vector<std::string> getTrackTitle(void);
	std::vector<std::string> getTrackPath(void);
	std::string getThumbPath(void);
    int getYear(void); // info>year
    std::string getMediathequeHyperlink(void); // info>link
    std::string getProducer(void); // info>producer
    std::string getExtra(void); // info>extra
protected:
	TiXmlDocument *mDoc;
	std::string mFileName;
	
};

#endif
