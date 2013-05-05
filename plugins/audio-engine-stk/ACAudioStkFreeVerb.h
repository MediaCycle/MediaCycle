/**
 * @brief Derived STK FreeVerb class with more setters/getters.
 * @author Christian Frisson
 * @date 24/04/2013
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

#ifndef ACAudioStkFreeVerb_H
#define ACAudioStkFreeVerb_H

#include "FreeVerb.h"

class ACAudioStkFreeVerb : public stk::FreeVerb
{
public:
    //! FreeVerb Constructor
    /*!
      Initializes the effect with default parameters. Note that these defaults
      are slightly different than those in the original implementation of
      FreeVerb [Effect Mix: 0.75; Room Size: 0.75; Damping: 0.25; Width: 1.0;
      Mode: freeze mode off].
    */
    ACAudioStkFreeVerb()
        : stk::FreeVerb(){}

    //! Get the effect mix [0 = mostly dry, 1 = mostly wet].
    stk::StkFloat getEffectMix(){return effectMix_;}
};
#endif
