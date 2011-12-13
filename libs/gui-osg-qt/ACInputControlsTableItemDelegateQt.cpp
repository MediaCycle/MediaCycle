/*
 *  ACInputControlsTableItemDelegateQt.cpp
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

#include "ACInputControlsTableItemDelegateQt.h"

ACInputControlsTableItemDelegateQt::ACInputControlsTableItemDelegateQt (
	QTableWidget *tableWidget )
	: QItemDelegate(tableWidget), tableWidget(tableWidget)
{
}


// Overridden paint method.
void ACInputControlsTableItemDelegateQt::paint ( QPainter *pPainter,
	const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
	// Special treatment for action icon+text...
	if (index.column() == 0) {
		QTableWidgetItem *pItem = tableWidget->item(index.row(), 0);
		pPainter->save();
		if (option.state & QStyle::State_Selected) {
			const QPalette& pal = option.palette;
			pPainter->fillRect(option.rect, pal.highlight().color());
			pPainter->setPen(pal.highlightedText().color());
		}
		// Draw the icon...
		QRect rect = option.rect;
		const QSize& iconSize = tableWidget->iconSize();
		pPainter->drawPixmap(1,
			rect.top() + ((rect.height() - iconSize.height()) >> 1),
			pItem->icon().pixmap(iconSize));
		// Draw the text...
		rect.setLeft(iconSize.width() + 2);
		pPainter->drawText(rect,
			Qt::TextShowMnemonic | Qt::AlignLeft | Qt::AlignVCenter,
			pItem->text());
		pPainter->restore();
	} else {
		// Others do as default...
		QItemDelegate::paint(pPainter, option, index);
	}
}


QWidget *ACInputControlsTableItemDelegateQt::createEditor ( QWidget *pParent,
	const QStyleOptionViewItem& /*option*/, const QModelIndex& index ) const
{
	ACInputControlsTableItemEditorQt *pItemEditor
		= new ACInputControlsTableItemEditorQt(pParent);
	pItemEditor->setDefaultText(
		index.model()->data(index, Qt::DisplayRole).toString());
	QObject::connect(pItemEditor,
		SIGNAL(editingFinished()),
		SLOT(commitEditor()));
	return pItemEditor;
}


void ACInputControlsTableItemDelegateQt::setEditorData ( QWidget *pEditor,
	const QModelIndex& index ) const
{
	ACInputControlsTableItemEditorQt *pItemEditor
		= qobject_cast<ACInputControlsTableItemEditorQt *> (pEditor);
	pItemEditor->setText(
		index.model()->data(index, Qt::DisplayRole).toString());
}


void ACInputControlsTableItemDelegateQt::setModelData ( QWidget *pEditor,
	QAbstractItemModel *pModel, const QModelIndex& index ) const
{
	ACInputControlsTableItemEditorQt *pItemEditor
		= qobject_cast<ACInputControlsTableItemEditorQt *> (pEditor);
	pModel->setData(index, pItemEditor->text());
}


void ACInputControlsTableItemDelegateQt::commitEditor (void)
{
	ACInputControlsTableItemEditorQt *pItemEditor
		= qobject_cast<ACInputControlsTableItemEditorQt *> (sender());
	emit commitData(pItemEditor);
	emit closeEditor(pItemEditor);
}
