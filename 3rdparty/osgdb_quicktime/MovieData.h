 /*
 *  MovieData.h
 *  encapsulates movie-related stuff
 *
 *  Created by Stephan Huber on Wed Mar 10 2004.
 *  Copyright (c) 2004 digital mind. All rights reserved.
 *
 */

#ifndef _MOVIEDATA_HEADER_
#define _MOVIEDATA_HEADER_

#include <osg/Notify>
#include <osg/Image>
#include <string>

#include "QTUtils.h"

#include <math.h>

//CF 2.8.x compatibility
#include <osg/Referenced>
#include <osg/Version>
#include <osg/Notify>
#if OSG_MIN_VERSION_REQUIRED(2,9,10)
#else
#undef OSG_NOTIFY
#define OSG_NOTIFY(level) if (osg::isNotifyEnabled(level)) osg::notify(level)
#define OSG_ALWAYS OSG_NOTIFY(osg::ALWAYS)
#define OSG_FATAL OSG_NOTIFY(osg::FATAL)
#define OSG_WARN OSG_NOTIFY(osg::WARN)
#define OSG_NOTICE OSG_NOTIFY(osg::NOTICE)
#define OSG_INFO OSG_NOTIFY(osg::INFO)
#define OSG_DEBUG OSG_NOTIFY(osg::DEBUG_INFO)
#define OSG_DEBUG_FP OSG_NOTIFY(osg::DEBUG_FP)
#endif
//CF end


    /** 
     * the class MovieData encapsulates all quicktime-related stuff, so it doesn't polute the namespaces
     * it handles all calls to quicktime etc... It is mainly used by the QuicktimeImageStream, it is 
     * rarely needed in other contexts 
     */
    class MovieData {
    
        public:
            /** default constructor */
            MovieData();
            
            /** default destructor */
            ~MovieData();
            
            /** 
             * loads a movie, start it playing at startTime, use Image for the storage
             * @param image the image, to use as storage
             * @param fileName the movie to open
             * @param startTime the starttime to begin with
             */
            void load(osg::Image* image, std::string fileName, float startTime = 0.0f);
            
            /** @return the duration for this movie in seconds */
            inline float getMovieDuration() { return GetMovieDuration(_movie)/(float)_timescale;}
            
            /** @return the current position for this movie in seconds */
            inline float getMovieTime()  {return GetMovieTime(_movie,NULL)/(float)_timescale; }
            
            /** stes the movietime */
            void setMovieTime(float atime);
                        
            /** @return the Movie-handle, to use it with other quicktime-calls */
            inline Movie &getMovie() { return _movie; }
            
            /** @return the current movieRate */
            inline float getMovieRate() { return Fix2X(GetMovieRate(_movie)); }
            /** @return returns the cached movierate, may differ to the real movierate */
            inline float getCachedMovieRate() { return _movieRate; }
            
            /** sets the MovieRate for this movie */
            void setMovieRate(float rate);
            
            /** sets the volume for the soundtrack of this movie */
            void setVolume(float volume) { SetMovieVolume(_movie,(short)(ceil(volume*255.0f)));}
            float getVolume() const { return GetMovieVolume(_movie) / 255.0f; }
            
            void setAudioBalance(float f) {
                Float32 balance = f;
                SetMovieAudioBalance(_movie, balance, 0);
            }
            
            float getAudioBalance() {
                Float32 balance;
                float f;
                GetMovieAudioBalance(_movie, &balance, 0);
                f = balance;
                return f;
            }
            
            /** @return true, if this movie is looping */
            bool isLooping() const { return _isLooping; }
            
            /** sets the looping mode */
            void setLooping(bool loop) {
                if (_isLooping != loop) {
                    _isLooping = loop;
                    switch (_isLooping) {
                        case true:
                            SetTimeBaseFlags(GetMovieTimeBase(_movie), loopTimeBase);
                            break;
                        case false:
                            SetTimeBaseFlags(GetMovieTimeBase(_movie), 0);
                            break;
                    }
                }
            }
                
            
        protected:
            char*           _pointer;
            Movie           _movie;
            GWorldPtr       _gw;
            
            unsigned int    _movieWidth, _movieHeight, _textureWidth, _textureHeight;
            float           _timescale;
            bool            _fError;
            float           _movieRate;
            bool            _preRolled;
            bool            _isLooping;
            
            /** inits the image for storage */
            void _initImage(osg::Image* image);
            
            /** inits the gWorld, where the movie gets drawn into */
            void _initGWorldStuff(osg::Image * image);
            
            /** inits the texture */
            void _initTexture();
            
            /** checks for an movie-error */
            inline void _checkMovieError(std::string msg) {
                if (GetMoviesError()) {
                    _fError = true;
                    OSG_ALWAYS << "MovieData: GetMoviesError fails at " << msg << std::endl;
                }
            }
            
    };




#endif
