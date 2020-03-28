/**
 * @brief View wrapping a QWebView
 * @author Christian Frisson
 * @date 22/04/2014
 * @copyright (c) 2014 – UMONS - Numediart
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

#ifndef HEADER_ACWebkitViewQt
#define HEADER_ACWebkitViewQt

#include <QtGui>

//class QWebView;
#include <QWebView>
#include <QWebFrame>

#include "ACAbstractViewQt.h"
#include "ACEventListener.h"

class QLineEdit;

class ACWebkitViewQt : public QWebView, public ACEventListener, public ACAbstractViewQt
{
    Q_OBJECT

public:
    ACWebkitViewQt();

    // MediaCycle listener callback
    virtual void mediaImported(int n,int nTot,int mId);
    virtual void updateDisplayNeeded();
    virtual void pluginLoaded(std::string plugin_name);

    void setMediaCycle(MediaCycle* _media_cycle);
signals:
    void needsUpdate();

protected slots:
    void testJQuery();
    void adjustLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);
    void updateLibrary();

public:
    void setMediaLibraryWriterPluginName(QString _name);
    void setMediaLibraryBaseFileName(QString _name);
    void setWebpageRelativeSourcePath(QString _name); /// relative to the MediaCycle root source path

public:
    virtual bool isLibraryLoaded(){return true;}
    virtual void setLibraryLoaded(bool load_status){}
    virtual void prepareBrowser(){}
    virtual void prepareTimeline(){}
    ACAbstractBrowserRenderer* getBrowser(){return 0;}
    ACAbstractTimelineRenderer* getTimeline(){return 0;}
    virtual void clean(/*bool updategl=true*/);

    virtual void contextMenuEvent(QContextMenuEvent *);

private:
    QUrl mainUrl;
    QLineEdit *locationEdit;
    QAction *rotateAction;
    int progress;
    QString webFolder;
    QAction* jqueryAction;
    QString libraryWriterPluginName;
    QString libraryBaseFileName;
};
#endif
