/*
 *  ACMediaCycleOsgQt.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 16/11/10
 *  @copyright (c) 2010 – UMONS - Numediart
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

#include "ACMediaCycleOsgQt.h"
#include "ACPluginQt.h"
#include <fstream>
//#include <iomanip> // for setw
//#include <cstdlib> // for atoi

#ifdef OSG_LIBRARY_STATIC
#include <osgViewer/GraphicsWindow>
USE_GRAPHICSWINDOW()
#endif

#include <osgDB/Registry>

#include "ACPluginControlsDockWidgetQt.h"

/*ACQProgressBar::ACQProgressBar(QWidget *parent)
    :   QProgressBar(parent){
    this->setMinimum(0);
    this->setFormat("Loading file %v/%m");// due to Apple standards the progress bar won't display this, it does under Ubuntu
}


void ACQProgressBar::loading_started()
{
    //std::cout << "ACQProgressBar::loading_started" << std::endl;
    this->reset();
    this->show();
}

void ACQProgressBar::loading_finished()
{
    //std::cout << "ACQProgressBar::loading_finished" << std::endl;
    this->reset();
    this->hide();
}

void ACQProgressBar::loading_file(int media_id, int dir_size)
{
    //std::cout << "ACQProgressBar::loading_file " << media_id << "/" << dir_size << std::endl;
    this->setMaximum(dir_size);
    this->setValue(media_id);
}*/

void ACMediaCycleOsgQt::mediaImported(int n,int nTot,int mId){
    std::cout << "ACMediaCycleOsgQt::mediaImported media id " << mId << " ("<< n << "/" << nTot << ")" << std::endl;
    std::string send = "";
    if (n==0) { //  && n!=nTot
        send = "Loading Directory...";
        emit mediacycle_message_changed(QString(send.c_str()));
        emit loading_started();
    }
    else if (n==nTot && mId==-1) { //
        send = "";
        emit mediacycle_message_changed(QString(send.c_str()));
        emit loading_finished();
        this->updateLibrary();

        //Threading problem if we change during the import
        if(media_cycle->getLibrary()->getParentIds().size()>=1){
            if(dockWidgetsManager)
                dockWidgetsManager->updatePluginsSettings();
        }
    }
    else if(n<nTot){
        //Threading problem if we change during the import
        //if(media_cycle->getLibrary()->getParentIds().size()==1){
        //	dockWidgetsManager->updatePluginsSettings();
        //}
        /*if (progressBar){
            emit loading_file(n,nTot);
            stringstream status_message;
            status_message << "Loading File " << n << "/" << nTot;
            send = status_message.str();
            emit mediacycle_message_changed(QString(send.c_str()));
        }*/
    }
}

void ACMediaCycleOsgQt::pluginLoaded(std::string plugin_name){
    if(!media_cycle){
        std::cout << "ACMediaCycleOsgQt::pluginLoaded: mediacycle not set" << std::endl;
        return;
    }
    if(!dockWidgetsManager){
        std::cout << "ACMediaCycleOsgQt::pluginLoaded: dock widgets manager not set" << std::endl;
        return;
    }
    QObject* qobject = dynamic_cast<QObject*>(media_cycle->getPluginManager()->getPlugin(plugin_name));
    if(!qobject){
        //std::cerr << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " not an ACPluginQt" << std::endl;
        return;
    }
    //std::cout << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " is a QObject" << std::endl;
    ACPluginQt* plugin = qobject_cast<ACPluginQt*>(qobject);
    if(!plugin){
        //std::cerr << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " not an ACPluginQt" << std::endl;
        return;
    }
    std::cout << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " is an ACPluginQt" << std::endl;
    /*std::string dockName = plugin->dockName();
    if(dockName!=""){
        std::cout << "ACMediaCycleOsgQt::pluginLoaded: adding dock " << dockName << " for plugin " << plugin_name << std::endl;
        dockWidgetsManager->addControlDock( plugin->createDock(this) );
    }*/
    //dockWidgetsManager->addControlDock( new ACPluginControlsDockWidgetQt(media_cycle->getPluginManager()->getPlugin(plugin_name)->getPluginType(),plugin->dockName(),this ));
    //dockWidgetsManager->addControlDock( media_cycle->getPluginManager()->getPlugin(plugin_name)->getPluginType());
}

// ----------- 

