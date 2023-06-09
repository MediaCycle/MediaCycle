//
//  ACInputActionQt.cpp
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

#include "ACInputActionQt.h"

ACInputActionQt::ACInputActionQt( const QString & text, QObject * parent, Qt::MouseButton mouse_button)
    : QAction(text,parent), mouse_button(Qt::NoButton),
      key_event_type(QEvent::None), mouse_event_type(QEvent::None), tablet_event_type(QEvent::None), touch_event_type(QEvent::None),
      key_pressed(false),other_key_pressed(false),mouse_pressed(false),toggle(false),category(QString("")),
      device_name(QString("")),device_event(QString(""))
{
    // Mouse action map, should be done elsewhere / static
    mouseEventNames[QEvent::None] = " ";
    //mouseEventNames[QEvent::MouseButtonDblClick] = "DoubleClick"; // Mouse press again (QMouseEvent).
    //mouseEventNames[QEvent::MouseButtonPress] = "Press"; // Mouse press (QMouseEvent).
    mouseEventNames[QEvent::MouseButtonRelease] = "Click"; // Mouse release (QMouseEvent).
    mouseEventNames[QEvent::MouseMove] = "Move"; // Mouse move (QMouseEvent)
    //mouseEventNames[QEvent::HoverEnter] = "HoverEnter"; // The mouse cursor enters a hover widget
    //mouseEventNames[QEvent::HoverLeave] = "HoverLeave"; // The mouse cursor leaves a hover widget
    //mouseEventNames[QEvent::HoverMove] = "HoverMove"; // The mouse cursor moves inside a hover widget
    mouseEventNames[ACEventQt::MousePressedMove] = "Click+Move"; // Mouse moved when pressed (custom).
}

void ACInputActionQt::setCategory(const QString _category)
{
    this->category = _category;
}

void ACInputActionQt::eventAbsorber ( QEvent * _event )
{
    this->event(_event);
}

