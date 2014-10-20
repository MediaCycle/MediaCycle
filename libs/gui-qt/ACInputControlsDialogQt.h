/*
 *  ACInputControlsDialogQt.h
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
 */

#ifndef ACInputControlsDialogQt_H
#define ACInputControlsDialogQt_H

#include <QtGui>
#include <QComboBox>
#include <QMessageBox>
#include "ui_ACInputControlsDialogQt.h"
#include "ACInputControlsTableItemDelegateQt.h"
#include "ACInputControlsTableItemQt.h"
#include "ACInputControlsTableItemEditorQt.h"
#include "ACInputControlsTableItemDelegateQt.h"
#include "ACInputActionQt.h"
#include "ACAbstractWidgetQt.h"

class ACInputControlsDialogQt : public QDialog, public ACAbstractWidgetQt
{
    Q_OBJECT

public:
    ACInputControlsDialogQt(QWidget *pParent = NULL);
    ~ACInputControlsDialogQt();
    void addActions( QList<QAction *> _actions );
    void addInputActions( QList<ACInputActionQt *> _actions );

public slots:
    void on_pushButtonAddTag_clicked();
    void on_pushButtonRemoveTags_clicked();
    void tagMedia(QString tag);
    bool isShortcutAssigned(QString shortcut);

signals:
    void clearShortcut();

protected slots:
    void actionActivated(QTableWidgetItem *);
    void actionChanged(QTableWidgetItem *);
    void accept();
    void reject();

private:
    Ui::ACInputControlsDialogQt ui;
    QList<QAction *> actions;
    QSignalMapper* tagMapper;
    int iDirtyCount;
    QWidget* parent;
};
#endif