ACMediaCycleOsgQt::ACMediaCycleOsgQt(QWidget *parent)
    : QMainWindow(parent),ACEventListener(),features_known(false),
      aboutDialog(0),controlsDialog(0),view(0),
      osgViewDockWidget(0),osgViewDockLayout(0),/*progressBar(0),*/metadataWindow(0),
      dockWidgetsManager(0),
      userProfileWindow(0),segmentationDialog(0),librarySaveDialog(0)
{
    ui.setupUi(this); // first thing to do
    this->media_type = MEDIA_TYPE_NONE;
    this->browser_mode = AC_MODE_NONE;
    this->media_cycle = 0;
    media_cycle = new MediaCycle(this->media_type);
    this->config_file_xml = "";
    this->project_directory = QApplication::applicationDirPath().append(QDir::separator()).toStdString();

    this->use_segmentation_default = false;
    this->use_segmentation_current = false;
    this->use_feature_extraction = true;
    this->use_visualization_plugins = true;
    this->setting = AC_SETTING_DESKTOP;

    // Apple bundled *.app, just look for bundled osg plugins
#ifndef USE_DEBUG
#if defined __APPLE__ and not defined (XCODE)
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("PlugIns");
    std::cout << "OSG plugins path" << dir.absolutePath().toStdString() << std::endl;
    osgDB::Registry::instance()->setLibraryFilePathList(dir.absolutePath().toStdString());
#endif
#endif

    ui.menubar->hide();
    ui.statusbar->hide();
    ui.toolbar->hide();

    //dockWidgetsManager = new ACDockWidgetsManagerQt(this);

    aboutDialogFactory = new ACAboutDialogFactoryQt();
    //settingsDialog = new ACSettingsDialogQt(this);

    if(dockWidgetsManager){
        connect( dockWidgetsManager, SIGNAL( toggleControls(bool)), ui.actionToggle_Controls, SLOT(setChecked(bool)));
        connect( ui.actionAnchor_Controls, SIGNAL(triggered(bool)), dockWidgetsManager, SLOT(anchorDocks(bool)));
    }

    // This is required to populate the available file extensions list at startup
    // until we clean mediacycle instead of deleting/creating it at every media type change.
    ACMediaFactory::getInstance();
    ACMediaFactory::getInstance().useRendering(true);
    // Since it is time consuming, we might want to add a splash screen with progress bar at startup?

    osgViewDockWidget = new QWidget; // it seems this intermediary widget is required to set a layout to a dock widget
    osgViewDockLayout = new QHBoxLayout;
    osgViewDockLayout->setSpacing(0); // no blank space between the progress bar and the osg view
    osgViewDockLayout->setContentsMargins(0,0,0,0);// no unnecessary corners in the osg view dock widget (this supersedes the OS theme defaults)
    osgViewDockWidget->setLayout(osgViewDockLayout);

    QWidget* stackWidget = new QWidget; // it seems this intermediary widget is required to set a layout to a dock widget
    QVBoxLayout *stackLayout = new QVBoxLayout();
    stackLayout->setSpacing(0); // no blank space between the progress bar and the osg view
    stackLayout->setContentsMargins(0,0,0,0);// no unnecessary corners in the osg view dock widget (this supersedes the OS theme defaults)
    stackWidget->setLayout(stackLayout);


    kisScore = new QLCDNumber();
    QPalette* lcdpalette = new QPalette;
    kisScore->setAutoFillBackground(true);
    lcdpalette->setColor(QPalette::Background, QColor(0, 0, 0));
    lcdpalette->setColor(QPalette::Base, QColor(0, 0, 0));
    lcdpalette->setColor(QPalette::AlternateBase, QColor(0, 0, 0));
    kisScore->setPalette(*lcdpalette);


    stackLayout->addWidget(kisScore);
    kisTimer = new ACKnownItemSearchTimerQt();
    // CF for 11/04/2014 test
    //stackLayout->addWidget(kisTimer);

    osgViewDockLayout->addWidget(stackWidget);
    //osgViewDockLayout->addWidget(kisTimer);

    view = new ACOsgCompositeViewQt();
    view->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    osgViewDockLayout->addWidget(view);
    connect( view, SIGNAL( importDirectoriesThreaded(std::vector<std::string>,bool ) ),
             this, SLOT(importDirectoriesThreaded(std::vector<std::string>,bool) ) );

    //progressBar = new ACQProgressBar(); //QProgressBar();
    //osgViewDockLayout->addWidget(progressBar);
    //this->setCentralWidget(osgViewDockWidget);


    QWidget* container = new QWidget;
    container->setLayout(osgViewDockLayout);
    this->setCentralWidget(container);


    // to make window appear on top of others.
    this->activateWindow();
    this->showMaximized();//this->show();

    metadataWindow = new ACMediaLibraryMetadataQt(this);
    metadataWindow->hide();
    userProfileWindow = new ACUserProfileQt(this);
    userProfileWindow->hide();

    //progressBar->hide();
#ifdef __APPLE__ // due to Apple standards the progress bar won't display any overlaid message, while it is the case under Ubuntu
    connect( this, SIGNAL( mediacycle_message_changed(QString) ),
             this->statusBar(), SLOT(message(QString) ) );
#endif
    /*connect( this, SIGNAL( loading_started() ),
             this->progressBar, SLOT(loading_started() ) );
    connect( this, SIGNAL( loading_finished() ),
             this->progressBar, SLOT(loading_finished() ) );
    connect( this, SIGNAL( loading_file(int,int) ),
             this->progressBar, SLOT(loading_file(int,int) ) );
    */
    // Debugging accentuated media filenames
    /*#ifdef USE_DEBUG
 qDebug() << "System Locale name:"      << QLocale::system().name();
 qDebug() << "Qt codecForCStrings:" << QTextCodec::codecForCStrings();
 qDebug() << "Qt codecForLocale:"   << QTextCodec::codecForLocale()->name();
 #endif*/
    if (!QTextCodec::codecForCStrings())
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//CF hack for CF to load accents

    // XS reminder: need to call configureSettings from the application main.

    // Remove access to the Config Settings panel from the config menu for releases
    ui.menuConfig->removeAction( ui.actionEdit_Config_File );

    // Adapt the help panel action to support standard keys
    ui.actionHelpAbout->setShortcuts(QList<QKeySequence>(ui.actionHelpAbout->shortcuts()) << QKeySequence(QKeySequence::HelpContents));

    // Keeps actions (mouse/keyboard shortcuts) active in fullscreen mode when bars are hidden
    this->addActions(ui.toolbar->actions());
    this->addActions(ui.menubar->actions());
    this->addActions(ui.menuFile->actions());
    this->addActions(ui.menuConfig->actions());
    this->addActions(ui.menuDisplay->actions());
    this->addActions(ui.menuHelp->actions());
    //this->addActions(view->actions());

    this->setAcceptDrops(true); // for drag and drop

    // Must be the last lines of the constructor
    this->configureSettings();
    this->init();

}

ACMediaCycleOsgQt::~ACMediaCycleOsgQt(){
    this->clean();
    //delete settingsDialog;
    delete aboutDialogFactory;
    if(dockWidgetsManager) delete dockWidgetsManager;
    if (aboutDialog) delete aboutDialog;
    if (controlsDialog) delete controlsDialog;
    if (view) delete view;
    //if (progressBar) delete progressBar;
    if (osgViewDockLayout) delete osgViewDockLayout;
    if (osgViewDockWidget) delete osgViewDockWidget;
    if (metadataWindow) delete metadataWindow;
    if (userProfileWindow) delete userProfileWindow;
    if (segmentationDialog) delete segmentationDialog;
    if (librarySaveDialog) delete librarySaveDialog;
    this->destroyMediaCycle();
}

void ACMediaCycleOsgQt::changeMenuBarVisibility(bool visibility)
{
    if(visibility)
        ui.menubar->show();
    else
        ui.menubar->hide();
}

void ACMediaCycleOsgQt::changeStatusBarVisibility(bool visibility)
{
    if(visibility)
        ui.statusbar->show();
    else
        ui.statusbar->hide();
}

void ACMediaCycleOsgQt::changeToolBarVisibility(bool visibility)
{
    if(visibility)
        ui.toolbar->show();
    else
        ui.toolbar->hide();
}

void ACMediaCycleOsgQt::setMediaType(ACMediaType _mt)
{
    if(this->media_type != MEDIA_TYPE_NONE && this->media_type != _mt)
        this->clean(); //CF this ensures that the browser is cleaned at every media type change, and kept when adding new media files of the same type
    this->media_type = _mt;
    if(dockWidgetsManager)
        dockWidgetsManager->changeMediaType(_mt);
}

// tries to read settings from previous run
// if it does not find any, use default (centered) geometry
void ACMediaCycleOsgQt::configureSettings(){
    if (this->readQSettings()){
        /*QMessageBox msgBox;
  msgBox.setText("Launching MediaCycle.");
  msgBox.setInformativeText("Do you want to load your media settings from previous session ?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setDetailedText ("GUI settings (window size, ...) are loaded automatically. Here you can load MediaCycle settings (media type, browser mode, ...).");
                int ret = msgBox.exec();*/
        int ret = QMessageBox::No;
        switch (ret) {
        case QMessageBox::Yes:
            // config file normally in readSettings...
            if (this->readXMLConfig(this->config_file_xml)){
                if(dockWidgetsManager)
                    dockWidgetsManager->changeMediaType(this->media_type);
            }
            else
                this->showError("Error occured reading XML. Please Load your media and save a new XML file.");
            break;
        case QMessageBox::No:
            // then dock widgets will be configured afterwards (in updateLibrary, after loading media)
            break;
        default:
            //should never be reached
            //throw runtime_error("problem reading settings");
            break;
        }
    }
    else
        this->setDefaultQSettings();
}

// creates a MediaCycle object (containing the whole application)
// with the appropriate type (audio/image/video/text/mixed/composite/...)
void ACMediaCycleOsgQt::createMediaCycle(ACMediaType _media_type, ACBrowserMode _browser_mode){
    this->media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.xml");

    // keep track locally of the media and browser modes
    this->media_type = _media_type;
    this->browser_mode = _browser_mode;

    this->init();
}

