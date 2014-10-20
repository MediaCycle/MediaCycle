/*
 *  ACPluginsTreeItemQt.h
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

#ifndef ACPluginsTreeItemQt_H
#define ACPluginsTreeItemQt_H

#include <QList>
#include <QVariant>
#include <QVector>

class ACPluginsTreeItemQt {
public:
    ACPluginsTreeItemQt(const QVector<QVariant> &data, ACPluginsTreeItemQt *parent = 0);
    ~ACPluginsTreeItemQt();
	
    void appendChild(ACPluginsTreeItemQt *child);
	
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool setData(int column, const QVariant &value);

	bool insertChildren(int position, int count, int columns);
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
	
	int row() const;
    ACPluginsTreeItemQt *parent();
    ACPluginsTreeItemQt *child(int number);

private:
    QList<ACPluginsTreeItemQt*> childItems;
    QVector<QVariant> itemData;
    ACPluginsTreeItemQt *parentItem;
};
#endif //ACPluginsTreeItemQt_H
