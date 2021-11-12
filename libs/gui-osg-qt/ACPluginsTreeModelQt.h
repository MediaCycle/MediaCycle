/*
 *  ACPluginsTreeModelQt.h
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


#ifndef ACPluginsTreeModelQt_H
#define ACPluginsTreeModelQt_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class ACPluginsTreeItemQt;

class ACPluginsTreeModelQt : public QAbstractItemModel {
	Q_OBJECT
	
public:
    ACPluginsTreeModelQt(QObject *parent = 0);
//	ACPluginsTreeModelQt(const QString &data, QObject *parent = 0);
    ~ACPluginsTreeModelQt();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	
	bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
	bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

	// XS 
    void addRow(ACPluginsTreeItemQt *row);
    ACPluginsTreeItemQt* getRootItem() {return rootItem;}
    ACPluginsTreeItemQt *getItem(const QModelIndex &index) const;

private:
    ACPluginsTreeItemQt *rootItem;
};

#endif
