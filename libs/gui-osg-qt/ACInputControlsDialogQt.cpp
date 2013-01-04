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

ACInputControlsDialogQt::ACInputControlsDialogQt ( QWidget *pParent ) : QDialog(pParent), ACAbstractWidgetQt()
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
    ui.ShortcutTable->horizontalHeader()->resizeSection(2, 60);
    ui.ShortcutTable->horizontalHeader()->resizeSection(3, 60);
    ui.ShortcutTable->horizontalHeader()->resizeSection(4, 60);

    int iRowHeight = ui.ShortcutTable->fontMetrics().height() + 4;
    ui.ShortcutTable->verticalHeader()->setDefaultSectionSize(iRowHeight);
    ui.ShortcutTable->verticalHeader()->hide();

    // addActions() previously here

    // Restore last seen form position and extents...
    // here...


    /*
    QObject::connect(ui.ShortcutTable,
        SIGNAL(itemActivated(QTableWidgetItem *)),
        SLOT(actionActivated(QTableWidgetItem *)));

    QObject::connect(ui.ShortcutTable,
        SIGNAL(itemChanged(QTableWidgetItem *)),
        SLOT(actionChanged(QTableWidgetItem *)));
*/
    QObject::connect(ui.DialogButtonBox,
                     SIGNAL(accepted()),
                     SLOT(accept()));
    QObject::connect(ui.DialogButtonBox,
                     SIGNAL(rejected()),
                     SLOT(reject()));

    tagMapper = new QSignalMapper(this);
}

ACInputControlsDialogQt::~ACInputControlsDialogQt (void)
{
    // Store form position and extents...
    // here..
}

void ACInputControlsDialogQt::addActions( QList<QAction *> _actions ){

    QObject::disconnect(ui.ShortcutTable,
                        SIGNAL(itemActivated(QTableWidgetItem *)),
                        this,
                        SLOT(actionActivated(QTableWidgetItem *)));

    QObject::disconnect(ui.ShortcutTable,
                        SIGNAL(itemChanged(QTableWidgetItem *)),
                        this,
                        SLOT(actionChanged(QTableWidgetItem *)));

    //int iRow = 0;
    int iRow = this->actions.size();

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
        ui.ShortcutTable->setItem(iRow, 3,
                                  new ACInputControlsTableItemQt(QString("")));
        ui.ShortcutTable->setItem(iRow, 4,
                                  new ACInputControlsTableItemQt(QString("")));
        ui.ShortcutTable->setItem(iRow, 5,
                                  new ACInputControlsTableItemQt(QString("")));
        this->actions.append(pAction);
        ++iRow;
    }

    // Hack (otherwise InputActions won't fill in)
    /*QObject::connect(ui.ShortcutTable,
        SIGNAL(itemActivated(QTableWidgetItem *)),
        SLOT(actionActivated(QTableWidgetItem *)));

    QObject::connect(ui.ShortcutTable,
        SIGNAL(itemChanged(QTableWidgetItem *)),
        SLOT(actionChanged(QTableWidgetItem *)));*/
}

