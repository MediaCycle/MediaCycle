/*
 *  pluginsTreeItem.cpp
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

#include "pluginsTreeItem.h"

#include <QStringList>

pluginsTreeItem::pluginsTreeItem(const QVector<QVariant> &data, pluginsTreeItem *parent){
    parentItem = parent;
    itemData = data;
}

pluginsTreeItem::~pluginsTreeItem() {
    qDeleteAll(childItems);
}

void pluginsTreeItem::appendChild(pluginsTreeItem *item) {
    childItems.append(item);
}

int pluginsTreeItem::childCount() const {
    return childItems.count();
}

int pluginsTreeItem::columnCount() const {
    return itemData.count();
}

QVariant pluginsTreeItem::data(int column) const {
    return itemData.value(column);
}

pluginsTreeItem *pluginsTreeItem::parent() {
    return parentItem;
}

pluginsTreeItem *pluginsTreeItem::child(int number)
{
    return childItems.value(number);
}


bool pluginsTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;
	
    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);
	
    return true;
}

bool pluginsTreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;
	
    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        pluginsTreeItem *item = new pluginsTreeItem(data, this);
        childItems.insert(position, item);
    }
	
    return true;
}


bool pluginsTreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;
	
    for (int column = 0; column < columns; ++column)
        itemData.remove(position);
	
    foreach (pluginsTreeItem *child, childItems)
	child->removeColumns(position, columns);
	
    return true;
}

int pluginsTreeItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<pluginsTreeItem*>(this));
	
    return 0;
}
