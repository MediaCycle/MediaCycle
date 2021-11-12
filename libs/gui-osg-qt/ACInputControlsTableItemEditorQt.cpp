/*
 *  ACInputControlsTableItemEditorQt.cpp
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

#include "ACInputControlsTableItemEditorQt.h"

ACInputControlsTableItemEditorQt::ACInputControlsTableItemEditorQt (
	QWidget *pParent ) : QWidget(pParent)
{
    pLineEdit = new ACInputControlsTableItemEditQt(/*this*/);

	pToolButton = new QToolButton(/*this*/);
	pToolButton->setFixedWidth(18);
	pToolButton->setText("X");

	QHBoxLayout *pLayout = new QHBoxLayout();
	pLayout->setSpacing(0);
	pLayout->setMargin(0);
	pLayout->addWidget(pLineEdit);
	pLayout->addWidget(pToolButton);
	QWidget::setLayout(pLayout);
	
	QWidget::setFocusPolicy(Qt::StrongFocus);
	QWidget::setFocusProxy(pLineEdit);

	QObject::connect(pLineEdit,
		SIGNAL(editingFinished()),
		SLOT(finish()));
	QObject::connect(pToolButton,
		SIGNAL(clicked()),
		SLOT(clear()));
}


// Shortcut text accessors.
void ACInputControlsTableItemEditorQt::setText ( const QString& sText )
{
	pLineEdit->setText(sText);
}


QString ACInputControlsTableItemEditorQt::text (void) const
{
	return pLineEdit->text();
}


// Shortcut text clear/toggler.
void ACInputControlsTableItemEditorQt::clear (void)
{
	if (pLineEdit->text() == sDefaultText)
		pLineEdit->clear();
	else
		pLineEdit->setText(sDefaultText);

	pLineEdit->setFocus();
}


// Shortcut text finish notification.
void ACInputControlsTableItemEditorQt::finish (void)
{
	emit editingFinished();
}
