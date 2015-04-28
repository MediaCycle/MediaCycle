/**
 * @brief A class that provides a media data instance to parse and read videos using FFmpeg.
 * @author Christian Frisson
 * @date 14/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#ifndef ACVideoFFmpegData_H
#define ACVideoFFmpegData_H

#include "ACVideoData.h"
#include "ACMediaTypes.h"
#include "ACFFmpegInclude.h"
#include <libavutil/mathematics.h>


class ACVideoFFmpegDataContainer : public ACMediaDataContainer, public ACSpatialData, public ACTemporalData {
public:
    ACVideoFFmpegDataContainer() : ACMediaDataContainer(), ACSpatialData(), ACTemporalData(),data(0) {}
    virtual ~ACVideoFFmpegDataContainer(){
        if(data)
            av_free(data);//delete data;
        data = 0;
    }
    void setData(AVFrame* _data){
        if(data)
            av_free(data);//delete data;
        data = 0;
        data = _data;
    }
    AVPicture* getData(){return (AVPicture*)data;}
    virtual int getNumberOfFrames(){
        if(data)
            return data->nb_samples;
        else
            return 0;//data.size();
    }
protected:
    AVFrame* data;
};

class ACVideoFFmpegData: public ACVideoData {
public:
    ACVideoFFmpegData():ACVideoData(), m_context(0),pFormatCtx(0){}
    virtual ~ACVideoFFmpegData(){}
    ACVideoFFmpegData(std::string _fname);

    virtual std::string getName(){return "FFmpeg video";}

    virtual bool readData(std::string _fname);
    virtual bool closeFile();

    /// Function for spatial media
    virtual int getWidth();
    virtual int getHeight();

    /// Functions for temporal media
    virtual float getSampleRate();
    virtual int getNumberOfChannels();
    virtual int getNumberOfFrames();
    virtual ACMediaDataContainer* getBuffer(int start_frame, int number_of_frames, int channel);

protected:
    int convert(AVPicture *dst, int dst_pix_fmt, AVPicture *src,int src_pix_fmt, int src_width, int src_height);
    void yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height);
    AVCodecContext* m_context;
    AVFormatContext *pFormatCtx;
};

#endif // ACVideoFFmpegData_H
