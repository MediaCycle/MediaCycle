/**
 * @brief Derived STK granular synthesis class with more setters/getters.
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

#include "ACAudioStkGranulate.h"

using namespace stk;

ACAudioStkGranulate :: ACAudioStkGranulate( void )
    : stk::Granulate()
{
}

ACAudioStkGranulate :: ACAudioStkGranulate( unsigned int nVoices, std::string fileName, bool typeRaw )
    : stk::Granulate(nVoices, fileName, typeRaw)
{
}

ACAudioStkGranulate :: ~ACAudioStkGranulate( void )
{
}

unsigned int ACAudioStkGranulate :: getStretch()
{
    return gStretch_;
}

unsigned int ACAudioStkGranulate :: getDuration()
{
    return gDuration_;
}

void ACAudioStkGranulate :: setDuration( unsigned int duration )
{
    gDuration_ = duration;
    if ( gDuration_ == 0 ) {
        gDuration_ = 1;
        oStream_ << "ACAudioStkGranulate::setDuration: duration argument cannot be zero ... setting to 1 millisecond.";
        handleError( StkError::WARNING );
    }
}

unsigned int ACAudioStkGranulate :: getRampPercent()
{
    return gRampPercent_;
}

void ACAudioStkGranulate :: setRampPercent( unsigned int rampPercent )
{
    gRampPercent_ = rampPercent;
    if ( gRampPercent_ > 100 ) {
        gRampPercent_ = 100;
        oStream_ << "ACAudioStkGranulate::setRampPercent: rampPercent argument cannot be greater than 100 ... setting to 100.";
        handleError( StkError::WARNING );
    }
}

int ACAudioStkGranulate :: getOffset()
{
    return gOffset_;
}

void ACAudioStkGranulate :: setOffset( int offset )
{
    gOffset_ = offset;
}

unsigned int ACAudioStkGranulate :: getDelay()
{
    return gDelay_;
}

void ACAudioStkGranulate :: setDelay( unsigned int delay )
{
    gDelay_ = delay;
}

stk::StkFloat ACAudioStkGranulate :: getRandomFactor()
{
    return gRandomFactor_;
}

unsigned int ACAudioStkGranulate :: getVoices( )
{
    return grains_.size();
}
