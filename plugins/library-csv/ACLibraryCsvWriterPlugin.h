/**
 * @brief A plugin that allows to save media libraries as csv files.
 * @author Christian Frisson
 * @date 20/06/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#ifndef _ACLibraryCsvWriterPlugin_H
#define	_ACLibraryCsvWriterPlugin_H

#include <MediaCycle.h>
#include <iostream>
#include <sstream>

class ACLibraryCsvWriterPlugin : public ACMediaLibraryWriterPlugin {
public:
    ACLibraryCsvWriterPlugin();
    ~ACLibraryCsvWriterPlugin();

    virtual bool openLibrary(std::string filepath);
    virtual bool closeLibrary(std::string filepath="");
    virtual bool isLibraryOpened(std::string filepath="");
    virtual std::vector<std::string> fileFormats();
	
    virtual bool saveLibraryMetadata();
    virtual bool saveMedia(ACMedia* media);

    void withMediaId();

protected:
    std::ofstream file;
    bool with_media_id;
};
#endif	/* _ACLibraryCsvWriterPlugin_H */