void ACInputControlsDialogQt::addInputActions( QList<ACInputActionQt *> _actions ){

    QObject::disconnect(ui.ShortcutTable,
                        SIGNAL(itemActivated(QTableWidgetItem *)),
                        this,
                        SLOT(actionActivated(QTableWidgetItem *)));

    QObject::disconnect(ui.ShortcutTable,
                        SIGNAL(itemChanged(QTableWidgetItem *)),
                        this,
                        SLOT(actionChanged(QTableWidgetItem *)));

    int iRow = this->actions.size();

    QList<QAction* > _acts;
    QListIterator<ACInputActionQt *> _iter(_actions);
    while (_iter.hasNext())
        _acts.append( dynamic_cast <QAction*> (_iter.next()) );
    this->addActions(_acts);
    QListIterator<ACInputActionQt *> iter(_actions);
    while (iter.hasNext()) {
        ACInputActionQt *pAction = iter.next();
        if (pAction->menu() || pAction->text().isEmpty())
            continue;
        QComboBox* combo = new QComboBox();
        combo->addItems( pAction->getMouseEventNames() );
        int _index = combo->findText( pAction->getMouseEventName() );
        combo->setEnabled(false);
        ui.ShortcutTable->setCellWidget(iRow,3,combo);
        combo->setCurrentIndex(_index);
        //connect(combo, SIGNAL(currentIndexChanged(int)), signalMapper, SLOT(map()));
        //if(this->isShortcutAssigned( ui.ShortcutTable->item(iRow, 2)->text() ))
        //connect(ui.ShortcutTable->item(iRow,2),SIGNAL())

        QLabel* device_name = new QLabel();
        device_name->setText(pAction->getDeviceName());
        ui.ShortcutTable->setCellWidget(iRow,4,device_name);

        QLabel* device_event = new QLabel();
        device_event->setText(pAction->getDeviceEvent());
        ui.ShortcutTable->setCellWidget(iRow,5,device_event);

        ++iRow;
    }

    QObject::connect(ui.ShortcutTable,
                     SIGNAL(itemActivated(QTableWidgetItem *)),
                     SLOT(actionActivated(QTableWidgetItem *)));

    QObject::connect(ui.ShortcutTable,
                     SIGNAL(itemChanged(QTableWidgetItem *)),
                     SLOT(actionChanged(QTableWidgetItem *)));
}

void ACInputControlsDialogQt::on_pushButtonAddTag_clicked()
{
    if(!osg_view){
        std::cerr << "ACInputControlsDialogQt::on_pushButtonAddTag_clicked: requires an OSG composite view that isn't set, can't add tag." << std::endl;
        return;
    }

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Add tag");
    QGridLayout* layout = new QGridLayout(dialog);
    QComboBox* combo = new QComboBox();
    ACInputActionQt *pAction = new ACInputActionQt("");
    combo->addItems( pAction->getMouseEventNames() );
    int _index = combo->findText("Click");
    combo->setCurrentIndex(_index);
    combo->setEnabled(false);
    QLabel* tag = new QLabel("Tag");
    QLineEdit* tagName = new QLineEdit();
    QLabel* shortcut = new QLabel("Shortcut");
    QLineEdit* shortcutValue = new QLineEdit();
    shortcutValue->setMaxLength(1);
    QLabel* mouse = new QLabel("Mouse");
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
    connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));
    layout->addWidget(tag,0,0);
    layout->addWidget(tagName,0,1);
    layout->addWidget(shortcut,1,0);
    layout->addWidget(shortcutValue,1,1);
    layout->addWidget(mouse,2,0);
    layout->addWidget(combo,2,1);
    layout->addWidget(buttonBox,3,1);
    dialog->setLayout(layout);

    connect(shortcutValue,SIGNAL(textEdited(QString)),this,SLOT(isShortcutAssigned(QString)));
    connect(this,SIGNAL(clearShortcut()),shortcutValue,SLOT(clear()));

    if(dialog->exec()){
        if (tagName->text().isEmpty() || shortcutValue->text().isEmpty())
            return;
        //std::cout << "Adding tag " << tagName->text().toStdString() << " with shortcut " << shortcutValue->text().toStdString() << std::endl;
        pAction->setText(QString("Tag ") + tagName->text());
        pAction->setParent(this->osg_view);
        pAction->setKeyEventType(QEvent::KeyPress);
        pAction->setShortcut(QKeySequence(shortcutValue->text()));
        pAction->setMouseEventType(QEvent::MouseButtonRelease);
        tagMapper->setMapping(pAction,tagName->text());
        connect(pAction,SIGNAL(triggered()),tagMapper,SLOT(map()));
        connect(tagMapper,SIGNAL(mapped(QString)),this,SLOT(tagMedia(QString)));
        QList<ACInputActionQt *> _actions;
        _actions.append(pAction);
        this->addInputActions(_actions);
        osg_view->addInputAction(pAction);
    }
    disconnect(shortcutValue);
}

