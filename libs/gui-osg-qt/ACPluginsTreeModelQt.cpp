/*
 *  ACPluginsTreeModelQt.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 14/02/11
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

#include <QtGui>

#include "ACPluginsTreeItemQt.h"
#include "ACPluginsTreeModelQt.h"

ACPluginsTreeModelQt::ACPluginsTreeModelQt(QObject *parent)
: QAbstractItemModel(parent) {
    QVector<QVariant> rootData;
    rootData << "Plugin Name" << "Slider Number";
    rootItem = new ACPluginsTreeItemQt(rootData);
}

//ACPluginsTreeModelQt::ACPluginsTreeModelQt(const QString &data, QObject *parent)
//: QAbstractItemModel(parent) {
//    QVector<QVariant> rootData;
//    rootData << "Title" << "Summary";
//    rootItem = new ACPluginsTreeItemQt(rootData);
//    setupModelData(data.split(QString("\n")), rootItem);
//}

ACPluginsTreeModelQt::~ACPluginsTreeModelQt() {
    delete rootItem;
}

int ACPluginsTreeModelQt::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<ACPluginsTreeItemQt*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant ACPluginsTreeModelQt::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();
	
    if (role != Qt::DisplayRole)
        return QVariant();
	
    ACPluginsTreeItemQt *item = static_cast<ACPluginsTreeItemQt*>(index.internalPointer());
	
    return item->data(index.column());
}

bool ACPluginsTreeModelQt::setData(const QModelIndex &index, const QVariant &value,
                        int role)
{
    if (role != Qt::EditRole)
        return false;
	
    ACPluginsTreeItemQt *item = getItem(index);
    bool result = item->setData(index.column(), value);
	
    if (result)
        emit dataChanged(index, index);
	
    return result;
}


Qt::ItemFlags ACPluginsTreeModelQt::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return 0;
	
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ACPluginsTreeModelQt::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);
	
    return QVariant();
}

QModelIndex ACPluginsTreeModelQt::index(int row, int column, const QModelIndex &parent)
const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
	
    ACPluginsTreeItemQt *parentItem;
	
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ACPluginsTreeItemQt*>(parent.internalPointer());
	
    ACPluginsTreeItemQt *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ACPluginsTreeModelQt::parent(const QModelIndex &index) const{
    if (!index.isValid())
        return QModelIndex();
	
    ACPluginsTreeItemQt *childItem = static_cast<ACPluginsTreeItemQt*>(index.internalPointer());
    ACPluginsTreeItemQt *parentItem = childItem->parent();
	
    if (parentItem == rootItem)
        return QModelIndex();
	
    return createIndex(parentItem->row(), 0, parentItem);
}

int ACPluginsTreeModelQt::rowCount(const QModelIndex &parent) const {
    ACPluginsTreeItemQt *parentItem;
    if (parent.column() > 0)
        return 0;
	
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ACPluginsTreeItemQt*>(parent.internalPointer());
	
    return parentItem->childCount();
}

bool ACPluginsTreeModelQt::insertRows(int position, int rows, const QModelIndex &parent)
{
    ACPluginsTreeItemQt *parentItem = getItem(parent);
    bool success;
	
    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();
	
    return success;
}

bool ACPluginsTreeModelQt::removeRows(int position, int rows, const QModelIndex &parent)
{
    ACPluginsTreeItemQt *parentItem = getItem(parent);
    bool success = true;
	
    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();
	
    return success;
}

ACPluginsTreeItemQt *ACPluginsTreeModelQt::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        ACPluginsTreeItemQt *item = static_cast<ACPluginsTreeItemQt*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}



void ACPluginsTreeModelQt::addRow(ACPluginsTreeItemQt *row){
	QModelIndex index = QModelIndex(); //this->index(0,0);
	int r = rowCount();
	// XS comment
	// beginInsertRows and endInsertRows are necessary to update the view.
	// note quite sure about the indices though...
	beginInsertRows(index, r, r);
	rootItem->appendChild(row);
	endInsertRows();

}



//void ACPluginsTreeModelQt::setupModelData(const QStringList &lines, ACPluginsTreeItemQt *parent){
//    QList<ACPluginsTreeItemQt*> parents;
//    QList<int> indentations;
//    parents << parent;
//    indentations << 0;
//	
//    int number = 0;
//	
//    while (number < lines.count()) {
//        int position = 0;
//        while (position < lines[number].length()) {
//            if (lines[number].mid(position, 1) != " ")
//                break;
//            position++;
//        }
//		
//        QString lineData = lines[number].mid(position).trimmed();
//		
//        if (!lineData.isEmpty()) {
//            // Read the column data from the rest of the line.
//            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
//            QList<QVariant> columnData;
//            for (int column = 0; column < columnStrings.count(); ++column)
//                columnData << columnStrings[column];
//			
//            if (position > indentations.last()) {
//                // The last child of the current parent is now the new parent
//                // unless the current parent has no children.
//				
//                if (parents.last()->childCount() > 0) {
//                    parents << parents.last()->child(parents.last()->childCount()-1);
//                    indentations << position;
//                }
//            } else {
//                while (position < indentations.last() && parents.count() > 0) {
//                    parents.pop_back();
//                    indentations.pop_back();
//                }
//            }
//			
//            // Append a new item to the current parent's list of children.
//            parents.last()->appendChild(new ACPluginsTreeItemQt(columnData, parents.last()));
//        }
//		
//        number++;
//    }
//}