void ACMediaCycleOsgQt::init(){
    //media_cycle->setCallback(mediacycle_callback, (void*)this);
    media_cycle->addListener(this);
    view->setMediaCycle(this->media_cycle);
    view->prepareFromBrowser();
    view->prepareFromTimeline();
    aboutDialogFactory->setMediaCycle(media_cycle);
    if(dockWidgetsManager){
        dockWidgetsManager->setMediaCycle(media_cycle);
        dockWidgetsManager->set View(view);
    }
    metadataWindow->setMediaCycle(media_cycle);
    userProfileWindow->setMediaCycle(media_cycle);
}

// destroys the MediaCycle object (containing the whole application)
// it should leave an empty blue frame, just as lauch time
// use with care; there should only be one instance of mediacycle per application
void ACMediaCycleOsgQt::destroyMediaCycle(){
    delete media_cycle;
}


// XS in theory one could select multiple XML files and concatenate them (not tested yet)
void ACMediaCycleOsgQt::on_actionLoad_Library_triggered(bool checked){
    this->readXMLConfig();
}

bool ACMediaCycleOsgQt::readXMLConfig(string _filename){
    if (_filename==""){
        QString fileName;
        QFileDialog dialog(this,"Open XML Library File(s)");
        dialog.setDefaultSuffix ("xml");
        dialog.setNameFilter("Library Files (*.xml)");
        dialog.setFileMode(QFileDialog::ExistingFile); 	// change to ExistingFiles for multiple file handling
        QStringList fileNames;
        if (dialog.exec())
            fileNames = dialog.selectedFiles();
        QStringList::Iterator file = fileNames.begin();
        // only one file -> if
        // many files -> while (not implemented yet)
        // while(file != fileNames.end()) {
        if(file != fileNames.end()) {
            fileName = *file;
            _filename=fileName.toStdString();
        }
        //++file;
    }

    std::cout << "Opening XML config file: '" << _filename << std::endl;

    //XS TODO: this is cut and paste from MediaCycle.
    // the problem is that we have no mediacycle instance
    // we need to read the XML file first.

    // 1) read header info
    //TiXmlHandle rootHandle = this->readXMLConfigHeader(_filename);
    try {
        if (_filename=="")
            return false;
        //throw runtime_error("bad XML file name");

        TiXmlDocument doc( _filename.c_str() );
        if (!doc.LoadFile( ))
            throw runtime_error("error reading XML file, malformed");
        TiXmlHandle docHandle(&doc);
        TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );
        // XS TODO make roothandle a pointer and check this
        //		if (!rootHandle)
        //			throw runtime_error("corrupted XML file, missing <MediaCycle>");

        // XS TODO browser mode and media type as string instead of enum
        TiXmlText* browserModeText=rootHandle.FirstChild( "BrowserMode" ).FirstChild().Text();
        std::stringstream tmp;
        if (browserModeText){
            tmp << browserModeText->ValueStr();
            int bm; // ACBrowserMode
            tmp >> bm;
            this->setBrowserMode(ACBrowserMode (bm));
            //CF todo propagate to docks
        }
        else{
            throw runtime_error("corrupted XML file, no browser mode");
        }
        TiXmlText* mediaTypeText=rootHandle.FirstChild( "MediaType" ).FirstChild().Text();
        if (mediaTypeText) {
            std::stringstream tmp2;
            tmp2 << mediaTypeText->ValueStr();
            int mt; //ACMediaType
            tmp2 >> mt;
            ACMediaType _mt(mt);
            if(this->media_cycle){
                if(this->media_cycle->getCurrentConfig() && this->media_cycle->getMediaType() != _mt) // means: if we're loading an XML file from MultimediaCycle
                    throw runtime_error("XML file of different media type");
            }
            this->setMediaType(_mt);
        }
        else{
            throw runtime_error("corrupted XML file, no media type");
        }

        // features vector weights
        // XS TODO check that it equals the number of media
        int n_feat=-1;
        vector<float> fw;

        TiXmlElement* FeaturesWeightsNode=rootHandle.FirstChild( "FeaturesWeights" ).Element();
        if (FeaturesWeightsNode) {
            TiXmlText* FeaturesWeightsText=rootHandle.FirstChild( "FeaturesWeights" ).FirstChild().Text();
            FeaturesWeightsNode->QueryIntAttribute("NumberOfFeatures", &n_feat);
            if (n_feat < 0)
                throw runtime_error("corrupted XML file, wrong number of features weights");
            try {
                if(FeaturesWeightsText){
                    std::stringstream tmp3;
                    tmp3 << FeaturesWeightsText->ValueStr();
                    for (int j=0; j<n_feat; j++) {
                        // XS TODO add tests !! on number of features
                        float w;
                        tmp3 >> w;
                        fw.push_back(w);
                    }
                }
            }
            catch (...) {
                // attempt to catch potential problems and group them
                throw runtime_error("corrupted XML file, error reading feature weight");
            }
        }

        // 2) change mediacycle settings accordingly
        if (this->media_cycle){
            this->changeMediaType(this->media_type);
            this->media_cycle->changeBrowserMode(this->browser_mode);
        }
        else
            createMediaCycle(this->media_type, this->browser_mode);

        // 3) load the plugin libraries
        media_cycle->readXMLConfigFilePlugins(rootHandle);
        ACPluginManager* acpl = media_cycle->getPluginManager();
        if (acpl->getAvailablePluginsSize(PLUGIN_TYPE_FEATURES,media_type) &&/*acpl->getFeaturesPlugins()->getSize(media_type)>0 &&*/ !use_feature_extraction){ // if no feature extraction plugin was loaded before opening the XML and if the XML loaded one
            this->showError("Feature extraction plugin(s) now loaded again. Importing media files now enabled.");
            this->switchFeatureExtraction(true);
        }

        //CF due to the newly-granularized plugins, we load a selected current generic config in case XML won't list enough plugins:
        bool needs_saving = false;
        if( ACMediaFactory::getInstance().getExtensionsFromMediaType(this->media_type).size() == 0){
            std::cerr << "ACMediaCycleOsgQt::readXMLConfig: old XML file, trying to match one of the available default configs" << std::endl;
            needs_saving=true;
        }
        if(!media_cycle->getCurrentConfig()){ // happens when loading an XML file from a newly-launched MultiMediaCycle
            std::vector<std::string> defaultConfigs = media_cycle->getDefaultConfigsNames();
            for(std::vector<std::string>::iterator defaultConfig = defaultConfigs.begin(); defaultConfig != defaultConfigs.end();++defaultConfig){
                QString config(defaultConfig->c_str());
                config = config.toLower();
                QString mediaType = ACMediaFactory::getInstance().getLowCaseStringFromMediaType(this->media_type).c_str();
                if(config.contains(mediaType) && config.contains("cycle")){
                    std::cout << "ACMediaCycleOsgQt::readXMLConfig: loading config " << *defaultConfig << std::endl;
                    this->loadDefaultConfig( media_cycle->getDefaultConfig(*defaultConfig));
                }
            }
            if(!media_cycle->getCurrentConfig())
                this->showError("Deprecated XML file can't be loaded. Please ask the MediaCycle team to repair it or import your media files again.");
        }

        if(dockWidgetsManager){
            dockWidgetsManager->changeMediaType(this->media_type);
            //dockWidgetsManager->updatePluginsSettings();
        }

        // 4) load the media elements (features and segments already pre-computed)
        media_cycle->readXMLConfigFileCore(rootHandle);


        // XS TODO check this.
        // should be overwritten if dimensions do not match
        media_cycle->setWeightVector(fw);

        // XML features are not normalized, so we force normalization here
        // media_cycle->normalizeFeatures(1);

        // only after loading all XML files:
        //this->updateLibrary();//TR update will be done in the mediaImported(int n,int nTot,int mId) with n==nTot and mId==-1
        media_cycle->storeNavigationState();

        if(dockWidgetsManager)
            dockWidgetsManager->updatePluginsSettings();

        if(needs_saving)
            this->showError("Please save again your library.");
    }
    catch (const exception& e) {
        this->showError(e);
        return false;
    }

    // no errors occured, no exception caught.

    this->postLoadXML();

    if(this->setting == AC_SETTING_INSTALLATION){
        if(dockWidgetsManager) dockWidgetsManager->updateDocksVisibility(false);
    }

    return true;
}