void ACInputControlsDialogQt::tagMedia(QString tag){
    if(!media_cycle) return;
    if(!media_cycle->hasBrowser()) return;
    if(media_cycle->getLibrarySize() == 0) return;
    int media_id = media_cycle->getClickedNode();
    if(media_id >= 0)
    {
        ACMedia* media = this->media_cycle->getLibrary()->getMedia(media_id);
        if(!media){
            std::cerr << "ACInputControlsDialogQt::tagMedia: no media of id " << media_id << std::endl;
            return;
        }
        media->setLabel(tag.toStdString());
        std::cout << "ACInputControlsDialogQt::tagMedia: tagged media of id " << media_id << " with tag " << tag.toStdString() << std::endl;
    }
}

bool ACInputControlsDialogQt::isShortcutAssigned(QString shortcut){
    if(shortcut.isEmpty())
        return false;
    QListIterator<QAction *> action(this->actions);
    while(action.hasNext()){
        QAction* act = action.next();
        if(act->shortcut().toString().toLower() == shortcut.toLower()){
            QMessageBox::warning(this, "Error", QString("Shortcut ") + shortcut + QString(" is already assigned to action ") + act->text());
            emit this->clearShortcut();
            return true;
        }
    }
    return false;
}

void ACInputControlsDialogQt::on_pushButtonRemoveTags_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui.ShortcutTable->selectedItems();
    QListIterator<QTableWidgetItem*> selectedItem(selectedItems);
    while (selectedItem.hasNext()) {
        QTableWidgetItem* item = selectedItem.next();
        if( ui.ShortcutTable->item(item->row(),0)->text().contains("Tag ") ){
            if( item->column() == ui.ShortcutTable->columnCount()-1){
                QListIterator<QAction *> action(this->actions);
                while(action.hasNext()){
                    QAction* act = action.next();
                    if(act->text() == ui.ShortcutTable->item(item->row(),0)->text()){
                        this->actions.removeOne(act);
                        tagMapper->removeMappings(act);
                    }
                }
                ui.ShortcutTable->removeRow(item->row());
            }
        }
    }
}

void ACInputControlsDialogQt::actionActivated ( QTableWidgetItem *pItem )
{
    if(pItem->column()==2){
        ui.ShortcutTable->editItem(ui.ShortcutTable->item(pItem->row(), 2));
    }
}

void ACInputControlsDialogQt::actionChanged ( QTableWidgetItem *pItem )
{
    //for (int iRow = 0; iRow < actions.count(); ++iRow) {
    int iRow = pItem->row();
        if(pItem->column()==2){
            if(!this->isShortcutAssigned( pItem->text() ))
                pItem->setText(QString(QKeySequence(pItem->text().trimmed())));
            else{
                pItem->setText("");
                return;
            }
            QAction *pAction = actions[iRow];
            pAction->setShortcut(
                        QKeySequence(ui.ShortcutTable->item(iRow, 2)->text()));
            //std::cout << "Shortcut '" << ui.ShortcutTable->item(iRow, 2)->text().toStdString() << "'" << std::endl;
        }
        else if(pItem->column()==3){
            ACInputActionQt *pAction = dynamic_cast<ACInputActionQt*>(actions[iRow]);
            pAction->setMouseEventType( pAction->convertMouseEventNameToType( ui.ShortcutTable->item(iRow, 3)->text() ) );
        }
    //}
            ++iDirtyCount;
}

void ACInputControlsDialogQt::accept (void)
{
    if (iDirtyCount > 0) {
        for (int iRow = 0; iRow < actions.count(); ++iRow) {
            QAction *pAction = actions[iRow];
            pAction->setShortcut(
                        QKeySequence(ui.ShortcutTable->item(iRow, 2)->text()));
            if ( ui.ShortcutTable->item(iRow, 3)->text() != ""){
                ACInputActionQt* pInputAction = dynamic_cast<ACInputActionQt*>(pAction);
                pInputAction->setMouseEventType( pInputAction->convertMouseEventNameToType( ui.ShortcutTable->item(iRow, 3)->text() ) );
            }
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
