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
    :ACAbstractWidgetQt(), mainWindow(_mainWindow),plugins_scanned(false),media_type(MEDIA_TYPE_NONE),auto_connect_osc(false),
      current_docks_visibility(true),previous_docks_visibility(true),appOrigMinHeight(0)
{
    dockWidgets.resize(0);
    dockWidgetFactory = new ACDockWidgetFactoryQt();

    appOrigMinHeight = mainWindow->minimumHeight();
    updateDockHeight();
}

ACDockWidgetsManagerQt::~ACDockWidgetsManagerQt()
{
    delete dockWidgetFactory;
    vector<ACAbstractDockWidgetQt*>::iterator dwiter;
    for (dwiter=dockWidgets.begin(); dwiter!=dockWidgets.end(); dwiter++)
        delete *dwiter;
}

void ACDockWidgetsManagerQt::setMediaCycle(MediaCycle* _media_cycle)
{
    this->media_cycle = _media_cycle;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setMediaCycle(media_cycle);
    }
}

#if defined (SUPPORT_AUDIO)
void ACDockWidgetsManagerQt::setAudioEngine(ACAudioEngine* _audio_engine)
{
    this->audio_engine = _audio_engine;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setAudioEngine(audio_engine);
    }
}
#endif //defined (SUPPORT_AUDIO)

void ACDockWidgetsManagerQt::setOsgView(ACOsgCompositeViewQt* _osg_view)
{
    this->osg_view = _osg_view;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setOsgView(_osg_view);
    }
}

#if defined (USE_OSC)
void ACDockWidgetsManagerQt::autoConnectOSC(bool _status)
{
    auto_connect_osc = _status;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->autoConnectOSC(_status);//CF quick hack for LoopJam, should be (_status);
    }
}

void ACDockWidgetsManagerQt::setOscBrowser(ACOscBrowser* _browser)
{
    this->osc_browser = _browser;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setOscBrowser(_browser);
    }
}