void ACMediaCycleOsgQt::on_actionSave_Library_triggered(bool checked){
    // XS TODO what is the use of "checked" (in argument above) ?
    this->writeXMLConfig();
}

// this saves the XML file "as is"
// ex: the user quits when all features have'nt been computed yet
void ACMediaCycleOsgQt::writeXMLConfig(string _filename){
    if (! hasMediaCycle()) return;

    if (_filename=="") {
        // no file name supplied, ask for one
        QString fileName("");
        QString filters=QString("MediaCycle XML Library (*.xml);;Library (*.xml)");
        QString filter=QString("MediaCycle XML Library (*.xml)");

        //CF condensed version that crashes on OSX 10.8+ when selecting the folder from the drop-down menu
        //fileName = QFileDialog::getSaveFileName(this, tr("Save Config as XML Library"),"",filter));

        //CF alternative with multiple file formats
        //fileName = QFileDialog::getSaveFileName(this, tr("Save Config as XML Library"),"",filters,&filter);

        //CF non-native alternative: ugly and won't provide the user-bookmarked locations
        //fileName = QFileDialog::getSaveFileName(this, tr("Save Config as XML Library"),"",filter,0,QFileDialog::DontUseNativeDialog);

        QFileDialog dialog(this,"Save Config as XML Library");
        //dialog.setDefaultSuffix("xml");
        QStringList filtersList;
        filtersList.append("MediaCycle XML Library (*.xml)");

        QString default_writer("");
        if(media_cycle->getCurrentConfig()){
            default_writer = QString(media_cycle->getCurrentConfig()->libraryWriterPlugin().c_str());
        }

        std::vector<std::string> writers = media_cycle->getAvailablePluginNames(PLUGIN_TYPE_LIBRARY_WRITER, this->getMediaType());
        for(std::vector<std::string>::iterator writer = writers.begin();writer != writers.end();writer++){
            ACPlugin* plugin = media_cycle->getPlugin(*writer);
            ACMediaLibraryWriterPlugin* writer_plugin = dynamic_cast<ACMediaLibraryWriterPlugin*>(plugin);
            if(writer_plugin){
                std::vector<std::string> formats = writer_plugin->fileFormats();
                for(std::vector<std::string>::iterator format = formats.begin();format != formats.end();format++){
                    QString current_writer( QString((*writer).c_str()) + " (*" + QString((*format).c_str()) +")" );
                    if( default_writer.compare( QString((*writer).c_str())) == 0 ){
                        filtersList.push_front( current_writer );
                    }
                    else{
                        filtersList.append( current_writer );
                    }
                }
            }
        }

        dialog.setNameFilters(filtersList);

        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        QStringList fileNames;
        if (!dialog.exec())
            return;

        fileNames = dialog.selectedFiles();
        QString nameFilter = dialog.selectedFilter();

        bool as_xml = (nameFilter.compare("MediaCycle XML Library (*.xml)") == 0);

        if(fileNames.size() != 1)
            return;
        fileName = fileNames.first();

        if (fileName.isEmpty()) return;
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this,
                                 tr("File error"),
                                 tr("Failed to open\n%1").arg(fileName));
        }
        else {
            _filename = fileName.toStdString();
        }

        if(as_xml){
            cout << "saving config in XML file: " << _filename << endl;
            media_cycle->saveXMLConfigFile(_filename);
        }
        else{
            int ext = nameFilter.lastIndexOf('*');
            int hyphen = nameFilter.lastIndexOf('(');
            if(ext == -1 || hyphen == -1){
                return;
            }
            QString extension(nameFilter);
            extension = extension.remove(0,ext+1);
            extension = extension.remove(")");
            QString type(nameFilter);
            type = nameFilter.remove(hyphen-1,nameFilter.size()-ext+2);

            ACPlugin* plugin = media_cycle->getPlugin(type.toStdString());
            ACMediaLibraryWriterPlugin* writer_plugin = dynamic_cast<ACMediaLibraryWriterPlugin*>(plugin);
            if(!writer_plugin)
                return;

            if(writer_plugin->getParametersCount() > 0){

                if(!librarySaveDialog)
                    librarySaveDialog = new ACPluginControlsDialogQt(PLUGIN_TYPE_LIBRARY_WRITER,"Library saving parameters","Do you want to save the library?");
                librarySaveDialog->setMediaCycle(media_cycle);
                librarySaveDialog->updatePluginsSettings();

                if(!librarySaveDialog->exec())
                    return;
                if(librarySaveDialog->result() != 1)
                    return;

                try{
                    media_cycle->saveLibrary(_filename,type.toStdString());
                }
                catch (const exception& e) {
                    this->showError(e);
                    return;
                }
            }
        }
    }else{
        // filename supplied (either as argument or from QMessageBox above)
        cout << "saving config in XML file: " << _filename << endl;
        media_cycle->saveXMLConfigFile(_filename);
    }
}

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACMediaCycleOsgQt::on_actionLoad_Media_Directory_triggered(bool checked){

    if(!use_feature_extraction){
        this->showError("No feature extraction plugin loaded. Can't import media files.");
        return;
    }

    std::vector<string> directories;

    if (! hasMediaCycle()) return;

    QString select_dir = QFileDialog::getExistingDirectory
            (
                this,
                tr("Open Directory"),
                "",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );

    if (select_dir=="")
        return; // e.g. the user pressed "cancel"
    else
        directories.push_back((string)select_dir.toStdString());

    if (!(directories.empty())){
        this->importDirectoriesThreaded(directories);
        directories.empty();
    }


    // XS TODO to use progress bar, we need to import files one by one...
    // so split the importdDirectory into scanDirectory + importFile
    //	pb->show();
    //	pb->setRange(0, 100);
    //	pb->setValue(2);

    // SD not working with threaded version
    /*


 int warn_button = 0;
 if (res > 0){
  statusBar()->showMessage(tr("Directory Imported."), 2000);
  media_cycle->normalizeFeatures();
  statusBar()->showMessage(tr("Features Normalized."), 2000);
  this->updateLibrary();
 }
 else if (res==0) {
  warn_button = QMessageBox::warning(this, "Warning",
              "Empty Directory"); //, <#const QString button0Text#>, <#const QString button1Text#>, <#const QString button2Text#>, <#int defaultButtonNumber#>, <#int escapeButtonNumber#>);

 }
 else  {
  warn_button = QMessageBox::warning(this, "Error",
              "Problem Importing Directory"); //, <#const QString button0Text#>, <#const QString button1Text#>, <#const QString button2Text#>, <#int defaultButtonNumber#>, <#int escapeButtonNumber#>);
 }
 statusBar()->clearMessage();
 */

}