bool ACInputActionQt::event ( QEvent * _event )
{
    //QAction::event(_event );
    //std::cout << "QEvent type " <<_event->type() <<std::endl;
    /*if(this->text().toStdString() == "Select grains"){
        std::cout << "AudioGarden selecting grains type" << _event->type() << std::endl;
    }*/
    //std::cout << "Action " << this->text().toStdString()  << std::endl;

    switch (_event->type()){
    case QEvent::Shortcut : // Key press (QKeyEvent).
    {

        //if(this->text().toStdString() == "Play clicked node")
        //    std::cout << "Play" << std::endl;
        QShortcutEvent *_revent = static_cast<QShortcutEvent*>(_event);
        if( !this->shortcut().isEmpty() && this->shortcut().matches(QKeySequence(_revent->key())) ){
            //std::cout << "ACInputActionQt KeyPress" << std::endl;
            key_pressed = true;
            other_key_pressed = false;
            if(this->key_event_type == QEvent::KeyPress && (this->mouse_event_type == QEvent::None || this->mouse_event_type == QEvent::MouseMove)){
                emit triggered(true);
                toggle = !toggle;
                emit toggled(toggle);
            }
        }
        else
            other_key_pressed = true;
        //std::cout << "ACInputActionQt " << this->text().toStdString() << " Shortcut: key pressed " << key_pressed << " other " << other_key_pressed << std::endl;
    }
        break;
    case QEvent::KeyPress : // Key press (QKeyEvent).
    {
        //if(this->text().toStdString() == "Play clicked node")
        //    std::cout << "Play" << std::endl;
        QKeyEvent *_revent = static_cast<QKeyEvent*>(_event);
        if( !this->shortcut().isEmpty() && this->shortcut().matches(QKeySequence(_revent->key())) ){
            //std::cout << "ACInputActionQt KeyPress" << std::endl;
            key_pressed = true;
            other_key_pressed = false;
            if(this->key_event_type == QEvent::KeyPress && (this->mouse_event_type == QEvent::None || this->mouse_event_type == QEvent::MouseMove) || this->mouse_event_type == QEvent::None){
                emit triggered(true);
                toggle = !toggle;
                //std::cout << "Action " << this->text().toStdString() << " key pressed and mouse pressed" << mouse_pressed << std::endl;
                emit toggled(toggle);
            }
        }
        else
            other_key_pressed = true;
        //std::cout << "ACInputActionQt " << this->text().toStdString() << " KeyPress: key pressed " << key_pressed << " other " << other_key_pressed << std::endl;
    }
        break;
    case QEvent::KeyRelease : // Key release (QKeyEvent).
    {
        //if(this->text().toStdString() == "Play clicked node")
        //    std::cout << "Play" << std::endl;
        QKeyEvent *_revent = static_cast<QKeyEvent*>(_event);
        if( !this->shortcut().isEmpty() && this->shortcut().matches(QKeySequence(_revent->key())) ){
            //std::cout << "ACInputActionQt KeyRelease" << std::endl;
            key_pressed = false;
            if(this->key_event_type == QEvent::KeyRelease && (this->mouse_event_type == QEvent::None || this->mouse_event_type == QEvent::MouseMove) || this->mouse_event_type == QEvent::None){
                //std::cout << "Action " << this->text().toStdString() << " key released and mouse pressed" << mouse_pressed << std::endl;
                emit triggered(false);
                //emit toggled(false);
            }
        }
        else
            other_key_pressed = true;
        //std::cout << "ACInputActionQt " << this->text().toStdString() << " KeyPress: key pressed " << key_pressed << " other " << other_key_pressed << std::endl;
        //
    }
        break;
    case QEvent::MouseButtonDblClick : // Mouse press again (QMouseEvent).
        //std::cout << "ACInputActionQt MouseButtonDblClick" << std::endl;
        break;
    case QEvent::MouseButtonPress : // Mouse press (QMouseEvent).
    {
        //if(this->text().toStdString() == "Play clicked node")
        //    std::cout << "Play" << std::endl;
        mouse_pressed = true;
        if(this->mouse_event_type == QEvent::MouseButtonPress){
            //std::cout << "ACInputActionQt MouseButtonPress" << std::endl;
            if((this->key_event_type == QEvent::None && !other_key_pressed)|| (this->key_event_type != QEvent::None && key_pressed && !other_key_pressed) ){
                //std::cout << "Action " << this->text().toStdString() << " mouse pressed and key pressed " << key_pressed << std::endl;
                emit triggered();
            }
        }
        //std::cout << "ACInputActionQt " << this->text().toStdString() << " MouseButtonPress: key pressed " << key_pressed << " other " << other_key_pressed << std::endl;
    }
        break;
    case QEvent::MouseButtonRelease : // Mouse release (QMouseEvent).
    {
        //if(this->text().toStdString() == "Play clicked node")
        //    std::cout << "Play" << std::endl;
        mouse_pressed = false;
        if(this->mouse_event_type == QEvent::MouseButtonRelease){
            //std::cout << "ACInputActionQt MouseButtonRelease" << std::endl;
            if( (this->key_event_type == QEvent::None && !other_key_pressed)|| (this->key_event_type != QEvent::None && key_pressed && !other_key_pressed) ){
                //std::cout << "Action " << this->text().toStdString() << " mouse released and key pressed " << key_pressed << std::endl;
                emit triggered();
            }
        }
        //std::cout << "ACInputActionQt " << this->text().toStdString() << " MouseButtonRelease: key pressed " << key_pressed << " other " << other_key_pressed << std::endl;
        other_key_pressed = false;
    }
        break;
    case QEvent::MouseMove : // Mouse move (QMouseEvent).
    {
        if(this->mouse_event_type == QEvent::MouseMove || this->mouse_event_type == ACEventQt::MousePressedMove){
            //std::cout << "ACInputActionQt MouseMove" << std::endl;
            if( (this->key_event_type == QEvent::None && !other_key_pressed && this->mouse_event_type != ACEventQt::MousePressedMove ) || (this->key_event_type != QEvent::None && key_pressed) || (this->mouse_event_type == ACEventQt::MousePressedMove && mouse_pressed) ){
                QMouseEvent *_revent = static_cast<QMouseEvent*>(_event);
                emit mouseMovedXY(_revent->x(), _revent->y());
                emit mouseMovedX(_revent->x());
                emit mouseMovedY(_revent->y());
            }
        }
    }
        break;
        /*		case QEvent::HoverEnter : // The mouse cursor enters a hover widget (QHoverEvent).
            std::cout << "ACInputActionQt HoverEnter" << std::endl;
            break;
        case QEvent::HoverLeave : // The mouse cursor leaves a hover widget (QHoverEvent).
            std::cout << "ACInputActionQt HoverLeave" << std::endl;
            break;
        case QEvent::HoverMove : // The mouse cursor moves inside a hover widget (QHoverEvent).
            std::cout << "ACInputActionQt HoverMove" << std::endl;
            //this->mouse_event_type = _type;
*/			break;
        /*		case QEvent::TabletMove : // Wacom tablet move (QTabletEvent).
            std::cout << "ACInputActionQt TabletMove" << std::endl;
            break;
        case QEvent::TabletPress : // Wacom tablet press (QTabletEvent).
            std::cout << "ACInputActionQt TabletPress" << std::endl;
            break;
        case QEvent::TabletRelease : // Wacom tablet release (QTabletEvent).
            std::cout << "ACInputActionQt TabletRelease" << std::endl;
            break;
        case QEvent::TabletEnterProximity :  // Wacom tablet enter proximity event (QTabletEvent), sent to QApplication.
            std::cout << "ACInputActionQt TabletEnterProximity" << std::endl;
            break;
        case QEvent::TabletLeaveProximity : // Wacom tablet leave proximity event (QTabletEvent), sent to QApplication.
            std::cout << "ACInputActionQt TabletLeaveProximity" << std::endl;
            //this->tablet_event_type = _type;
            break;
        case QEvent::TouchBegin : // Beginning of a sequence of touch-screen and/or track-pad events (QTouchEvent)
            std::cout << "ACInputActionQt TouchBegin" << std::endl;
            break;
        case QEvent::TouchUpdate : // Touch-screen event (QTouchEvent)
            std::cout << "ACInputActionQt TouchUpdate" << std::endl;
            break;
        case QEvent::TouchEnd : // End of touch-event sequence (QTouchEvent)
            std::cout << "ACInputActionQt TouchEnd" << std::endl;
            //this->touch_event_type = _type;
            break;*/
    default:
        break;
    }
    //return QAction::event(_event);
    return true;
}

