/*
 *  ACPluginControlsWidgetQt.cpp
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

#include "ACPluginControlsWidgetQt.h"
#include "ACPluginParameterQt.h"
#include <qwt.h>
#include <qwt_slider.h>

ACPluginControlsWidgetQt::ACPluginControlsWidgetQt(ACPluginType type, QWidget *parent)
    : pluginType(type),QWidget(parent),ACAbstractWidgetQt(),comboBoxPlugins(0),vboxLayout(0),parametersContainer(0)
{
    //setupUi(this); // first thing to do

#if !(pluginType == PLUGIN_TYPE_SEGMENTATION)
#error Currently on segmentation plugins are supported
#endif

    vboxLayout = new QVBoxLayout();
    vboxLayout->setMargin(0);
    comboBoxPlugins = new QComboBox();
    comboBoxPlugins->addItem("None");
    comboBoxPlugins->setEnabled(false);
    vboxLayout->addWidget(comboBoxPlugins);
    this->setLayout(vboxLayout);
    this->show();
    connect(comboBoxPlugins,SIGNAL(activated(QString)),this,SLOT(on_comboBoxPlugins_activated(QString)));
    connect(this,SIGNAL(rebuildPluginList()),this,SLOT(buildPluginList()));
    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
}

ACPluginControlsWidgetQt::~ACPluginControlsWidgetQt(){
    this->cleanPluginList();
}

void ACPluginControlsWidgetQt::on_comboBoxPlugins_activated(const QString & text)
{
    std::cout << "Chosen plugin: " << text.toStdString() << std::endl;
    //std::cout << "Media type " << ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_cycle->getMediaType()) << std::endl;
    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;
    if(this->pluginType == PLUGIN_TYPE_SEGMENTATION)
        media_cycle->getPluginManager()->setActiveSegmentPlugin(text.toStdString());
    else if(this->pluginType == PLUGIN_TYPE_CLUSTERS_METHOD)
        media_cycle->changeClustersMethodPlugin(text.toStdString());
    else if(this->pluginType == PLUGIN_TYPE_CLUSTERS_POSITIONS)
        media_cycle->changeClustersPositionsPlugin(text.toStdString());
    else if(this->pluginType == PLUGIN_TYPE_NEIGHBORS_METHOD)
        media_cycle->changeNeighborsMethodPlugin(text.toStdString());
    else if(this->pluginType == PLUGIN_TYPE_NEIGHBORS_POSITIONS)
        media_cycle->changeNeighborsPositionsPlugin(text.toStdString());
    emit this->rebuildPluginList();
}

void ACPluginControlsWidgetQt::changeMediaType(ACMediaType _media_type)
{
    this->cleanPluginList();
    if (media_cycle == 0) return;
    emit this->rebuildPluginList();
}

void ACPluginControlsWidgetQt::updatePluginsSettings()
{
    this->cleanPluginList();
    if (media_cycle == 0) return;
    emit this->rebuildPluginList();
}

void ACPluginControlsWidgetQt::resetPluginsSettings()
{
    this->cleanPluginList();
}

void ACPluginControlsWidgetQt::resetMediaType(ACMediaType _media_type)
{
    this->cleanPluginList();
    if (media_cycle == 0) return;
    emit this->rebuildPluginList();
}

void ACPluginControlsWidgetQt::cleanPluginList()
{
    comboBoxPlugins->clear();
    if(parametersContainer){
        vboxLayout->removeWidget(parametersContainer);
        delete parametersContainer;
    }
    parametersContainer = 0;

    comboBoxPlugins->addItem("None");
    comboBoxPlugins->setEnabled(false);
}

void ACPluginControlsWidgetQt::buildPluginList()
{
    this->cleanPluginList();

    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;

    std::vector<std::string> availablePlugins = media_cycle->getPluginManager()->getAvailablePluginsNames( this->pluginType , media_cycle->getMediaType() );

    std::string current_plugin_name("");
    if(this->pluginType == PLUGIN_TYPE_SEGMENTATION){
        int activePlugins = media_cycle->getPluginManager()->getActiveSegmentPluginsSize(media_cycle->getMediaType());
        if(activePlugins == 1){
            current_plugin_name = media_cycle->getPluginManager()->getActiveSegmentPluginsNames(media_cycle->getMediaType()).front();
            std::cout << "Already active plugin: " << current_plugin_name << std::endl;
        }
        else if(activePlugins == 0){
            if (media_cycle->getPluginManager()->getAvailablePluginsNames(this->pluginType,media_cycle->getMediaType()).size()>0){
                current_plugin_name = media_cycle->getPluginManager()->getAvailablePluginsNames(this->pluginType,media_cycle->getMediaType()).front();
            media_cycle->getPluginManager()->setActiveSegmentPlugin( current_plugin_name );
                std::cout << "Default active plugin: " << current_plugin_name << std::endl;
            }
        }
    }
    else{
        current_plugin_name = media_cycle->getBrowser()->getActivePluginName(this->pluginType);
        if(current_plugin_name == "" && availablePlugins.size()>0){
            current_plugin_name = availablePlugins.front();
            if(this->pluginType == PLUGIN_TYPE_CLUSTERS_METHOD)
                media_cycle->changeClustersMethodPlugin(current_plugin_name);
            else if(this->pluginType == PLUGIN_TYPE_CLUSTERS_POSITIONS)
                media_cycle->changeClustersPositionsPlugin(current_plugin_name);
            else if(this->pluginType == PLUGIN_TYPE_NEIGHBORS_METHOD)
                media_cycle->changeNeighborsMethodPlugin(current_plugin_name);
            else if(this->pluginType == PLUGIN_TYPE_NEIGHBORS_POSITIONS)
                media_cycle->changeNeighborsPositionsPlugin(current_plugin_name);
        }
    }

    // Build plugin list
    comboBoxPlugins->clear();
    for (std::vector<std::string>::iterator availablePlugin = availablePlugins.begin(); availablePlugin != availablePlugins.end(); availablePlugin++){
        QString plugin_name = QString((*availablePlugin).c_str());
        comboBoxPlugins->addItem(plugin_name);
        ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(*availablePlugin);

        //QFont font = QFont(QApplication::font().defaultFamily(),10);

        QFont font = comboBoxPlugins->font();
        font.setPointSize(10);

        comboBoxPlugins->setFont(font);

        if(plugin_name.toStdString() == current_plugin_name && plugin->getParametersCount()>0){
            parametersContainer = new QWidget(this);
            QGridLayout* layout = new QGridLayout();
            layout->setMargin(0);
            layout->setHorizontalSpacing(1);
            layout->setVerticalSpacing(5);
            int minHeight = 0;
            QPushButton* pushbutton = new QPushButton("Reset");
            pushbutton->setFont(font);

            int paramIdx = 0;
            std::vector<std::string> strParamNames = plugin->getStringParametersNames();
            //std::cout << "Number of string parameters " << strParamNames.size() << std::endl;
            for(std::vector<std::string>::iterator strParamName=strParamNames.begin();strParamName!=strParamNames.end();strParamName++){
                //std::cout << "String parameter: " << (*strParamName) << std::endl;//" = " << (*strParamName).name << std::endl;

                QString name = QString((*strParamName).c_str());
                std::vector<std::string> names = plugin->getStringParameterValues(*strParamName);
                QString desc = QString( plugin->getStringParameterDesc(*strParamName).c_str());

                QLabel* label = new QLabel(name);
                label->setFont(font);
                layout->addWidget(label,paramIdx,0);

                QComboBox* combobox = new QComboBox();
                for (std::vector<std::string>::iterator name = names.begin();name!=names.end();name++){
                    combobox->addItem(QString((*name).c_str()));
                }

                combobox->setCurrentIndex( plugin->getStringParameterValueIndex(*strParamName) );

                combobox->setFont(font);
                layout->addWidget(combobox,paramIdx,1);

                ACPluginParameterQt* stringParameter = new ACPluginParameterQt(plugin,name);

                connect(combobox,SIGNAL(currentIndexChanged(QString)),stringParameter,SLOT(updateStringParameter(QString)) );
                connect(pushbutton,SIGNAL(clicked()),stringParameter,SLOT(resetStringParameter()));
                connect(stringParameter,SIGNAL(stringParameterIndexChanged(int)),combobox,SLOT(setCurrentIndex(int)));
                combobox->adjustSize();
                minHeight += combobox->height();
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
                float init = plugin->getNumberParameterValue(*numParamName);
                QString desc = QString( plugin->getNumberParameterDesc(*numParamName).c_str());

                QLabel* label = new QLabel(name);
                label->setFont(font);
                layout->addWidget( label,paramIdx,0);
                if(min==0 && max==1 && step==1){
                    QCheckBox* checkbox = new QCheckBox("");
                    checkbox->setChecked((bool)init);
                    layout->addWidget( checkbox,paramIdx,1);

                    connect(checkbox,SIGNAL(stateChanged(int)),numberParameter,SLOT(updateNumberParameter(int)));
                    connect(numberParameter,SIGNAL(numberParameterChanged(double)),checkbox,SLOT(setChecked(bool)));
                    //connect(checkbox,SIGNAL(stateChanged(int)), new ACPluginParameterQt(plugin,name), SLOT(updateNumberParameter(int)));
                    checkbox->adjustSize();
                    minHeight += checkbox->height();
                }
                else{
                    QwtSlider* slider = new QwtSlider(0);
                    slider->setRange(min,max,step);
                    slider->setValue(init);
#if QWT_VERSION < 0x060000
                    slider->setBgStyle(QwtSlider::BgSlot);
                    slider->setMargins(12,12);
#else
                    slider->setBackgroundStyle(QwtSlider::Groove);
                    slider->setHandleSize(12,12);
#endif
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
                    spinbox->setFont(font);
                    spinbox->setAccessibleDescription(desc);
                    layout->addWidget( spinbox,paramIdx,2);

                    connect(slider,SIGNAL(valueChanged(double)),spinbox,SLOT(setValue(double)));
                    connect(spinbox,SIGNAL(valueChanged(double)),slider,SLOT(setValue(double)));

                    connect(slider,SIGNAL(valueChanged(double)),numberParameter,SLOT(updateNumberParameter(double)));
                    connect(numberParameter,SIGNAL(numberParameterChanged(double)),slider,SLOT(setValue(double)));
                    spinbox->adjustSize();
                    minHeight += spinbox->height();
                }
                paramIdx++;
            }
            layout->addWidget(pushbutton, paramIdx,0);
            pushbutton->adjustSize();
            minHeight += pushbutton->height();
            parametersContainer->setLayout(layout);
            vboxLayout->addWidget(parametersContainer);
            parametersContainer->setMinimumHeight(minHeight);
        }
    }

    int index = -1;
    index = comboBoxPlugins->findText(QString(current_plugin_name.c_str()));
    if (index != -1){
        comboBoxPlugins->setCurrentIndex(index);
    }

    comboBoxPlugins->setEnabled(false);
	// If no plugin exists
    if(comboBoxPlugins->count()==0)
        comboBoxPlugins->addItem("None");
	else if(comboBoxPlugins->count()>1)
        comboBoxPlugins->setEnabled(true);
    emit this->readjustHeight();
}

void ACPluginControlsWidgetQt::adjustHeight(){
    if(comboBoxPlugins){
        if(parametersContainer){
            parametersContainer->adjustSize();
            //comboBoxPlugins->setMinimumHeight( comboBoxPlugins->height() );
            //parametersContainer->setMinimumHeight( parametersContainer->height() );
            this->setMinimumHeight( comboBoxPlugins->minimumHeight() + parametersContainer->minimumHeight() );
            this->setFixedHeight( comboBoxPlugins->height() + parametersContainer->height() );
        }
        else{
            this->setMinimumHeight( comboBoxPlugins->height() );
            this->setFixedHeight( comboBoxPlugins->height() );
        }
    }
    else{
        this->setMinimumHeight( 0 );
    }
    this->adjustSize();
}