void ACMediaCycleOsgQt::on_actionLoad_Media_Files_triggered(bool checked){

    if(!use_feature_extraction){
        this->showError("No feature extraction plugin loaded. Can't import media files.");
        return;
    }

    std::vector<string> directories;

    if (! hasMediaCycle()) return;

    QString fileName;
    QFileDialog dialog(this,"Open MediaCycle Media File(s)");

    //CF generating supported file extensions from used media I/O libraries and current media type:
    std::vector<std::string> mediaExt = media_cycle->getExtensionsFromMediaType( media_cycle->getLibrary()->getMediaType() );

    if(mediaExt.size()==0){
        this->showError("No file extensions supported for this media type. Please check the media factory. Can't import media files.");
        return;
    }

    QString mediaExts = "Supported Extensions (";
    std::vector<std::string>::iterator mediaIter = mediaExt.begin();
    for(;mediaIter!=mediaExt.end();++mediaIter){
        if (mediaIter != mediaExt.begin())
            mediaExts.append(" ");
        mediaExts.append("*");
        mediaExts.append(QString((*mediaIter).c_str()));
    }
    mediaExts.append(")");

    dialog.setNameFilterDetailsVisible(true);
    QStringList filtersList;
    filtersList.append(mediaExts);
    //filtersList.append("Dummy (*.dumb)");
    dialog.setNameFilters ( filtersList );

    //dialog.setNameFilter(mediaExts);



    dialog.setFileMode(QFileDialog::ExistingFiles); // ExistingFile(s); "s" is for multiple file handling

    QStringList fileNames;
    if (dialog.exec()){
        fileNames = dialog.selectedFiles();
        QString filter = dialog.selectedNameFilter();
    }

    QStringList::Iterator file = fileNames.begin();
    while(file != fileNames.end()) {
        fileName = *file;
        ++file;
        std::cout << "Opening: '" << fileName.toStdString() << "'" << std::endl;

        if (!(fileName.isEmpty())) {
            directories.push_back((string)fileName.toStdString());
        }
    }

    if (!(directories.empty())){
        this->importDirectoriesThreaded(directories);
        directories.empty();
    }
}


// import (threaded or not) moved here instead of in MediaCycle
// because of problems with Qt and threads
// e.g., QCoreApplication::sendPostedEvents: Cannot send posted events for objects in another thread
void ACMediaCycleOsgQt::importDirectoriesThreaded(vector<string> directories,bool askToSegment) {
    // check if the user wants segments
    bool do_segments=false;
    if (askToSegment)
        do_segments= this->doSegments();
    bool forward_order = true; // only make it false for AudioGarden where media have been presegmented and segments have special names
    int recursive = 1;

    // XS TODO : for progress bar and threading, see:
    // http://hopf.chem.brandeis.edu/yanglingfa/Qt/threading/index.html

    // not necessary to thread if only few files.
    int n_dir_for_threading = 0;
    if (directories.size() > n_dir_for_threading)
        media_cycle->importDirectoriesThreaded(directories, recursive, forward_order, do_segments);
    else{
        media_cycle->importDirectories(directories, recursive, forward_order, do_segments);
        // XS TODO FIXME !!
        // does not belong here for threaded version
        this->updateLibrary();
    }
}

bool ACMediaCycleOsgQt::doSegments(){
    bool do_segments = false;

    /*if(use_segmentation_current){
  int seg_button = QMessageBox::question(this,
   tr("Segmentation"),
   tr("Do you want to segment the media ?"),
   QMessageBox::Yes | QMessageBox::No);
  if (seg_button == QMessageBox::Yes) {
   do_segments = true;
  }
        }*/

    if(!use_segmentation_current)
        return do_segments;

    if(!segmentationDialog)
        segmentationDialog = new ACPluginControlsDialogQt(PLUGIN_TYPE_SEGMENTATION, "Segmentation?", "Do you want to segment the media files?", this);
    segmentationDialog->setMediaCycle(media_cycle);
    segmentationDialog->updatePluginsSettings();

    if (segmentationDialog->exec()){
        if(segmentationDialog->result() == 1)
            do_segments = true;
    }
    return do_segments;
}

void ACMediaCycleOsgQt::on_actionEdit_Config_File_triggered(bool checked){
    cout << "Editing config file with Setting Dialog GUI..." << endl;
    //settingsDialog->show();
    //settingsDialog->setMediaCycleMainWindow(this);
    //settingsDialog->setFocus();
}

void ACMediaCycleOsgQt::on_actionEdit_Input_Controls_triggered(bool checked){

    if (controlsDialog == 0){
        //this->addActions(view->actions());
        //std::cout << "this->actions().size() " << this->actions().size() << std::endl;
        ///QList<QAction *> _actions = QList<QAction *> (this->actions());
        //std::cout << "_actions.size() " << _actions.size() << std::endl;
        ///_actions.append(view->actions());
        //std::cout << "view->actions().size() " << view->actions().size() << std::endl;
        //std::cout << "_actions.size() " << _actions.size() << std::endl;
        controlsDialog = new ACInputControlsDialogQt(this);
        controlsDialog->addActions(this->actions());
        controlsDialog->addInputActions(view->getInputActions());
        //controlsDialog = new ACInputControlsDialogQt(this->actions(),this);
    }
    controlsDialog->setMediaCycle(this->media_cycle);
    controlsDialog->set View(this->view);

    if(controlsDialog->isVisible()){
        controlsDialog->hide();
    }
    else{
        controlsDialog->show();
    }
}

bool ACMediaCycleOsgQt::addControlDock(std::string dock_type){
    if(!dockWidgetsManager)
        return false;
    ACAbstractDockWidgetQt* dock = dockWidgetsManager->addControlDock(dock_type);
    return this->addControlDock(dock);
}

bool ACMediaCycleOsgQt::addControlDock(ACPluginType plugin_type){
    if(!dockWidgetsManager)
        return false;
    ACAbstractDockWidgetQt* dock = dockWidgetsManager->addControlDock(plugin_type);
    return this->addControlDock(dock);
}

bool ACMediaCycleOsgQt::addControlDock(ACAbstractDockWidgetQt* dock)
{
    if(dock){
        QAction* dock_action = new QAction(this);
        dock_action->setCheckable(true);
        dock_action->setChecked(true);
        dock_action->setText( dock->windowTitle() + QString(" Controls") );
        connect(dock_action,SIGNAL(toggled(bool)),dock,SLOT(setVisible(bool)));
        connect(dock,SIGNAL(visibilityChanged(bool)),dock_action,SLOT(setChecked(bool)));
        ui.menuDisplay->addAction(dock_action);
        this->addAction(dock_action);
        return true;
    }
    return false;
}

void ACMediaCycleOsgQt::on_actionToggle_Controls_triggered(bool controlsToEnable){
    if(dockWidgetsManager)
        dockWidgetsManager->updateDocksVisibility(controlsToEnable);
}

