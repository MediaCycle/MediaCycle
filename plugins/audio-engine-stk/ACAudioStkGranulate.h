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

#ifndef ACAudioStkACAudioStkGranulate_H
#define ACAudioStkACAudioStkGranulate_H

#include <Granulate.h>

class ACAudioStkGranulate: public stk::Granulate
{
 public:
  //! Default constructor.
  ACAudioStkGranulate( void );

  //! Constructor taking input audio file and number of voices arguments.
  ACAudioStkGranulate( unsigned int nVoices, std::string fileName, bool typeRaw = false );

  //! Class destructor.
  virtual ~ACAudioStkGranulate( void );

  //! Get the number of simultaneous grain "voices" to use.
  /*!
    Multiple grains are offset from one another in time by grain
    duration / nVoices.  For this reason, it is best to set the grain
    parameters before calling this function (during initialization).
  */
  unsigned int getVoices();

  //! Get the stretch factor used for grain playback (1 - 1000).
  /*!
    Granular synthesis allows for time-stetching without affecting
    the original pitch of a sound.  A stretch factor of 4 will produce
    a resulting sound of length 4 times the orignal sound.  The
    default parameter of 1 produces no stretching.
  */
  unsigned int getStretch();

  //! Get global grain parameters used to determine individual grain settings.
  /*!
    Each grain is defined as having a length of \e duration
    milliseconds which must be greater than zero.  For values of \e
    rampPercent (0 - 100) greater than zero, a linear envelope will be
    applied to each grain.  If \e rampPercent = 100, the resultant
    grain "window" is triangular while \e rampPercent = 50 produces a
    trapezoidal window.  In addition, each grain can have a time delay
    of length \e delay and a grain pointer increment of length \e
    offset, which can be negative, before the next ramp onset (in
    milliseconds).  The \e offset parameter controls grain pointer
    jumps between enveloped grain segments, while the \e delay
    parameter causes grain calculations to pause between grains.  The
    actual values calculated for each grain will be randomized by a
    factor set using the setRandomFactor() function.
  */

  unsigned int getDuration();
  void setDuration( unsigned int duration = 30 );
  unsigned int getRampPercent();
  void setRampPercent( unsigned int rampPercent = 50 );
  int getOffset();
  void setOffset( int offset = 0 );
  unsigned int getDelay();
  void setDelay( unsigned int delay = 0 );

  //! This factor is used when setting individual grain parameters (0.0 - 1.0).
  /*!
    This random factor is applied when all grain state durations
    are calculated.  If set to 0.0, no randomness occurs.  When
    randomness = 1.0, a grain segment of length \e duration will be
    randomly augmented by up to +- \e duration seconds (i.e., a 30
    millisecond length will be augmented by an extra length of up to
    +30 or -30 milliseconds).
   */
  stk::StkFloat getRandomFactor();

};

#endif
