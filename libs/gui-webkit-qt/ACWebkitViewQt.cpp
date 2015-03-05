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

#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include "ACWebkitViewQt.h"
#include <QMenu>

#include <iostream>
#include <sstream>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACWebkitViewQt::ACWebkitViewQt()
    : QWebView(), ACEventListener(), ACAbstractViewQt(), libraryWriterPluginName(""), libraryBaseFileName("")
{
    progress = 0;

    // Forces a clean cache, makes the display flicker when reloading, but is necessary to reload media data through JavaScript update calls
    this->settings()->setObjectCacheCapacities(0,0,0);

    // Allows to inspect the JavaScript code and see console logs
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    // Enable audio?
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::WebAudioEnabled, true);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    // No context menu
    //this->setContextMenuPolicy( Qt::PreventContextMenu );

    // Remove scroll bars
    this->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical,Qt::ScrollBarAlwaysOff );
    this->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal,Qt::ScrollBarAlwaysOff );

    connect(this, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(this, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(this, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(this, SIGNAL(needsUpdate()),SLOT(updateLibrary()));

    this->addAction(this->pageAction(QWebPage::Reload));

    jqueryAction = new QAction(this);
    jqueryAction->setText(tr("Test JQuery"));
    connect(jqueryAction, SIGNAL(triggered()), this, SLOT(testJQuery()));
}

void ACWebkitViewQt::setMediaLibraryWriterPluginName(QString _name)
{
    this->libraryWriterPluginName = _name;
}

void ACWebkitViewQt::setMediaLibraryBaseFileName(QString _name){
    this->libraryBaseFileName = _name;
}

/// relative to the MediaCycle root source path
/// for instance: 3rdparty/audio-gui-html5/index.html
void ACWebkitViewQt::setWebpageRelativeSourcePath(QString _name)
{
    string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif

    fs::path root_source_path( __FILE__ );
    fs::path webpage_file_path = fs::path( root_source_path.parent_path().parent_path().parent_path().string() + slash + _name.toStdString() );
    fs::path webpage_filename = webpage_file_path.filename();
    if ( !fs::exists( webpage_file_path ) )
    {
        throw runtime_error("ACWebkitViewQt::setMediaLibraryBaseFileName file or folder '" + webpage_file_path.string() + "' doesn't exist ");
        //std::cerr << "ACWebkitViewQt::setMediaLibraryBaseFileName file or folder '" << webpage_file_path.string() << "' doesn't exist " << std::endl;
        //return;
    }
    QString _webpageFilename = QString(webpage_filename.string().c_str());
    qDebug() << "_webpageFilename " << _webpageFilename;

    fs::path webpage_folder_path = webpage_file_path.parent_path();
    if ( !fs::exists( webpage_folder_path ) )
    {
        throw runtime_error("ACWebkitViewQt::setMediaLibraryBaseFileName file or folder '" + webpage_folder_path.string() + "' doesn't exist ");
        //std::cerr << "ACWebkitViewQt::setMediaLibraryBaseFileName file or folder '" << webpage_folder_path.string() << "' doesn't exist " << std::endl;
        //return;
    }
    QString _webpagePath = QString(webpage_folder_path.string().c_str());
    qDebug() << "_webpagePath " << _webpagePath;

    fs::path webpage_folder_name = webpage_folder_path.stem();
    QString _webpageFolderName = QString(webpage_folder_name.string().c_str());
    qDebug() << "_webpageFolderName " << _webpageFolderName;

#if defined(__APPLE__) && !defined(USE_DEBUG)
    fs::path executable_path( getExecutablePath() );
    fs::path resource_path = fs::path(executable_path.parent_path().parent_path().string() + "/Resources/" + webpage_folder_name.string());
    if ( !fs::exists( resource_path ) )
    {
        throw runtime_error("ACWebkitViewQt::setMediaLibraryBaseFileName file or folder '" + resource_path.string() + "' doesn't exist ");
        //std::cerr << "ACWebkitViewQt::setMediaLibraryBaseFileName file or folder '" << resource_path.string() << "' doesn't exist " << std::endl;
        //return;
    }
    webFolder = QString(resource_path.string().c_str());
#else
    webFolder = _webpagePath;
#endif

    mainUrl = QUrl("file://" + webFolder + QString(slash.c_str()) + _webpageFilename);
    qDebug() << "mainUrl " << mainUrl;

    // Init library file
    this->updateLibrary();

    this->load(mainUrl);
}

void ACWebkitViewQt::setMediaCycle(MediaCycle* _media_cycle)
{
    this->media_cycle = _media_cycle;
    media_cycle->addListener(this);
    emit this->needsUpdate();
}

void ACWebkitViewQt::mediaImported(int n, int nTot,int mId){
    /*if(n < nTot){

    }
    else if(n==nTot && mId==-1){

    }*/
    std::cout << "ACWebkitViewQt::mediaImported: " << n << "/" << nTot << " mId " << mId << std::endl;
    emit this->needsUpdate(); //calls this->updateLibrary();
}

void ACWebkitViewQt::updateDisplayNeeded(){
    std::cout << "ACWebkitViewQt::updateDisplayNeeded" << std::endl;
    emit this->needsUpdate(); //calls this->updateLibrary();
}

void ACWebkitViewQt::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = this->page()->createStandardContextMenu();
    menu->addAction(jqueryAction);

    menu->exec(QCursor::pos());
}

void ACWebkitViewQt::pluginLoaded(std::string plugin_name){
    if(media_cycle /*&& media_cycle->getLibrarySize()>0*/){
                if(plugin_name == "JSON export"){
            media_cycle->saveLibrary(webFolder.toStdString() + "library.json","JSON export");
        }
    }
}

void ACWebkitViewQt::updateLibrary(){

    if(media_cycle /*&& media_cycle->getLibrarySize()>0*/){
        media_cycle->saveLibrary(webFolder.toStdString() + "/"+ libraryBaseFileName.toStdString(),libraryWriterPluginName.toStdString());
    }

    QString code = "updateData()";
    QVariant result = this->page()->mainFrame()->evaluateJavaScript(code);
    qDebug() << result.toString();

    //CF page reloading makes the view flicker
    //this->reload();
    //this->triggerPageAction(QWebPage::Reload);
}

void ACWebkitViewQt::adjustLocation()
{
    //locationEdit->setText(this->url().toString());
}

void ACWebkitViewQt::testJQuery(){
    QString code = "updateData()";
    this->page()->mainFrame()->evaluateJavaScript(code);
}

void ACWebkitViewQt::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle(this->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(this->title()).arg(progress));
}

void ACWebkitViewQt::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void ACWebkitViewQt::finishLoading(bool)
{
    progress = 100;
    adjustTitle();
}
