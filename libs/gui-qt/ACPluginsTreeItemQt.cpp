/*
 *  ACPluginsTreeItemQt.cpp
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

#include "ACPluginsTreeItemQt.h"

#include <QStringList>

ACPluginsTreeItemQt::ACPluginsTreeItemQt(const QVector<QVariant> &data, ACPluginsTreeItemQt *parent){
    parentItem = parent;
    itemData = data;
}

ACPluginsTreeItemQt::~ACPluginsTreeItemQt() {
    qDeleteAll(childItems);
}

void ACPluginsTreeItemQt::appendChild(ACPluginsTreeItemQt *item) {
    childItems.append(item);
}

int ACPluginsTreeItemQt::childCount() const {
    return childItems.count();
}

int ACPluginsTreeItemQt::columnCount() const {
    return itemData.count();
}

QVariant ACPluginsTreeItemQt::data(int column) const {
    return itemData.value(column);
}



ACPluginsTreeItemQt *ACPluginsTreeItemQt::parent() {
    return parentItem;
}

ACPluginsTreeItemQt *ACPluginsTreeItemQt::child(int number) {
    return childItems.value(number);
}

bool ACPluginsTreeItemQt::setData(int column, const QVariant &value) {
    if (column < 0 || column >= itemData.size())
        return false;
	
    itemData[column] = value;
    return true;
}

bool ACPluginsTreeItemQt::removeChildren(int position, int count) {
    if (position < 0 || position + count > childItems.size())
        return false;
	
    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);
	
    return true;
}

bool ACPluginsTreeItemQt::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;
	
    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        ACPluginsTreeItemQt *item = new ACPluginsTreeItemQt(data, this);
        childItems.insert(position, item);
    }
	
    return true;
}


bool ACPluginsTreeItemQt::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;
	
    for (int column = 0; column < columns; ++column)
        itemData.remove(position);
	
    foreach (ACPluginsTreeItemQt *child, childItems)
	child->removeColumns(position, columns);
	
    return true;
}

int ACPluginsTreeItemQt::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<ACPluginsTreeItemQt*>(this));
	
    return 0;
}
