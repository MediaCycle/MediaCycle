/**
 * @brief ACInputActionQt.h
 * @author Alexis Moinet
 * @date 21/06/2012
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

#ifndef HEADER_ACInputActionQt
#define HEADER_ACInputActionQt

//
//  ACInputActionQt.h
//  MediaCycle
//
//  @author Christian Frisson
//  @date 8/12/2011
//  @copyright (c) 2011 – UMONS - Numediart
//  
//  MediaCycle of University of Mons – Numediart institute is 
//  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
//  licence (the “License”); you may not use this file except in compliance 
//  with the License.
//  
//  This program is free software: you can redistribute it and/or 
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  Each use of this software must be attributed to University of Mons – 
//  Numediart Institute
//  
//  Any other additional authorizations may be asked to avre@umons.ac.be 
//  <mailto:avre@umons.ac.be>
//

#include <QAction>
#include <QEvent>
#include <QtGui>

#include <iostream>
#include <MediaCycle.h>

typedef  std::map<QEvent::Type,QString> ACMouseEventNamesQt;

namespace ACEventQt {
static const QEvent::Type MousePressedMove = static_cast<QEvent::Type>(QEvent::User); // warning, optimize with QEvent::registerEventType() since QEvent::User might already be registered elsewhere in 3rdparty libraries
}

class ACInputActionQt : public QAction
{
	Q_OBJECT
public slots:	
	void eventAbsorber ( QEvent * event );
public:
    ACInputActionQt ( const QString & text, QObject * parent = 0, Qt::MouseButton mouse_button = Qt::NoButton  );
    ~ACInputActionQt(){}

    void setCategory(const QString _category); // to categorize actions (for instance: Library/Browser/Timeline) (not yet used)

    // Keyboard shortcuts
    void setKeyEventType(QEvent::Type _type);
    QEvent::Type getKeyEventType();

    // Mouse actions
    void setMouseButton(Qt::MouseButton _button); // not yet used
    Qt::MouseButton getMouseButton(); // not yet used
	void setMouseEventType(QEvent::Type _type);
    QEvent::Type getMouseEventType();
    QString getMouseEventName();
    QStringList getMouseEventNames();
    QEvent::Type convertMouseEventNameToType(QString name);
    QString convertMouseEventTypeToName(QEvent::Type type);

    // Tablet actions
    void setTabletEventType(QEvent::Type _type); // not yet used
    QEvent::Type getTabletEventType(); // not yet used

    // Touch actions
    void setTouchEventType(QEvent::Type _type); // not yet used
    QEvent::Type getTouchEventType(); // not yet used

signals:
	virtual void triggered ( bool checked = false );
    virtual void toggled ( bool checked );
    virtual void mouseMovedXY ( float x, float y );
    virtual void mouseMovedX ( float x );
    virtual void mouseMovedY ( float y );

protected:
	virtual bool event ( QEvent * event );
	Qt::MouseButton mouse_button;
	QEvent::Type key_event_type, mouse_event_type, tablet_event_type, touch_event_type;
    bool key_pressed, mouse_pressed;
    bool toggle;
    QString category;
    ACMouseEventNamesQt mouseEventNames;
};
#endif
