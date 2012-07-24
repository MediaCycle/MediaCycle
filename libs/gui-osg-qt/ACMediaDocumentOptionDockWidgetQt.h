/**
 * @brief ACMediaDocumentOptionDockWidgetQt.h
 * @author Christian Frisson
 * @date 24/07/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#ifndef ACDOCKWIDGETMEDIADOCUMENTOPTION_H
#define ACDOCKWIDGETMEDIADOCUMENTOPTION_H

#include <QDockWidget>

#include "ACAbstractDockWidgetQt.h"
#include "ui_ACMediaDocumentOptionDockWidgetQt.h"
#include <MediaCycle.h>

#ifdef SUPPORT_MULTIMEDIA
#include "ACMediaDocument.h"
#endif//def SUPPORT_MULTIMEDIA

class ACMediaDocumentOptionDockWidgetQt : public ACAbstractDockWidgetQt 
{
    Q_OBJECT

public:
    explicit ACMediaDocumentOptionDockWidgetQt(QWidget *parent = 0);
    ~ACMediaDocumentOptionDockWidgetQt();
    virtual bool canBeVisible(ACMediaType _media_type);

private slots:
    void changeMediaType(QString name);
public:
#ifdef SUPPORT_MULTIMEDIA
    virtual void updatePluginsSettings();
    virtual void resetPluginsSettings();
#endif//def SUPPORT_MULTIMEDIA
private:
    Ui::ACMediaDocumentOptionDockWidgetQt ui;
    bool initOn;
};

#endif // ACDOCKWIDGETMEDIADOCUMENTOPTION_H
