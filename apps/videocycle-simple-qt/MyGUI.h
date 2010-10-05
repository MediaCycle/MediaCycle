/**
 * @brief MyGUI.h
 * @author Alexis Moinet
 * @date 05/10/2010
 * @copyright (c) 2010 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#ifndef MYGUI_H
#define MYGUI_H

#include <QtGui/QMainWindow>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "ui_VideoCycle.h"

class MyGUI : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT

public:
    MyGUI(QWidget* parent = 0);
    virtual ~MyGUI();

public slots: // or private ? or protected ?
	void BrowseVideoDir();
	void BrowseOutputFile();
	void Calculate();
};

#endif // MYGUI_H
