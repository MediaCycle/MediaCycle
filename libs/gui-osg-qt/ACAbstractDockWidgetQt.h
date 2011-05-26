/*
 *  ACAbstractDockWidget.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 18/02/11
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

#ifndef ACABSTRACTDOCKWIDGETQT_H
#define ACABSTRACTDOCKWIDGETQT_H

#include <MediaCycle.h>
#include <string>

#include <QtGui>

#if defined (SUPPORT_AUDIO)
#include <ACAudioEngine.h>
#endif //defined (SUPPORT_AUDIO)

#include "ACOsgCompositeViewQt.h"

class ACAbstractDockWidgetQt : public QDockWidget { 
//Q_OBJECT
	
public:
	ACAbstractDockWidgetQt(QWidget *parent = 0, ACMediaType _media_type = MEDIA_TYPE_NONE, std::string _class_name = "")
		: QDockWidget(parent), media_type(_media_type),class_name(_class_name),media_cycle(0),osg_view(0)
	{
		#if defined (SUPPORT_AUDIO)
		audio_engine = 0;
		#endif //defined (SUPPORT_AUDIO)
	};
    virtual ~ACAbstractDockWidgetQt(){};
	
	void setMediaType(ACMediaType _media_type){this->media_type=_media_type;}
	ACMediaType getMediaType(){return this->media_type;}
	void setClassName(std::string _class_name){this->class_name=_class_name;}
	std::string getClassName(){return this->class_name;}
	virtual void setMediaCycle(MediaCycle* _media_cycle){ media_cycle = _media_cycle;}
	MediaCycle* getMediaCycle() {return media_cycle;}
	#if defined (SUPPORT_AUDIO)
	virtual void setAudioEngine(ACAudioEngine* _audio_engine){ audio_engine = _audio_engine;}
	ACAudioEngine* getAudioEngine() {return audio_engine;}
	#endif //defined (SUPPORT_AUDIO)
	void setOsgView(ACOsgCompositeViewQt* _osg_view){ osg_view = _osg_view;}
	ACOsgCompositeViewQt* getOsgView() {return osg_view;}
	
private:
	ACMediaType media_type;
	std::string class_name;

protected:
	MediaCycle *media_cycle;
	#if defined (SUPPORT_AUDIO)
	ACAudioEngine *audio_engine;
	#endif //defined (SUPPORT_AUDIO)
	ACOsgCompositeViewQt* osg_view;
};

#endif // ACABSTRACTDOCKWIDGETQT_H