bool ACMediaCycleOsgQt::addAboutDialog(ACAbstractAboutDialogQt* dialog)
{
    if (dialog == 0){
        std::cerr << "Invalid about dialog" << std::endl;
        return false;
    }

    aboutDialog = dialog;
    return true;
}

bool ACMediaCycleOsgQt::addAboutDialog(std::string about_type)
{
    return this->addAboutDialog(aboutDialogFactory->createAboutDialog(this,about_type));
}

void ACMediaCycleOsgQt::on_actionHelpAbout_triggered(bool checked) {
    if (aboutDialog==0)
        aboutDialog = new ACAboutDialogQt(this);
    aboutDialog->setMediaCycle(this->media_cycle);//currently updates the licenses list
    aboutDialog->show();
}

void ACMediaCycleOsgQt::on_actionEdit_Library_Metadata_triggered(bool checked) {
    if (metadataWindow)
        metadataWindow->show();
}

void ACMediaCycleOsgQt::on_actionEdit_Profile_triggered(bool checked) {
    if (userProfileWindow)
        userProfileWindow->show();
}

void ACMediaCycleOsgQt::on_actionFullscreen_triggered(bool checked) {
    if (!checked || view->isFullScreen() || this->isFullScreen()){
        std::cout << "Not fullscreen" << std::endl;
        this->showNormal();
        view->showNormal();
        ui.actionToggle_Controls->setChecked(true);
        this->on_actionToggle_Controls_triggered(true);
        /*if(this->setting != AC_SETTING_INSTALLATION){
            ui.menubar->show();
            ui.statusbar->show();
            ui.toolbar->show();
        }*/
        this->readQSettings();
    }
    else{
        this->writeQSettings();
        ui.actionToggle_Controls->setChecked(false);
        this->on_actionToggle_Controls_triggered(false);
        this->on_actionToggle_Controls_triggered(false);
        ui.menubar->hide();
        ui.statusbar->hide();
        ui.toolbar->hide();
        view->showFullScreen();
        this->showFullScreen();

        std::cout << "Fullscreen" << std::endl;
    }
    view->setFocus();
}

void ACMediaCycleOsgQt::dragEnterEvent(QDragEnterEvent *event)
{
    std::cout <<"<drop content>" << std::endl;
    event->acceptProposedAction();
}

void ACMediaCycleOsgQt::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ACMediaCycleOsgQt::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    /*if (mimeData->hasImage()) {
              setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
          } else if (mimeData->hasHtml()) {
              setText(mimeData->html());
              setTextFormat(Qt::RichText);
          } else if (mimeData->hasText()) {
              setText(mimeData->text());
              setTextFormat(Qt::PlainText);
          } else*/ if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QString text;
        for (int i = 0; i < urlList.size() && i < 32; ++i) {
            QString url = urlList.at(i).path();
            text += url + QString("\n");
            std::cout << url.toStdString() << std::endl;
        }
        //setText(text);
    } /*else {
              setText(tr("Cannot display data"));
          }*/

    /*if (mimeData){
         foreach (QString format, mimeData->formats()) {
             std::cout << format.toStdString() << std::endl;
         }
    }*/
    event->acceptProposedAction();
}

void ACMediaCycleOsgQt::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}


void ACMediaCycleOsgQt::updateLibrary(){
    if (! hasMediaCycle()) return;
    // XS TODO updateLibrary()
    media_cycle->libraryContentChanged();

    int refNode=media_cycle->getReferenceNode();
    if (refNode<0||refNode>=media_cycle->getLibrarySize())
        media_cycle->setReferenceNode(0);

    // XS TODO this is sooo ugly:
    // XS TODO updateBrowser()
    media_cycle->getBrowser()->setState(AC_CHANGING);

    view->prepareFromBrowser();
    view->prepareFromTimeline();
    //browserOsgView->setPlaying(true);
    media_cycle->setNeedsDisplay(true);
    if(dockWidgetsManager)
        dockWidgetsManager->updatePluginsSettings();
    view->setFocus();
    metadataWindow->updateLibrary();
    //media_cycle->updateDisplay(true); //CF tried
}

// adds the plugins in _library pth via mediaCycle's pluginManager
// keeps track of the plugins added by the Settings Dialog
bool ACMediaCycleOsgQt::addPluginLibrary(string _library){
    bool ok = false;
    if (! hasMediaCycle()) return false;
    if (this->media_cycle->addPluginLibrary(_library)){
        this->plugins_libraries.push_back(_library);
        cout << "added plugin " << _library << endl;
        ok = true;
    }
    if (!ok) throw runtime_error("failed adding plugin library " + _library);
    return ok;
}

bool ACMediaCycleOsgQt::removePluginLibrary(string _library){
    bool ok = false;
    if (! hasMediaCycle()) return false;
    if (this->media_cycle->removePluginLibrary(_library)){
        vector<string>::iterator f = std::find(this->plugins_libraries.begin(), this->plugins_libraries.end(), _library);
        if( f != this->plugins_libraries.end() ) {
            this->plugins_libraries.erase(f);
            cout << "erased plugin " << _library << endl;
            ok = true;
        }
    }
    return ok;
}

ACPluginLibrary* ACMediaCycleOsgQt::getPluginLibrary(string _library) {
    if (! hasMediaCycle()) return 0;
    return this->getMediaCycle()->getPluginLibrary(_library);
}

bool ACMediaCycleOsgQt::removePluginFromLibrary(std::string _plugin_name, std::string _library_path){
    if (! hasMediaCycle()) return 0;
    return this->getMediaCycle()->removePluginFromLibrary(_plugin_name, _library_path);
}

//Return copies of a string with whitespace removed from the right
//http://www2.warwick.ac.uk/fac/sci/physics/research/epp/people/andrew_bennieston/computing/cpp/

std::string ACMediaCycleOsgQt::rstrip(const std::string& s){
    std::string::size_type p = s.find_last_not_of(" \n\r\t");
    return std::string(s, 0, p+1);
}

int ACMediaCycleOsgQt::tryLoadFeaturePluginFromBaseName(std::string basename)
{
    if (!media_cycle)
        return -1;

    //CF test if basename is a mediatype?

    std::string plugin = media_cycle->getPluginPathFromBaseName(basename);
    int n_elements = media_cycle->addPluginLibrary(plugin);
    if( n_elements == -1){
#ifdef USE_DEBUG
        this->showError("Couldn't load the " + basename + " feature extraction plugin from path '" + plugin + "'. Importing media files might work only by loading XML library files.");
#else
        this->showError("Couldn't load the " + basename + " feature extraction plugin. Importing media files might work only by loading XML library files.");
#endif
        this->switchFeatureExtraction(false);
        return n_elements;
    }
    else{
        if(!use_feature_extraction){ // if no feature extraction plugin could be loaded before this change of media type
            this->showError(basename + " feature extraction plugin now loaded again. Importing media files now enabled.");
        }
        this->switchFeatureExtraction(true);
        return n_elements;
    }
}

