/*
 *  ACInputControlsDialogQt.cpp
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

#include "ACInputControlsDialogQt.h"
#include<iostream>

ACInputControlsDialogQt::ACInputControlsDialogQt ( QList<QAction *> _actions,
	QWidget *pParent ) : QDialog(pParent)
{
	// Setup UI struct...
	ui.setupUi(this);

	// Window modality (let plugin/tool windows rave around).
    QDialog::setWindowModality(Qt::NonModal);

	iDirtyCount = 0;

	ui.ShortcutTable->setIconSize(QSize(16, 16));
    ui.ShortcutTable->setItemDelegate(new ACInputControlsTableItemDelegateQt(ui.ShortcutTable));
//	ui.ShortcutTable->setSelectionMode(QAbstractItemView::SingleSelection);
//	ui.ShortcutTable->setSelectionBehavior(QAbstractItemView::SelectRows);
//	ui.ShortcutTable->setAlternatingRowColors(true);
	ui.ShortcutTable->setSortingEnabled(true);

//	ui.ShortcutTable->setHorizontalHeaderLabels(
//		QStringList() << tr("Item") << tr("Description") << tr("Shortcut"));
	ui.ShortcutTable->horizontalHeader()->setStretchLastSection(true);
	ui.ShortcutTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	ui.ShortcutTable->horizontalHeader()->resizeSection(0, 120);
    ui.ShortcutTable->horizontalHeader()->resizeSection(1, 250);

	int iRowHeight = ui.ShortcutTable->fontMetrics().height() + 4;
	ui.ShortcutTable->verticalHeader()->setDefaultSectionSize(iRowHeight);
	ui.ShortcutTable->verticalHeader()->hide();

	int iRow = 0;
    QListIterator<QAction *> iter(_actions);
	while (iter.hasNext()) {
		QAction *pAction = iter.next();
        //std::cout << "ACInputControlsDialogQt pAction->objectName() '" << pAction->objectName().toStdString() <<"' pAction->text() '" << pAction->text().toStdString() << "' pAction->statusTip() '" << pAction->statusTip().toStdString() << "' pAction->toolTip() '" << pAction->toolTip().toStdString() << "' pAction->menu() '" << pAction->menu() <<"'" << std::endl;
		//if (pAction->objectName().isEmpty())
        //if (pAction->text().isEmpty())
        if (pAction->menu() || pAction->text().isEmpty())
			continue;
		ui.ShortcutTable->insertRow(iRow);
		ui.ShortcutTable->setItem(iRow, 0,
            new ACInputControlsTableItemQt(pAction->icon(), pAction->text()));
		ui.ShortcutTable->setItem(iRow, 1,
            new ACInputControlsTableItemQt(pAction->toolTip()));
		ui.ShortcutTable->setItem(iRow, 2,
            new ACInputControlsTableItemQt(pAction->shortcut()));
        this->actions.append(pAction);
		++iRow;
	}

    // Restore last seen form position and extents...
    // here...

	QObject::connect(ui.ShortcutTable,
		SIGNAL(itemActivated(QTableWidgetItem *)),
		SLOT(actionActivated(QTableWidgetItem *)));

	QObject::connect(ui.ShortcutTable,
		SIGNAL(itemChanged(QTableWidgetItem *)),
		SLOT(actionChanged(QTableWidgetItem *)));

	QObject::connect(ui.DialogButtonBox,
		SIGNAL(accepted()),
		SLOT(accept()));
	QObject::connect(ui.DialogButtonBox,
		SIGNAL(rejected()),
		SLOT(reject()));
}


ACInputControlsDialogQt::~ACInputControlsDialogQt (void)
{
    // Store form position and extents...
    // here..
}

void ACInputControlsDialogQt::actionActivated ( QTableWidgetItem *pItem )
{
	ui.ShortcutTable->editItem(ui.ShortcutTable->item(pItem->row(), 2));
}

void ACInputControlsDialogQt::actionChanged ( QTableWidgetItem *pItem )
{
	pItem->setText(QString(QKeySequence(pItem->text().trimmed())));
	++iDirtyCount;
    for (int iRow = 0; iRow < actions.count(); ++iRow) {
        QAction *pAction = actions[iRow];
        pAction->setShortcut(
            QKeySequence(ui.ShortcutTable->item(iRow, 2)->text()));
        //std::cout << "Shortcut '" << ui.ShortcutTable->item(iRow, 2)->text().toStdString() << "'" << std::endl;
    }
}

void ACInputControlsDialogQt::accept (void)
{
	if (iDirtyCount > 0) {
		for (int iRow = 0; iRow < actions.count(); ++iRow) {
			QAction *pAction = actions[iRow];
			pAction->setShortcut(
				QKeySequence(ui.ShortcutTable->item(iRow, 2)->text()));
		}
	}
	QDialog::accept();
}

void ACInputControlsDialogQt::reject (void)
{
	bool bReject = true;

	// Check if there's any pending changes...
	if (iDirtyCount > 0) {
		QMessageBox::StandardButtons buttons
			= QMessageBox::Discard | QMessageBox::Cancel;
		if (ui.DialogButtonBox->button(QDialogButtonBox::Ok)->isEnabled())
			buttons |= QMessageBox::Apply;
		switch (QMessageBox::warning(this,
			tr("Warning"),
			tr("Keyboard shortcuts have been changed.\n\n"
			"Do you want to apply the changes?"),
			buttons)) {
		case QMessageBox::Apply:
			accept();
			return;
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bReject = false;
		}
	}

	if (bReject)
		QDialog::reject();
}
