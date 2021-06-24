/**
 * @brief ACAudioStkFileLoop.h
 * @author Christian Frisson
 * @date 05/05/2013
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

#ifndef STK_ACAudioStkFileLoop_H
#define STK_ACAudioStkFileLoop_H

#include "FileWvIn.h"

//namespace stk {

/***************************************************/
/*! \class ACAudioStkFileLoop
    \brief STK file looping / oscillator class.

    This class provides audio file looping functionality.  Any audio
    file that can be loaded by FileRead can be looped using this
    class.

    ACAudioStkFileLoop supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames object for
    multi-channel and/or multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995-2012.
*/
/***************************************************/

class ACAudioStkFileLoop : public stk::FileWvIn
{
 public:
  //! Default constructor.
  ACAudioStkFileLoop( unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 );

  //! Class constructor that opens a specified file.
  ACAudioStkFileLoop( std::string fileName, bool raw = false, bool doNormalize = true,
            unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024,
            bool doInt2FloatScaling = true );

  //! Class destructor.
  ~ACAudioStkFileLoop( void );

  //! Open the specified file and load its data.
  /*!
    Data from a previously opened file will be overwritten by this
    function.  An StkError will be thrown if the file is not found,
    its format is unknown, or a read error occurs.  If the file data
    is to be loaded incrementally from disk and normalization is
    specified, a scaling will be applied with respect to fixed-point
    limits.  If the data format is floating-point, no scaling is
    performed.
  */
  void openFile( std::string fileName, bool raw = false, bool doNormalize = true, bool doInt2FloatScaling = true );

  //! Close a file if one is open.
  void closeFile( void ) { stk::FileWvIn::closeFile(); }

  //! Clear outputs and reset time (file) pointer to zero.
  void reset( void ) { stk::FileWvIn::reset(); }

  //! Return the number of audio channels in the data or stream.
  unsigned int channelsOut( void ) const { return data_.channels(); }

  //! Normalize data to a maximum of +-1.0.
  /*!
    This function has no effect when data is incrementally loaded
    from disk.
  */
  void normalize( void ) { stk::FileWvIn::normalize( 1.0 ); }

  //! Normalize data to a maximum of \e +-peak.
  /*!
    This function has no effect when data is incrementally loaded
    from disk.
  */
  void normalize( stk::StkFloat peak ) { stk::FileWvIn::normalize( peak ); };

  //! Return the file size in sample frames.
  unsigned long getSize( void ) const { return data_.frames(); };

  //! Return the input file sample rate in Hz (not the data read rate).
  /*!
    WAV, SND, and AIF formatted files specify a sample rate in
    their headers.  STK RAW files have a sample rate of 22050 Hz
    by definition.  MAT-files are assumed to have a rate of 44100 Hz.
  */
  stk::StkFloat getFileRate( void ) const { return data_.dataRate(); };

  //! Set the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  void setRate( stk::StkFloat rate );

  //! Get the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  stk::StkFloat getRate(){return rate_;}

  //! Set the data interpolation rate based on a looping frequency.
  /*!
    This function determines the interpolation rate based on the file
    size and the current Stk::sampleRate.  The \e frequency value
    corresponds to file cycles per second.  The frequency can be
    negative, in which case the loop is read in reverse order.
  */
  void setFrequency( stk::StkFloat frequency ) { this->setRate( file_.fileSize() * frequency / Stk::sampleRate() ); };

  //! Increment the read pointer by \e time samples, modulo file size.
  void addTime( stk::StkFloat time );

  //! Increment current read pointer by \e angle, relative to a looping frequency.
  /*!
    This function increments the read pointer based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
  */
  void addPhase( stk::StkFloat angle );

  //! Add a phase offset to the current read pointer.
  /*!
    This function determines a time offset based on the file
    size and the current Stk::sampleRate.  The \e angle value
    is a multiple of file size.
  */
  void addPhaseOffset( stk::StkFloat angle );

  //! Return the specified channel value of the last computed frame.
  /*!
    For multi-channel files, use the lastFrame() function to get
    all values from the last computed frame.  If no file data is
    loaded, the returned value is 0.0.  The \c channel argument must
    be less than the number of channels in the file data (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  stk::StkFloat lastOut( unsigned int channel = 0 ) { return stk::FileWvIn::lastOut( channel ); };

  //! Compute a sample frame and return the specified \c channel value.
  /*!
    For multi-channel files, use the lastFrame() function to get
    all values from the computed frame.  If no file data is loaded,
    the returned value is 0.0.  The \c channel argument must be less
    than the number of channels in the file data (the first channel is
    specified by 0).  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  stk::StkFloat tick( unsigned int channel = 0 );

  //! Fill the StkFrames argument with computed frames and return the same reference.
  /*!
    The number of channels in the StkFrames argument should equal
    the number of channels in the file data.  However, this is only
    checked if _STK_DEBUG_ is defined during compilation, in which
    case an incompatibility will trigger an StkError exception.  If no
    file data is loaded, the function does nothing (a warning will be
    issued if _STK_DEBUG_ is defined during compilation and
    Stk::showWarnings() has been set to \e true).
  */
  stk::StkFrames& tick( stk::StkFrames& frames );

 protected:

  stk::StkFrames firstFrame_;
  stk::StkFloat phaseOffset_;

};

//} // stk namespace

#endif