void ACDockWidgetsManagerQt::setOscFeedback(ACOscFeedback* _feedback)
{
    this->osc_feedback = _feedback;
    for (int d=0;d<dockWidgets.size();d++){
        dockWidgets[d]->setOscFeedback(_feedback);
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

    dockWidgets.push_back(dock);

    if(!(mainWindow->isFullScreen()))
    {
        //if( dock->getMediaType() == MEDIA_TYPE_ALL || dock->getMediaType() == media_type || dock->getMediaType() == MEDIA_TYPE_MIXED ){
        if( dock->canBeVisible(media_type) ){
            lastDocksVisibilities.insert(std::pair<std::string,int>( dock->getClassName(),1));
            mainWindow->addDockWidget(Qt::LeftDockWidgetArea,dockWidgets.back());
            dockWidgets.back()->setVisible(true);
            connect(dockWidgets.back(), SIGNAL(visibilityChanged(bool)), this , SLOT(syncControlToggleWithDocks()));
        }
        else {
            lastDocksVisibilities.insert(std::pair<std::string,int>( dock->getClassName(),0));
            dockWidgets.back()->setVisible(false);
        }
    }
    else{
        lastDocksVisibilities.insert(std::pair<std::string,int>( dock->getClassName(),0 ));
        dockWidgets.back()->setVisible(false);
    }
    connect(dockWidgets.back(), SIGNAL(libraryMediaTypeChanged(QString)), mainWindow, SLOT(changeLibraryMediaType(QString)));
#ifdef SUPPORT_MULTIMEDIA
    connect(dockWidgets.back(), SIGNAL(activeMediaTypeChanged(QString)), mainWindow, SLOT(changeActiveMediaType(QString)));
#endif

#if defined (USE_OSC)
    dockWidgets.back()->autoConnectOSC(auto_connect_osc);
    dockWidgets.back()->setOscBrowser(this->osc_browser);
    dockWidgets.back()->setOscFeedback(this->osc_feedback);
#endif //defined (USE_OSC)

    this->updateDockHeight();

    return true;
}

bool ACDockWidgetsManagerQt::addControlDock(std::string dock_type)
{
    this->addControlDock(dockWidgetFactory->createDockWidget(mainWindow,dock_type));
}

ACAbstractDockWidgetQt* ACDockWidgetsManagerQt::getDockFromFactoryName(std::string _name){
    std::string className("");
    if (_name == "MCOSC") className = "ACOSCDockWidgetQt";
    else if (_name == "MCBrowserControlsComplete") className = "ACBrowserControlsCompleteDockWidgetQt";
    else if (_name == "MCBrowserControlsClusters") className = "ACBrowserControlsClustersDockWidgetQt";
#if defined (SUPPORT_AUDIO)
    else if (_name == "MCAudioControls") className = "ACAudioControlsDockWidgetQt";
#endif //defined (SUPPORT_AUDIO)
    else if (_name == "MCMediaConfig") className = "ACMediaConfigDockWidgetQt";
#if defined (SUPPORT_VIDEO)
    else if (_name == "MCVideoControls") className = "ACVideoControlsDockWidgetQt";
#endif //defined (SUPPORT_VIDEO)
#if defined (SUPPORT_MULTIMEDIA)
    else if (_name == "MCMediaDocumentOption") className = "ACMediaDocumentOptionDockWidgetQt";
#endif //defined (SUPPORT_MULTIMEDIA)
    else if (_name == "MCSegmentationControls") className = "ACSegmentationControlsDockWidgetQt";
    return this->getDockFromClassName(className);
}

ACAbstractDockWidgetQt* ACDockWidgetsManagerQt::getDockFromClassName(std::string _name){
    if( _name == "")
        return 0;

    for(std::vector<ACAbstractDockWidgetQt*>::iterator dockWidget = dockWidgets.begin(); dockWidget != dockWidgets.end(); dockWidget++){
        if((*dockWidget)->getClassName() == _name)
            return (*dockWidget);
    }
    return 0;
}

void ACDockWidgetsManagerQt::updateDocksVisibility(bool visibility)
{
    this->current_docks_visibility = visibility;
    this->syncControlToggleWithDocks();
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::updateDockHeight()
{
    if(!mainWindow)
        return;
    if (mainWindow->isFullScreen())
        return;

    mainWindow->setMinimumHeight(appOrigMinHeight);
    int appWinNum = QApplication::desktop()->screenNumber(mainWindow);
    int availHeight = QApplication::desktop()->availableGeometry(appWinNum).height();

    int windowHeight = 0;
    int windowWidth = 0;
    for (int d=0;d<dockWidgets.size();d++){
        if(dockWidgets[d]->isVisible() && mainWindow->tabifiedDockWidgets(dockWidgets[d]).isEmpty() ){
            if(dockWidgets[d]->widget()){
                if (mainWindow->minimumHeight() + dockWidgets[d]->widget()->minimumHeight() < availHeight){
                    //mainWindow->setMinimumHeight( mainWindow->minimumHeight() + dockWidgets[d]->minimumHeight() );
                    windowHeight += dockWidgets[d]->widget()->minimumHeight() + 32;
                }

            }
            //std::cout << "ACDockWidgetsManagerQt::updateDockHeight: dock " << dockWidgets[d]->getClassName() << " of height " << dockWidgets[d]->minimumHeight() << " / window height " << mainWindow->minimumHeight() << " / availHeight " << availHeight << std::endl;
        }
    }

    // if the available height is too short, alternate media-specific controls and browser controls
    /*if(windowHeight>appOrigMinHeight){
        //std::cout << "ACDockWidgetsManagerQt::updateDockHeight: media-specific and browsing controls are alternated" << std::endl;
        std::string media_class_name("");
        if(this->media_type == MEDIA_TYPE_AUDIO)
            media_class_name = "ACAudioControlsDockWidgetQt";
        else if(this->media_type == MEDIA_TYPE_VIDEO)
            media_class_name = "ACVideoControlsDockWidgetQt";
        if(media_class_name != ""){
            ACAbstractDockWidgetQt* browser_dock = 0;
            ACAbstractDockWidgetQt* media_dock = 0;
            browser_dock = this->getDockFromClassName("ACBrowserControlsCompleteDockWidgetQt");
            if(!browser_dock)
                browser_dock = this->getDockFromClassName("ACBrowserControlsClustersDockWidgetQt");
            media_dock = this->getDockFromClassName(media_class_name);
            if(browser_dock && media_dock){
                //std::cout << "Browser dock visible " << browser_dock->isVisible() << std::endl;
                //std::cout << "Media dock visible " << media_dock->isVisible() << std::endl;
                mainWindow->tabifyDockWidget(media_dock,browser_dock);
                lastDocksVisibilities[media_class_name]=0;
            }
        }
    }*/

    if(windowHeight<appOrigMinHeight){
        windowHeight = appOrigMinHeight;
        windowWidth = appOrigMinHeight + 250;
    }
    else
        windowWidth = windowHeight + 250; // magic number, dock widgets width;
    mainWindow->setMinimumHeight(windowHeight + 3*26); // titlebar, toolbar, statusbar
   // mainWindow->setMinimumWidth(windowWidth);
    //mainWindow->resize(mainWindow->sizeHint());
    //std::cout << "window height " << mainWindow->minimumHeight() << " or " << windowHeight << " / availHeight " << availHeight << std::endl;
}

void ACDockWidgetsManagerQt::syncControlToggleWithDocks(){
    int docksVisibilitiesSum = 0;

    for (int d=0;d<dockWidgets.size();d++){
        //if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
        if( dockWidgets[d]->canBeVisible(media_type) )
            docksVisibilitiesSum += dockWidgets[d]->isVisible();
    }

    int lastDocksVisibilitiesSum = 0;
    for(std::map<std::string,int>::iterator v = lastDocksVisibilities.begin(); v != lastDocksVisibilities.end();v++)
        lastDocksVisibilitiesSum += v->second;

    if (current_docks_visibility){
        if (docksVisibilitiesSum == 0 && lastDocksVisibilitiesSum == 1 && previous_docks_visibility){
            current_docks_visibility = false;
            emit toggleControls(false);
        }
        else if (lastDocksVisibilitiesSum == 0 && !previous_docks_visibility){
            for (int d=0;d<dockWidgets.size();d++){
                //if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
                if( dockWidgets[d]->canBeVisible(media_type) )
                    dockWidgets[d]->setVisible(true);
            }
        }
        else {
            if (!previous_docks_visibility){
                for (int d=0;d<dockWidgets.size();d++){
                    //if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
                    if( dockWidgets[d]->canBeVisible(media_type) )
                        dockWidgets[d]->setVisible((bool)( lastDocksVisibilities[ dockWidgets[d]->getClassName() ]) );
                }
            }
        }
        for (int d=0;d<dockWidgets.size();d++){
            //if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
            if( dockWidgets[d]->canBeVisible(media_type) )
                lastDocksVisibilities[ dockWidgets[d]->getClassName() ]=dockWidgets[d]->isVisible();
        }
    }
    else {
        for (int d=0;d<dockWidgets.size();d++){
            //if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED)
            if( dockWidgets[d]->canBeVisible(media_type) )
                dockWidgets[d]->setVisible(false);
        }
    }
    previous_docks_visibility = current_docks_visibility;
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::changeMediaType(ACMediaType _media_type){
    this->media_type = _media_type;
    for (int d=0;d<dockWidgets.size();d++){

        dockWidgets[d]->changeMediaType(_media_type);

        //if (dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == _media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_MIXED){
        if( dockWidgets[d]->canBeVisible(_media_type) ){
            if (mainWindow->dockWidgetArea(dockWidgets[d]) == Qt::NoDockWidgetArea){
                mainWindow->addDockWidget(Qt::LeftDockWidgetArea,dockWidgets[d]);
                dockWidgets[d]->show();
                lastDocksVisibilities[ dockWidgets[d]->getClassName() ]=1;
                connect(dockWidgets[d], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
            }
        }
        else {
            disconnect(dockWidgets[d], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
            if (mainWindow->dockWidgetArea(dockWidgets[d]) == Qt::LeftDockWidgetArea){
                mainWindow->removeDockWidget(dockWidgets[d]);
            }
            lastDocksVisibilities[ dockWidgets[d]->getClassName() ]=0;
        }
    }
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

    // Hiding the OSC dock once a library is loaded
    for (int d=0;d<dockWidgets.size();d++){
        if(dockWidgets[d]->getClassName() == "ACOSCDockWidgetQt"){
            dockWidgets[d]->hide();
            lastDocksVisibilities[ dockWidgets[d]->getClassName() ]=0;
        }
    }
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::resetPluginsSettings()
{
    for (int d=0;d<dockWidgets.size();d++)
        dockWidgets[d]->resetPluginsSettings();
    plugins_scanned = false;
    this->updateDockHeight();
}

void ACDockWidgetsManagerQt::resetMediaType(ACMediaType _media_type)
{
    for (int d=0;d<dockWidgets.size();d++)
        dockWidgets[d]->resetMediaType(_media_type);
    this->updateDockHeight();
}
