/*
 *  ACSegmentationControlsWidgetQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 12/06/2012
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ACSegmentationControlsWidgetQt.h"
#include "ACPluginParameterQt.h"
#include <qwt/qwt.h>
#include <qwt/qwt_slider.h>

ACSegmentationControlsWidgetQt::ACSegmentationControlsWidgetQt(QWidget *parent)
    : QWidget(parent),ACAbstractWidgetQt()
{
    ui.setupUi(this); // first thing to do
    this->show();
    connect(this,SIGNAL(rebuildPluginList()),this,SLOT(buildPluginList()));
    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
}

ACSegmentationControlsWidgetQt::~ACSegmentationControlsWidgetQt(){
}

void ACSegmentationControlsWidgetQt::on_comboBoxPlugins_activated(const QString & text)
{
    std::cout << "Chosen segmentation plugin: " << text.toStdString() << std::endl;
    //std::cout << "Media type " << ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_cycle->getMediaType()) << std::endl;
    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;
    //std::cout << "Active segments plugins size " << media_cycle->getPluginManager()->getActiveSegmentPluginsSize(media_cycle->getMediaType()) << std::endl;
    //std::cout << "Avail segments plugins size " << media_cycle->getPluginManager()->getAvailableSegmentPluginsSize(media_cycle->getMediaType()) << std::endl;
    media_cycle->getPluginManager()->setActiveSegmentPlugin(text.toStdString());
    //std::cout << "Active segments plugins size " << media_cycle->getPluginManager()->getActiveSegmentPluginsSize(media_cycle->getMediaType()) << std::endl;
    std::vector<std::string> names = media_cycle->getPluginManager()->getActiveSegmentPlugins()->getName();
    //std::cout << "Active segments plugin names size " << names.size() << std::endl;
    std::cout << "Active segments plugin " << names.front() << std::endl;

    //ui.tabWidgetParameters->setCurrentIndex( ui.comboBoxPlugins->currentIndex() ); // might be wrong if some plugins have no parameters!
    for(int t=0;t<ui.tabWidgetParameters->count();t++){
        if ( ui.tabWidgetParameters->tabText(t) == QString( media_cycle->getPluginManager()->getActiveSegmentPlugins()->getName().front().c_str()).remove("Segmentation"))
            ui.tabWidgetParameters->setCurrentIndex(t);
    }

    emit this->readjustHeight();
    //std::cout<< "ACSegmentationControlsWidgetQt::on_comboBoxPlugins_activated: current container min height " << ui.tabWidgetParameters->currentWidget()->minimumHeight() << std::endl;
}

void ACSegmentationControlsWidgetQt::on_tabWidgetParameters_currentChanged(int index){
    emit this->readjustHeight();
}

void ACSegmentationControlsWidgetQt::changeMediaType(ACMediaType _media_type)
{
    if (media_cycle == 0) return;
    if(media_cycle->getMediaType() != _media_type)
    {
        this->cleanPluginList();
        if (media_cycle->getPluginManager() == 0) return;
        emit this->rebuildPluginList();
    }
}

void ACSegmentationControlsWidgetQt::updatePluginsSettings()
{
    this->cleanPluginList();
    emit this->rebuildPluginList();
}

void ACSegmentationControlsWidgetQt::resetPluginsSettings()
{
    //this->cleanPluginList();
}

void ACSegmentationControlsWidgetQt::resetMediaType(ACMediaType _media_type)
{
    this->cleanPluginList();
    emit this->rebuildPluginList();
}

void ACSegmentationControlsWidgetQt::cleanPluginList()
{
    ui.comboBoxPlugins->clear();
    ui.comboBoxPlugins->setEnabled(false);
    ui.tabWidgetParameters->clear();
}

void ACSegmentationControlsWidgetQt::buildPluginList()
{
    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;

    // Build plugin list
    std::vector<std::string> availableSegmentPlugins = media_cycle->getPluginManager()->getAvailableSegmentPluginsNames( media_cycle->getMediaType() );
    for (std::vector<std::string>::iterator availableSegmentPlugin = availableSegmentPlugins.begin(); availableSegmentPlugin != availableSegmentPlugins.end(); availableSegmentPlugin++){
        ui.comboBoxPlugins->addItem(QString((*availableSegmentPlugin).c_str()));
        //std::cout << "Available segmentation plugin: " << *availableSegmentPlugin << std::endl;
        ui.comboBoxPlugins->setEnabled(true);
        ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(*availableSegmentPlugin);

        //std::cout << "Number of parameters: " << plugin->getParametersCount() << std::endl;
        if(plugin->getParametersCount()>0){
            QWidget* container = new QWidget();
            QGridLayout* layout = new QGridLayout();
            layout->setMargin(0);
            layout->setHorizontalSpacing(1);
            layout->setVerticalSpacing(5);
            container->setLayout(layout);
            container->setFixedWidth(230);

            QPushButton* pushbutton = new QPushButton("Reset");

            int paramIdx = 0;
            std::vector<std::string> strParamNames = plugin->getStringParametersNames();
            //std::cout << "Number of string parameters " << strParamNames.size() << std::endl;
            for(std::vector<std::string>::iterator strParamName=strParamNames.begin();strParamName!=strParamNames.end();strParamName++){
                //std::cout << "String parameter: " << (*strParamName) << std::endl;//" = " << (*strParamName).name << std::endl;

                QString name = QString((*strParamName).c_str());
                std::vector<std::string> names = plugin->getStringParameterValues(*strParamName);
                QString desc = QString( plugin->getStringParameterDesc(*strParamName).c_str());

                QLabel* label = new QLabel(name);
                label->setFont(QFont(QApplication::font().defaultFamily(),10));
                layout->addWidget(label,paramIdx,0);

                QComboBox* combobox = new QComboBox();
                for (std::vector<std::string>::iterator name = names.begin();name!=names.end();name++){
                    combobox->addItem(QString((*name).c_str()));
                }
                layout->addWidget(combobox,paramIdx,1);

                ACPluginParameterQt* stringParameter = new ACPluginParameterQt(plugin,name);

                connect(combobox,SIGNAL(currentIndexChanged(QString)),stringParameter,SLOT(updateStringParameter(QString)) );
                connect(pushbutton,SIGNAL(clicked()),stringParameter,SLOT(resetStringParameter()));
                connect(stringParameter,SIGNAL(stringParameterIndexChanged(int)),combobox,SLOT(setCurrentIndex(int)));

                paramIdx++;
            }

            std::vector<std::string> numParamNames = plugin->getNumberParametersNames();
            //std::cout << "Number of number parameters " << numParamNames.size() << std::endl;
            for(std::vector<std::string>::iterator numParamName=numParamNames.begin();numParamName!=numParamNames.end();numParamName++){
                //std::cout << "Number parameter: " << (*numParamName) << std::endl;//" = " << (*strParamName).name << std::endl;

                QString name = QString((*numParamName).c_str());
                ACPluginParameterQt* numberParameter = new ACPluginParameterQt(plugin,name);
                connect(pushbutton,SIGNAL(clicked()),numberParameter,SLOT(resetNumberParameter()));

                float step = plugin->getNumberParameterStep(*numParamName);
                float min = plugin->getNumberParameterMin(*numParamName);
                float max = plugin->getNumberParameterMax(*numParamName);
                float init = plugin->getNumberParameterInit(*numParamName);
                QString desc = QString( plugin->getNumberParameterDesc(*numParamName).c_str());

                QLabel* label = new QLabel(name);
                label->setFont(QFont(QApplication::font().defaultFamily(),10));
                layout->addWidget( label,paramIdx,0);
                if(min==0 && max==1 && step==1){
                    QCheckBox* checkbox = new QCheckBox("");
                    checkbox->setChecked((bool)init);
                    layout->addWidget( checkbox,paramIdx,1);

                    connect(checkbox,SIGNAL(stateChanged(int)),numberParameter,SLOT(updateNumberParameter(int)));
                    connect(numberParameter,SIGNAL(numberParameterChanged(double)),checkbox,SLOT(setChecked(bool)));
                    //connect(checkbox,SIGNAL(stateChanged(int)), new ACPluginParameterQt(plugin,name), SLOT(updateNumberParameter(int)));
                }
                else{
                    QwtSlider* slider = new QwtSlider(container);
                    slider->setRange(min,max,step);
                    slider->setValue(init);
                    slider->setBackgroundStyle(QwtSlider::Groove);
                    //slider->setScalePosition(QwtSlider::TopScale);
                    //QSlider* slider = new QSlider(Qt::Horizontal);
                    /*ACSliderQt* slider = new ACSliderQt(Qt::Horizontal);
                    slider->setSingleStep(step );
                    slider->setMinimum(min);
                    slider->setMaximum(max);
                    slider->setSliderPosition(init);*/

                    slider->setToolTip(desc);
                    slider->setAccessibleDescription(desc);
                    layout->addWidget( slider,paramIdx,1);

                    QDoubleSpinBox* spinbox = new QDoubleSpinBox();
                    spinbox->setSingleStep(step);
                    if(step>=1.0f)
                        spinbox->setDecimals(0);
                    spinbox->setMinimum(min);
                    spinbox->setMaximum(max);
                    spinbox->setValue(init);
                    spinbox->setToolTip(desc);
                    spinbox->setAccessibleDescription(desc);
                    layout->addWidget( spinbox,paramIdx,2);

                    connect(slider,SIGNAL(valueChanged(double)),spinbox,SLOT(setValue(double)));
                    connect(spinbox,SIGNAL(valueChanged(double)),slider,SLOT(setValue(double)));

                    connect(slider,SIGNAL(valueChanged(double)),numberParameter,SLOT(updateNumberParameter(double)));
                    connect(numberParameter,SIGNAL(numberParameterChanged(double)),slider,SLOT(setValue(double)));
                }
                paramIdx++;
            }

            layout->addWidget(pushbutton, paramIdx,0);

            QString tabName = QString((*availableSegmentPlugin).c_str()).remove("Segmentation");
            ui.tabWidgetParameters->addTab(container,tabName);

            emit this->readjustHeight();
        }

    }

    // If no segmentation plugin exist
    if(ui.comboBoxPlugins->count()==0){
        ui.comboBoxPlugins->addItem("None");
        ui.comboBoxPlugins->setEnabled(false);
    }
    else{
        int activePlugins = media_cycle->getPluginManager()->getActiveSegmentPlugins()->getName().size();
        std::string name("");
        // If a segmentation plugin is already active, set it on the combo
        if(activePlugins == 1){
            name = media_cycle->getPluginManager()->getActiveSegmentPlugins()->getName().front();
            std::cout << "Already active plugin: " << name << std::endl;
            int index = -1;
            index = ui.comboBoxPlugins->findText(QString(name.c_str()));
            if (index != -1){
                ui.comboBoxPlugins->setCurrentIndex(index);
            }
        } // If no segmentation plugin is active, get the current from the combo
        else if(activePlugins == 0){
            name = ui.comboBoxPlugins->currentText().toStdString();
            media_cycle->getPluginManager()->setActiveSegmentPlugin( name );
            std::cout << "Default active plugin: " << name << std::endl;
        }
        if(name!=""){ // Sync tabs with the combo (if parameters exist
            for(int t=0;t<ui.tabWidgetParameters->count();t++){
                if ( ui.tabWidgetParameters->tabText(t) == QString(name.c_str()).remove("Segmentation"))
                    ui.tabWidgetParameters->setCurrentIndex(t);
            }
        }
    }
}

void ACSegmentationControlsWidgetQt::adjustHeight(){
    if( ui.tabWidgetParameters->currentWidget() ){
        ui.tabWidgetParameters->currentWidget()->adjustSize();
        ui.tabWidgetParameters->currentWidget()->setMinimumHeight( ui.tabWidgetParameters->currentWidget()->height() );
        ui.tabWidgetParameters->currentWidget()->setMaximumHeight( ui.tabWidgetParameters->currentWidget()->height() );
        //std::cout<< "ACSegmentationControlsWidgetQt::buildPluginList: container of height " << container->height() << std::endl;
        ui.tabWidgetParameters->setMinimumHeight( ui.tabWidgetParameters->currentWidget()->height() + 32);
        ui.tabWidgetParameters->setMaximumHeight( ui.tabWidgetParameters->currentWidget()->height() + 32);
        ui.groupBoxParameters->setMinimumHeight( ui.tabWidgetParameters->minimumHeight() + 32);
        ui.tabWidgetParameters->adjustSize();
        ui.groupBoxParameters->adjustSize();
        this->setMinimumHeight( ui.groupBoxPlugins->minimumHeight() + ui.groupBoxParameters->minimumHeight() );
        this->adjustSize();
    }
}
