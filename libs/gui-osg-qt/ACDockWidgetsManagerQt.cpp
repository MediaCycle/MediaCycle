/*
 *  ACDockWidgetsManagerQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 20/02/11
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

#include "ACDockWidgetsManagerQt.h"

ACDockWidgetsManagerQt::ACDockWidgetsManagerQt(QMainWindow *_mainWindow)
    :mainWindow(_mainWindow),plugins_scanned(false),media_type(MEDIA_TYPE_NONE),auto_connect_osc(false),
      current_docks_visibility(true),previous_docks_visibility(true),appOrigMinHeight(0)
{
    dockWidgets.resize(0);
    dockWidgetFactory = new ACDockWidgetFactoryQt();
    lastDocksVisibilities.resize(0);

    appOrigMinHeight = mainWindow->minimumHeight();
}

ACDockWidgetsManagerQt::~ACDockWidgetsManagerQt()
{
    delete dockWidgetFactory;
    vector<ACAbstractDockWidgetQt*>::iterator dwiter;
    for (dwiter=dockWidgets.begin(); dwiter!=dockWidgets.end(); dwiter++)
        delete *dwiter;
}

void ACDockWidgetsManagerQt::updateMediaCycle(MediaCycle* media_cycle)
{
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setMediaCycle(media_cycle);
    }
}

#if defined (SUPPORT_AUDIO)
void ACDockWidgetsManagerQt::updateAudioEngine(ACAudioEngine* audio_engine)
{
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setAudioEngine(audio_engine);
    }
}
#endif //defined (SUPPORT_AUDIO)

void ACDockWidgetsManagerQt::updateOsgView(ACOsgCompositeViewQt* compositeOsgView)
{
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setOsgView(compositeOsgView);
    }
}

void ACDockWidgetsManagerQt::setMediaType(ACMediaType _mt)
{
    this->media_type = _mt;
}

#if defined (USE_OSC)
void ACDockWidgetsManagerQt::autoConnectOSC(bool _status)
{
    auto_connect_osc = _status;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->autoConnectOSC(_status);
    }
}
#endif //defined (USE_OSC)

bool ACDockWidgetsManagerQt::addControlDock(ACAbstractDockWidgetQt* dock)
{
    if (dock == 0){
        std::cerr << "Invalid dock widget" << std::endl;
        return false;
    }

    if (dock->getMediaType() == MEDIA_TYPE_NONE && dock->getClassName()==""){
        std::cerr << "Invalid dock widget type" << std::endl;
        return false;
    }

    for (int d=0;d<dockWidgets.size();d++){
        if (dock->getClassName() == dockWidgets[d]->getClassName()){
            std::cerr << "Dock widget type already added" << std::endl;
            return false;
        }
    }

    if (dock->getClassName() == "ACBrowserControlsCompleteDockWidgetQt" || dock->getClassName() == "ACBrowserControlsClustersDockWidgetQt"){
        for (int d=0;d<dockWidgets.size();d++){
            if (dockWidgets[d]->getClassName() == "ACBrowserControlsCompleteDockWidgetQt" || dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt"){
                std::cerr << "Only one browser control dock widget for clusters and/or neighbors can be added." << std::endl;
                return false;
            }
        }
    }

    //XS TODO check indices -- or use push_back
    dockWidgets.resize(dockWidgets.size()+1);
    lastDocksVisibilities.resize(lastDocksVisibilities.size()+1);

    dockWidgets.back()=dock;

    if( dock->getMediaType() == MEDIA_TYPE_ALL || dock->getMediaType() == media_type || dock->getMediaType() == MEDIA_TYPE_MIXED ){
        lastDocksVisibilities.back()=1;
        mainWindow->addDockWidget(Qt::LeftDockWidgetArea,dockWidgets.back());
        dockWidgets.back()->setVisible(true);
        connect(dockWidgets.back(), SIGNAL(visibilityChanged(bool)), this , SLOT(syncControlToggleWithDocks()));
    }
    else {
        lastDocksVisibilities.back()=0;
        dockWidgets.back()->setVisible(false);
    }

    connect(dockWidgets.back(), SIGNAL(mediaTypeChanged(QString)), mainWindow, SLOT(comboDefaultSettingsChanged(QString)));

    dockWidgets.back()->autoConnectOSC(auto_connect_osc);

    mainWindow->setMinimumHeight(appOrigMinHeight);
    int appWinNum = QApplication::desktop()->screenNumber(mainWindow);
    int availHeight = QApplication::desktop()->availableGeometry(appWinNum).height();
    int windowHeight = 0;
    for (int d=0;d<dockWidgets.size();d++){
        if(dockWidgets[d]->isVisible()){
            if (mainWindow->minimumHeight() + dockWidgets[d]->minimumHeight() < availHeight){
                //mainWindow->setMinimumHeight( mainWindow->minimumHeight() + dockWidgets[d]->minimumHeight() );
                windowHeight += dockWidgets[d]->minimumHeight();
            }
            std::cout << "Dock " << dockWidgets[d]->getClassName() << " of height " << dockWidgets[d]->minimumHeight() << " / window height " << mainWindow->minimumHeight() << " / availHeight " << availHeight << std::endl;
        }
    }
    if(windowHeight<appOrigMinHeight)
        windowHeight = appOrigMinHeight;
    mainWindow->setMinimumHeight(windowHeight);
    mainWindow->resize(mainWindow->sizeHint());
    std::cout << "window height " << mainWindow->minimumHeight() << " or " << windowHeight << " / availHeight " << availHeight << std::endl;
    return true;
}

bool ACDockWidgetsManagerQt::addControlDock(std::string dock_type)
{
    this->addControlDock(dockWidgetFactory->createDockWidget(mainWindow,dock_type));
}

void ACDockWidgetsManagerQt::updateDocksVisibility(bool visibility)
{
    this->current_docks_visibility = visibility;
    this->syncControlToggleWithDocks();
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::updateDockHeight()
{
    mainWindow->setMinimumHeight(appOrigMinHeight);
    int appWinNum = QApplication::desktop()->screenNumber(mainWindow);
    int availHeight = QApplication::desktop()->availableGeometry(appWinNum).height();
    int windowHeight = 0;
    for (int d=0;d<dockWidgets.size();d++){
        if(dockWidgets[d]->isVisible()){
            if (mainWindow->minimumHeight() + dockWidgets[d]->minimumHeight() < availHeight){
                //mainWindow->setMinimumHeight( mainWindow->minimumHeight() + dockWidgets[d]->minimumHeight() );
                windowHeight += dockWidgets[d]->minimumHeight();
            }
            //std::cout << "Dock " << dockWidgets[d]->getClassName() << " of height " << dockWidgets[d]->minimumHeight() << " / window height " << mainWindow->minimumHeight() << " / availHeight " << availHeight << std::endl;
        }
    }
    if(windowHeight<appOrigMinHeight)
        windowHeight = appOrigMinHeight;
    mainWindow->setMinimumHeight(windowHeight);
    mainWindow->resize(mainWindow->sizeHint());
    //std::cout << "window height " << mainWindow->minimumHeight() << " or " << windowHeight << " / availHeight " << availHeight << std::endl;
}

void ACDockWidgetsManagerQt::syncControlToggleWithDocks(){
    int docksVisibilitiesSum = 0;

    for (int d=0;d<dockWidgets.size();d++){
        if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
            docksVisibilitiesSum += dockWidgets[d]->isVisible();
    }

    int lastDocksVisibilitiesSum = 0;
    for (int d=0; d<lastDocksVisibilities.size();d++)
        lastDocksVisibilitiesSum += lastDocksVisibilities[d];

    if (current_docks_visibility){
        if (docksVisibilitiesSum == 0 && lastDocksVisibilitiesSum == 1 && previous_docks_visibility){
            current_docks_visibility = false;
            emit toggleControls(false);
        }
        else if (lastDocksVisibilitiesSum == 0 && !previous_docks_visibility){
            for (int d=0;d<dockWidgets.size();d++){
                if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
                    dockWidgets[d]->setVisible(true);
            }
        }
        else {
            if (!previous_docks_visibility){
                for (int d=0;d<dockWidgets.size();d++){
                    if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
                        dockWidgets[d]->setVisible((bool)(lastDocksVisibilities[d]));
                }
            }
        }
        for (int d=0;d<dockWidgets.size();d++){
            if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
                lastDocksVisibilities[d]=dockWidgets[d]->isVisible();
        }
    }
    else {
        for (int d=0;d<dockWidgets.size();d++){
            if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
                dockWidgets[d]->setVisible(false);
        }
    }
    previous_docks_visibility = current_docks_visibility;
}

void ACDockWidgetsManagerQt::changeMediaType(ACMediaType _media_type){
    for (int d=0;d<dockWidgets.size();d++){

        dockWidgets[d]->changeMediaType(_media_type);

        if (dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == _media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED){
            if (mainWindow->dockWidgetArea(dockWidgets[d]) == Qt::NoDockWidgetArea){
                mainWindow->addDockWidget(Qt::LeftDockWidgetArea,dockWidgets[d]);
                dockWidgets[d]->show();
                lastDocksVisibilities[d]=1;
                connect(dockWidgets[d], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
            }
        }
        else {
            disconnect(dockWidgets[d], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
            if (mainWindow->dockWidgetArea(dockWidgets[d]) == Qt::LeftDockWidgetArea){
                mainWindow->removeDockWidget(dockWidgets[d]);
            }
            lastDocksVisibilities[d]=0;
        }
    }

    /*if(_media_type == MEDIA_TYPE_AUDIO){
        //void QMainWindow::tabifyDockWidget ( QDockWidget * first, QDockWidget * second )
        int o = -1;
        int a = -1;
        for (int d=0;d<dockWidgets.size();d++){
            if (dockWidgets[d]->getClassName()=="ACOSCDockWidgetQt")
                o = d;
            if (dockWidgets[d]->getClassName()=="ACAudioControlsDockWidgetQt")
                a = d;
        }
        if(o!=-1 && a !=-1)
            mainWindow->tabifyDockWidget(dockWidgets[a],dockWidgets[o]);
    }*/
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::updatePluginsSettings() {
    // do not re-scan the directory for plugins once they have been loaded
    if (!plugins_scanned)
    {
        for (int d=0;d<dockWidgets.size();d++)
            dockWidgets[d]->updatePluginsSettings();
    }
    plugins_scanned = true;
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::resetPluginsSettings()
{
    for (int d=0;d<dockWidgets.size();d++)
        dockWidgets[d]->resetPluginsSettings();
    plugins_scanned = false;

    // XS TODO : remove the boxes specific to the media that was loaded
    // e.g. ACAudioControlDockWidgets
    // modify the DockWidget's API to allow this
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::resetMediaType(ACMediaType _media_type)
{
    for (int d=0;d<dockWidgets.size();d++)
        dockWidgets[d]->resetMediaType(_media_type);
    this->updateDockHeight();
}