bool ACMediaCycleOsgQt::addDefaultConfig(ACAbstractDefaultConfig* _config)
{
    if(!media_cycle){
        std::cerr << "ACMediaCycleOsgQt::addDefaultConfig: requires a MediaCycle instance" << std::endl;
        return false;
    }
    if(!(media_cycle->addDefaultConfig(_config))){
        std::cerr << "ACMediaCycleOsgQt::addDefaultConfig: wrong config" << std::endl;
        return false;
    }
    // Allow to select configurations once more 2 are available
    if(media_cycle->getDefaultConfigsNumber()==2){
        this->addControlDock("MCMediaConfig"); // the dock widget manager checks for duplicates
    }
    if(dockWidgetsManager)
        dockWidgetsManager->updatePluginsSettings();
    return true;
}

bool ACMediaCycleOsgQt::loadDefaultConfig(ACAbstractDefaultConfig* _config)
{
    if(media_cycle->getCurrentConfig()){
        if(media_cycle->getCurrentConfig() != _config)
            this->clean();
    }

    // Check the config
    if(!(this->addDefaultConfig(_config)))
        return false;
    ACOsgAbstractDefaultConfig* _osg_config = dynamic_cast<ACOsgAbstractDefaultConfig*>(_config);
    ACOsgAbstractDefaultConfigQt* _osg_qt_config = dynamic_cast<ACOsgAbstractDefaultConfigQt*>(_config);

    // Check required OSG plugins:
    if(_osg_config){
        std::vector<std::string> osg_plugins = _osg_config->osgPlugins();
        for(std::vector<std::string>::iterator plugin = osg_plugins.begin();plugin != osg_plugins.end();++plugin){
            QString basename(plugin->c_str()),filename(plugin->c_str());
            basename = basename.remove("osgdb_");
#if defined (__MINGW32__) || defined (WIN32)
            filename.prepend("lib");
#endif
#ifdef OSG_LIBRARY_STATIC
            filename.append(".a");
#else
#if defined (__MINGW32__) || defined (WIN32)
            filename.append(".dll");
#else
            filename.append(".so");
#endif
#endif
            osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(basename.toStdString());
            if(!readerWriter){
                // Sometimes plugins are not named along file extensions, like for instance osgdb_imageio
                osgDB::Registry::LoadStatus status = osgDB::Registry::instance()->loadLibrary(filename.toStdString());
                if(status == osgDB::Registry::NOT_LOADED){
                    this->showError("OSG plugin '" + *plugin + "' is required but not accessible. Can't use config.");
                    return false;
                }
                else /*if(status == osgDB::Registry::LOADED)*/{ // last case could be osgDB::Registry::PREVIOUSLY_LOADED
                    osgDB::Registry::instance()->closeLibrary(filename.toStdString());
                }
            }
        }
    }

    // Try to load the default config with MediaCycle
    // Create or update MediaCycle instance:
    /*if (this->media_cycle){
        if(this->media_cycle->getMediaType() != _config->mediaType())
            this->changeMediaType(_config->mediaType());
        if(this->media_cycle->getBrowserMode() != _config->browserMode())
            this->media_cycle->changeBrowserMode(_config->browserMode());
    }
    else
        createMediaCycle(_config->mediaType(),_config->browserMode());

    // Load MediaCycle plugins
    std::vector<std::string> plugins = _config->pluginLibraries();
    for(std::vector<std::string>::iterator plugin = plugins.begin();plugin != plugins.end();++plugin){
        if(this->tryLoadFeaturePluginFromBaseName(*plugin) <= 0)
            this->showError("Couldn't load plugin library '" + *plugin + "'");
    }*/
    try{
        this->media_cycle->loadDefaultConfig(_config->name());
    }
    catch (const exception& e) {
        this->showError(e);
        return false;
    }
    this->setMediaType(media_cycle->getMediaType());
    this->changeSetting(_config->settingType());

    // Check plugins:
    /*if(media_cycle->getPluginManager()->getAvailableFeaturesPlugins()->getSize(_config->mediaType()) == 0){
        this->showError("No feature extraction plugin available. Can't load this config.");
        return false;
    }*/

    this->useSegmentationByDefault( _config->useSegmentation() );
    if(_config->useSegmentation()){
        bool segmentation_plugins_avail = (media_cycle->getPluginManager()->getAvailableSegmentPluginsSize(_config->mediaType()) > 0);
        if(!(this->switchSegmentation( segmentation_plugins_avail ) ) )
            this->showError("No segmentation plugin available. Segmentation disabled.");
    }

    //CF TODO test presence of all other plugin types: reader, thumbnailer, visualisation

    // Add docks:
    //if(_config->useOSC())
    //    this->addControlDock("MCOSC");
    if(media_cycle->getDefaultConfigsNumber()>1)
        this->addControlDock("MCMediaConfig");
#ifdef SUPPORT_MULTIMEDIA
    if(_config->mediaType() == MEDIA_TYPE_MIXED){
        if(_config->mediaReaderPlugin() != "")
            media_cycle->setMediaReaderPlugin(_config->mediaReaderPlugin());
        media_cycle->setActiveMediaType(_config->activeMediaType());
        this->addControlDock("MCMediaDocumentOption");
    }
#endif

    if(_config->useNeighbors()){
        //this->addControlDock("MCSimilaritySliderControls");
        this->addControlDock("MCSimilarityCheckboxControls");
        this->addControlDock("MCBrowserControlsComplete");
    }
    else{
        this->addControlDock("MCSimilarityCheckboxControls");
        this->addControlDock("MCBrowserControlsClusters");
    }

    if(media_cycle->getAvailablePluginNames(PLUGIN_TYPE_MEDIARENDERER,_config->mediaType()).size()>0)
        this->addControlDock("MCMediaControls");

    //if(_osg_qt_config->useSegmentation())
    //    this->addControlDock("MCSegmentationControls");

    if(media_cycle->getAvailablePluginNames(PLUGIN_TYPE_CLIENT,_config->mediaType()).size()>0)
        this->addControlDock(PLUGIN_TYPE_CLIENT);

    // Update the plugin lists of the browser control dock through DockWidget
    if(dockWidgetsManager){
        dockWidgetsManager->changeMediaType(_config->mediaType());
        dockWidgetsManager->updatePluginsSettings();
    }

    this->postLoadDefaultConfig();
    return true;
}

// media type of the library
void ACMediaCycleOsgQt::changeMediaConfig(QString media){
    string mt = media.toStdString();
    // custom settings = edit config file
    if (mt == "Custom"){
        cout << "editing configuration file" << endl;
        this->on_actionEdit_Config_File_triggered(true);
        return;
    }

    if (mt == "-- Config --"){
        return;
    }

    if(!media_cycle){
        std::cerr << "ACMediaCycleOsgQt::changeMediaConfig: requires a MediaCycle instance" << std::endl;
        return;
    }

    ACAbstractDefaultConfig* config = media_cycle->getDefaultConfig(mt);
    if(!config){
        std::cerr << "ACMediaCycleOsgQt::changeMediaConfig: no config named '" << mt << "'" << std::endl;
        return;
    }

    this->loadDefaultConfig(config);
    // default settings : find the right media
    /*ACMediaTypeNames::const_iterator iterm = media_type_names.find(mt);
    if( iterm == media_type_names.end() ) {
        cout << " media type not found : " << iterm->first << endl;
        return;
    }
    ACMediaType new_media_type = iterm->second;
    cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;

    if (this->media_type != new_media_type)
        this->loadDefaultConfig(new_media_type);//CF why not this->changeMediaType(new_media_type)?
    else
        return;*/
}