// TODO CF notify that QEvent::Key(Press,Release) with QEvent::MouseButtonRelease works, but not with QEvent::MouseButtonPress
// check occurences of setClickedNode(-1)

void ACInputActionQt::setKeyEventType(QEvent::Type _type)
{
    switch (_type){
    case QEvent::KeyPress : // Key press (QKeyEvent).
        //break;
    case QEvent::KeyRelease : // Key release (QKeyEvent).
    {
        this->key_event_type = _type;
        if(this->mouse_event_type == QEvent::MouseButtonPress)
            this->mouse_event_type = QEvent::MouseButtonRelease; //to make mouse+key combos work for now...
    }
        break;
    default:
        this->key_event_type = QEvent::None;
        break;
    }
}

QEvent::Type ACInputActionQt::getKeyEventType()
{
    return key_event_type;
}

void ACInputActionQt::setMouseButton(Qt::MouseButton _button)
{
    switch (_button){
    case Qt::LeftButton : // The left button is pressed, or an event refers to the left button. (The left button may be the right button on left-handed mice.)
        //break;
    case Qt::RightButton : // The right button.
        //break
    case Qt::MidButton : // The middle button (older Qt).
        //break
        //case Qt::MiddleButton : // The middle button (newer Qt, not working in Ubuntu 10.04).
        //break
    case Qt::XButton1 : // The first X button.
        //break
    case Qt::XButton2 : // The second X button.
        this->mouse_button = _button;
        break;
    default:
        this->mouse_button = Qt::NoButton; // The button state does not refer to any button (see QMouseEvent::button()).
        break;
    }
    this->mouse_button = _button;
}

Qt::MouseButton ACInputActionQt::getMouseButton() // not yet used
{
    return mouse_button;
}

