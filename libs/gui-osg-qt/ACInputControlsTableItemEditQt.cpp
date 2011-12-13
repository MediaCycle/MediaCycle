/*
 *  ACInputControlsTableItemEditQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 8/12/11
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
 *  Highly inspired from the Qtractor shortcut editor
 *  http://qtractor.sf.net
 *
 */

#include "ACInputControlsTableItemEditQt.h"

void ACInputControlsTableItemEditQt::keyPressEvent(QKeyEvent *pKeyEvent)
{
    int iKey = pKeyEvent->key();
    Qt::KeyboardModifiers modifiers = pKeyEvent->modifiers();

    if (iKey == Qt::Key_Return && modifiers == Qt::NoModifier) {
        emit editingFinished();
        return;
    }

    if (iKey >= Qt::Key_Shift && iKey < Qt::Key_F1) {
        QLineEdit::keyPressEvent(pKeyEvent);
        return;
    }

    if (modifiers & Qt::ShiftModifier)
        iKey |= Qt::SHIFT;
    if (modifiers & Qt::ControlModifier)
        iKey |= Qt::CTRL;
    if (modifiers & Qt::AltModifier)
        iKey |= Qt::ALT;

    QLineEdit::setText(QString(QKeySequence(iKey)));
}