// active media type for documents
#ifdef SUPPORT_MULTIMEDIA
void ACMediaCycleOsgQt::changeActiveMediaType(QString name){
    string nameStr=string(name.toAscii());
    if (media_cycle->getActiveSubMediaKey()!=nameStr){
        media_cycle->setActiveMediaType(nameStr);
        media_cycle->initializeFeatureWeights();
        media_cycle->normalizeFeatures(1);
        media_cycle->libraryContentChanged(1);
        if(dockWidgetsManager){
            dockWidgetsManager->resetPluginsSettings();
            dockWidgetsManager->updatePluginsSettings();
        }
    }
}
#endif

void ACMediaCycleOsgQt::on_actionClean_triggered(bool checked) {
    this->clean();
}

void ACMediaCycleOsgQt::clean(bool _updategl){
    metadataWindow->clean();

    if(!media_cycle) return;
    // need to turn all sounds off before leaving
    // do this before cleaning library !!
    this->media_cycle->muteAllSources();

    this->media_cycle->cleanLibrary();
    this->media_cycle->cleanBrowser();
    if(dockWidgetsManager){
        dockWidgetsManager->resetPluginsSettings();
    }
    
    this->use_segmentation_default = true;
    this->use_segmentation_current = true;
    this->use_feature_extraction = true;
    this->use_visualization_plugins = true;

    statusBar()->clearMessage();
    //progressBar->reset();
    //progressBar->hide();

    view->clean(_updategl);
    view->setFocus();
}

void ACMediaCycleOsgQt::showError(std::string s){
    int warn_button;
    const QString qs = QString::fromStdString(s);
    if(this->setting != AC_SETTING_INSTALLATION)
        warn_button = QMessageBox::warning(this, "Error", qs);
    cerr << s << endl;
}

void ACMediaCycleOsgQt::showError(const exception& e) {
    this->showError(e.what());
}

bool ACMediaCycleOsgQt::hasMediaCycle(){
    if(media_cycle->getCurrentConfig() == 0) {
        //get a list of media config names:
        QStringList mediaConfigz;
        std::vector< std::string > mediaConfigs = media_cycle->getDefaultConfigsNames();
        for (std::vector< std::string >::iterator mediaConfig = mediaConfigs.begin(); mediaConfig!=mediaConfigs.end(); ++mediaConfig)
            mediaConfigz << QString((*mediaConfig).c_str());
        // popup a dialog asking to set the media config
        bool ok;
        QString mediaConfig =  QInputDialog::getItem(this,tr("Media config"),tr("Please to your media config:"), mediaConfigz, 0, false, &ok);
        if (ok && !mediaConfig.isEmpty()){
            this->changeMediaConfig(mediaConfig);
            return true;
        }
        return false;
    }
    return true;
}

void ACMediaCycleOsgQt::closeEvent(QCloseEvent *event) {
    // XS - SD TOOD : stop threads
    this->writeQSettings();
    /*QMessageBox msgBox;
 msgBox.setText("Quitting Application.");
 msgBox.setInformativeText("Do you want to save your media settings and features ?");
 msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
 msgBox.setDefaultButton(QMessageBox::Save);
 msgBox.setDetailedText ("GUI settings (window size, ...) are saved automatically. Here you can save MediaCycle settings (media type, browser mode, ...).");
        int ret = msgBox.exec();*/
    int ret = QMessageBox::Discard;
    bool really_quit = true;
    switch (ret) {
    case QMessageBox::Save:
        this->writeXMLConfig(this->config_file_xml);
        break;
    case QMessageBox::Discard:
        break;
    case QMessageBox::Cancel:
        really_quit = false;
        break;
    default:
        // should never be reached
        break;
    }
    if (really_quit) {
        QMainWindow::closeEvent(event);
        qDebug("closed application window properly");
    }
    else {
        event->ignore();
        qDebug("Continuing Application");
    }
}

// platform-independent way of reading settings
// ("numediart", "MediaCycle") must be the same as in writeSettings
// The second argument to QSettings::value() is optional and specifies a default value for the setting if there exists none.
bool ACMediaCycleOsgQt::readQSettings() {
    QSettings settings("numediart", "MediaCycle");
    QPoint pos = settings.value("pos").toPoint(); //, QPoint(200, 200)).toPoint();
    QSize size = settings.value("size").toSize(); //, QSize(400, 400)).toSize();
    QString scf = settings.value("configFile").toString();
    bool has_settings = true;
    if (size.isEmpty()) { // e.g. we did settings.clear() or run the app for the 1st time
        has_settings = false;
    }
    else{
        this->resize(size);
        this->move(pos);
        this->config_file_xml = scf.toStdString();
        //		this->restoreState(settings.value("windowState").toByteArray());
        has_settings = true;
    }
    return has_settings;
}

// platform-independent way of saving settings
// ("numediart", "MediaCycle") must be the same as in readSettings
bool ACMediaCycleOsgQt::writeQSettings() {
    //QSettings settings(QApplication::applicationDirPath().append(QDir::separator()).append("settings.ini"),
    //				   QSettings::NativeFormat);
    QSettings settings("numediart", "MediaCycle");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("configFile", QVariant(QString::fromStdString(this->config_file_xml)));
    //	settings.setValue("windowState", saveState());
    return true;
}

void ACMediaCycleOsgQt::clearQSettings() {
    QSettings settings("numediart", "MediaCycle");
    settings.clear();
}

void ACMediaCycleOsgQt::setDefaultQSettings() {
    this->setGeometry( QStyle::alignedRect(
                           Qt::LeftToRight,
                           Qt::AlignCenter,
                           this->size(),
                           qApp->desktop()->availableGeometry()
                           ));
}

void ACMediaCycleOsgQt::changeMediaType(ACMediaType _media_type){
    // XS TODO turn off current audio engine if switch away from audio
    this->media_cycle->changeMediaType(_media_type);
    if(dockWidgetsManager){
        dockWidgetsManager->changeMediaType(_media_type);
    }
    metadataWindow->setMediaCycle(media_cycle);
    metadataWindow->clean();
    userProfileWindow->setMediaCycle(media_cycle);
}

void ACMediaCycleOsgQt::useSegmentationByDefault(bool _status)
{
    use_segmentation_default = _status;
}

bool ACMediaCycleOsgQt::switchSegmentation(bool _status)
{
    if (use_segmentation_default && ACMediaFactory::getInstance().isMediaTypeSegmentable(media_type) )
        use_segmentation_current = _status;
    else
        use_segmentation_current = false;
    return use_segmentation_current;
}

void ACMediaCycleOsgQt::switchFeatureExtraction(bool _status)
{
    use_feature_extraction = _status;
}

void ACMediaCycleOsgQt::switchPluginVisualizations(bool _status)
{
    use_visualization_plugins = _status;
}

void ACMediaCycleOsgQt::changeSetting(ACSettingType _setting)
{
    if(this->view)
        this->view->changeSetting(_setting);

    if(this->setting != _setting)
        this->setting = _setting;

    if(this->setting == AC_SETTING_INSTALLATION){
        ui.menubar->hide();
        ui.statusbar->hide();
        ui.toolbar->hide();
        if(dockWidgetsManager) dockWidgetsManager->updateDocksVisibility(false);
    }
}