// TODO CF notify that QEvent::Key(Press,Release) with QEvent::MouseButtonRelease works, but not with QEvent::MouseButtonPress
// check occurences of setClickedNode(-1)

void ACInputActionQt::setMouseEventType(QEvent::Type _type)
{
    switch (_type){
    case QEvent::MouseButtonDblClick : // Mouse press again (QMouseEvent).
        //break;
    case QEvent::MouseButtonPress : // Mouse press (QMouseEvent).
        //break;
    case QEvent::MouseButtonRelease : // Mouse release (QMouseEvent).
    {
        // Valid for all mouse clicks
        if(this->key_event_type != QEvent::None)
            _type = QEvent::MouseButtonRelease; // to make mouse+key combos work for now...
    }
        //break;
    case QEvent::MouseMove : // Mouse move (QMouseEvent).
        //break;
    case QEvent::HoverEnter : // The mouse cursor enters a hover widget (QHoverEvent).
        //break;
    case QEvent::HoverLeave : // The mouse cursor leaves a hover widget (QHoverEvent).
        //break;
    case QEvent::HoverMove : // The mouse cursor moves inside a hover widget (QHoverEvent).
        //break;
    case ACEventQt::MousePressedMove : // QEvent::MousePressedMove : // Mouse moved when pressed (custom).
        this->mouse_event_type = _type;
        break;
    default:
        this->mouse_event_type = QEvent::None;
        break;
    }
}

QEvent::Type ACInputActionQt::getMouseEventType()
{
    return this->mouse_event_type;
}

QString ACInputActionQt::getMouseEventName(){
    return mouseEventNames[this->mouse_event_type];
}

QStringList ACInputActionQt::getMouseEventNames(){
    QStringList _names;
    for (ACMouseEventNamesQt::iterator _name = mouseEventNames.begin(); _name != mouseEventNames.end();++_name)
        _names.append(_name->second);
    return _names;
}

QEvent::Type ACInputActionQt::convertMouseEventNameToType(QString _name)
{
    for (ACMouseEventNamesQt::iterator _names = mouseEventNames.begin(); _names != mouseEventNames.end();++_names)
        if(_names->second == _name)
            return _names->first;
    return QEvent::None;
}

QString ACInputActionQt::convertMouseEventTypeToName(QEvent::Type _type)
{
    return mouseEventNames[_type];
}

void ACInputActionQt::setTabletEventType(QEvent::Type _type)
{
    switch (_type){
    case QEvent::TabletMove : // Wacom tablet move (QTabletEvent).
        //break;
    case QEvent::TabletPress : // Wacom tablet press (QTabletEvent).
        //break;
    case QEvent::TabletRelease : // Wacom tablet release (QTabletEvent).
        //break;
    case QEvent::TabletEnterProximity :  // Wacom tablet enter proximity event (QTabletEvent), sent to QApplication.
        //break;
    case QEvent::TabletLeaveProximity : // Wacom tablet leave proximity event (QTabletEvent), sent to QApplication.
        this->tablet_event_type = _type;
        break;
    default:
        this->tablet_event_type = QEvent::None;
        break;
    }
}

QEvent::Type ACInputActionQt::getTabletEventType() // not yet used
{
    return tablet_event_type;
}

void ACInputActionQt::setTouchEventType(QEvent::Type _type)
{
    switch (_type){
    case QEvent::TouchBegin : // Beginning of a sequence of touch-screen and/or track-pad events (QTouchEvent)
        //break;
    case QEvent::TouchUpdate : // Touch-screen event (QTouchEvent)
        //break;
    case QEvent::TouchEnd : // End of touch-event sequence (QTouchEvent)
        this->touch_event_type = _type;
        break;
    default:
        this->touch_event_type = QEvent::None;
        break;
    }
}

//enum Qt::TouchPointState
//Qt::TouchPointPressed // The touch point is now pressed.
//Qt::TouchPointMoved // The touch point moved.
//Qt::TouchPointStationary // The touch point did not move.
//Qt::TouchPointReleased // The touch point was released.

QEvent::Type ACInputActionQt::getTouchEventType() // not yet used
{
    return touch_event_type;
}
