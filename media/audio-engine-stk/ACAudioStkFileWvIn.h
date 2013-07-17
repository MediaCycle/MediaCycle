/**
 * @brief Derived STK audio file input class with more setters/getters.
 * @author Christian Frisson
 * @date 25/03/2013
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

#ifndef ACAudioStkFileWvIn_H
#define ACAudioStkFileWvIn_H

#include "FileWvIn.h"

class ACAudioStkFileWvIn : public stk::FileWvIn
{
public:
    //! Default constructor.
    ACAudioStkFileWvIn( unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 )
        : stk::FileWvIn( chunkThreshold,chunkSize ){}

    //! Overloaded constructor for file input.
    /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
    ACAudioStkFileWvIn( std::string fileName, bool raw = false, bool doNormalize = true,
                        unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 )
        : stk::FileWvIn( fileName,raw,doNormalize,chunkThreshold,chunkSize ){}

    //! Class destructor.
    virtual ~ACAudioStkFileWvIn( void ){}

    stk::StkFloat getRate(){return rate_;}
};
#endif
