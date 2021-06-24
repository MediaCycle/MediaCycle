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

#include "ACAudioStkFileLoop.h"
#include <cmath>

using namespace stk;

//namespace stk {

ACAudioStkFileLoop :: ACAudioStkFileLoop( unsigned long chunkThreshold, unsigned long chunkSize )
  : FileWvIn( chunkThreshold, chunkSize ), phaseOffset_(0.0)
{
  Stk::addSampleRateAlert( this );
}

ACAudioStkFileLoop :: ACAudioStkFileLoop( std::string fileName, bool raw, bool doNormalize,
                      unsigned long chunkThreshold, unsigned long chunkSize,
                      bool doInt2FloatScaling )
  : FileWvIn( chunkThreshold, chunkSize ), phaseOffset_(0.0)
{
  this->openFile( fileName, raw, doNormalize );
  Stk::addSampleRateAlert( this );
}

ACAudioStkFileLoop :: ~ACAudioStkFileLoop( void )
{
  Stk::removeSampleRateAlert( this );
}

void ACAudioStkFileLoop ::  openFile( std::string fileName, bool raw, bool doNormalize, bool doInt2FloatScaling )
{
  // Call close() in case another file is already open.
  this->closeFile();

  // Attempt to open the file ... an error might be thrown here.
    try{
  file_.open( fileName, raw );
    }
    catch(StkError& e){
        std::cerr << "ACAudioStkFileLoop::openFile: error " << std::endl;
    }

  // Determine whether chunking or not.
  if ( file_.fileSize() > chunkThreshold_ ) {
    chunking_ = true;
    chunkPointer_ = 0;
    data_.resize( chunkSize_ + 1, file_.channels() );
  }
  else {
    chunking_ = false;
    data_.resize( file_.fileSize() + 1, file_.channels() );
  }

  if ( doInt2FloatScaling )
    int2floatscaling_ = true;
  else
    int2floatscaling_ = false;

  // Load all or part of the data.
  file_.read( data_, 0, int2floatscaling_ );

  // Resize our lastFrame container.
  lastFrame_.resize( 1, file_.channels() );

  // Close the file unless chunking
  fileSize_ = file_.fileSize();
  if ( !chunking_ ) file_.close();

  // Set default rate based on file sampling rate.
  this->setRate( data_.dataRate() / Stk::sampleRate() );

  if ( doNormalize & !chunking_ ) this->normalize();

  this->reset();
}

void ACAudioStkFileLoop :: setRate( StkFloat rate )
{
  rate_ = rate;

  if ( fmod( rate_, 1.0 ) != 0.0 ) interpolate_ = true;
  else interpolate_ = false;
}

void ACAudioStkFileLoop :: addTime( StkFloat time )
{
  // Add an absolute time in samples.
  time_ += time;

  StkFloat fileSize = file_.fileSize();
  while ( time_ < 0.0 )
    time_ += fileSize;
  while ( time_ >= fileSize )
    time_ -= fileSize;
}

void ACAudioStkFileLoop :: addPhase( StkFloat angle )
{
  // Add a time in cycles (one cycle = fileSize).
  StkFloat fileSize = file_.fileSize();
  time_ += fileSize * angle;

  while ( time_ < 0.0 )
    time_ += fileSize;
  while ( time_ >= fileSize )
    time_ -= fileSize;
}

void ACAudioStkFileLoop :: addPhaseOffset( StkFloat angle )
{
  // Add a phase offset in cycles, where 1.0 = fileSize.
  phaseOffset_ = file_.fileSize() * angle;
}

StkFloat ACAudioStkFileLoop :: tick( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "FileWvIn::tick(): channel argument and soundfile data are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( finished_ ) return 0.0;

  if ( time_ < 0.0 || time_ > (StkFloat) ( fileSize_ - 1.0 ) ) {
    for ( unsigned int i=0; i<lastFrame_.size(); i++ ) lastFrame_[i] = 0.0;
    finished_ = true;
    return 0.0;
  }

  StkFloat tyme = time_;
  if ( chunking_ ) {

    // Check the time address vs. our current buffer limits.
    if ( ( time_ < (StkFloat) chunkPointer_ ) ||
         ( time_ > (StkFloat) ( chunkPointer_ + chunkSize_ - 1 ) ) ) {

      while ( time_ < (StkFloat) chunkPointer_ ) { // negative rate
        chunkPointer_ -= chunkSize_ - 1; // overlap chunks by one frame
        if ( chunkPointer_ < 0 ) chunkPointer_ = 0;
      }
      while ( time_ > (StkFloat) ( chunkPointer_ + chunkSize_ - 1 ) ) { // positive rate
        chunkPointer_ += chunkSize_ - 1; // overlap chunks by one frame
        if ( chunkPointer_ + chunkSize_ > fileSize_ ) // at end of file
          chunkPointer_ = fileSize_ - chunkSize_;
      }

      // Load more data.
      file_.read( data_, chunkPointer_, int2floatscaling_ );
    }

    // Adjust index for the current buffer.
    tyme -= chunkPointer_;
  }

  if ( interpolate_ ) {
    for ( unsigned int i=0; i<lastFrame_.size(); i++ )
      lastFrame_[i] = data_.interpolate( tyme, i );
  }
  else {
    for ( unsigned int i=0; i<lastFrame_.size(); i++ )
      lastFrame_[i] = data_( (size_t) tyme, i );
  }

  // Increment time, which can be negative.
  time_ += rate_;

  return lastFrame_[channel];
}

StkFrames& ACAudioStkFileLoop :: tick( StkFrames& frames )
{
  if ( !file_.isOpen() ) {
#if defined(_STK_DEBUG_)
    oStream_ << "ACAudioStkFileLoop::tick(): no file data is loaded!";
    handleError( StkError::WARNING );
#endif
    return frames;
  }

  unsigned int nChannels = lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( nChannels != frames.channels() ) {
    oStream_ << "ACAudioStkFileLoop::tick(): StkFrames argument is incompatible with file data!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  unsigned int j, counter = 0;
  for ( unsigned int i=0; i<frames.frames(); i++ ) {
    this->tick();
    for ( j=0; j<nChannels; j++ )
      frames[counter++] = lastFrame_[j];
  }

  return frames;
}

//} // stk namespace
