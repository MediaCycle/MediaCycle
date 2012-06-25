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
#include <qwt.h>
#include <qwt_slider.h>

ACSegmentationControlsWidgetQt::ACSegmentationControlsWidgetQt(QWidget *parent)
    : QWidget(parent),ACAbstractWidgetQt(),comboBoxPlugins(0),vboxLayout(0),parametersLayout(0),parametersContainer(0)
{
    //setupUi(this); // first thing to do
    vboxLayout = new QVBoxLayout();
    vboxLayout->setMargin(0);
    comboBoxPlugins = new QComboBox();
    vboxLayout->addWidget(comboBoxPlugins);
    this->setLayout(vboxLayout);
    this->show();
    connect(comboBoxPlugins,SIGNAL(activated(QString)),this,SLOT(on_comboBoxPlugins_activated(QString)));
    connect(this,SIGNAL(rebuildPluginList()),this,SLOT(buildPluginList()));
    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
}

ACSegmentationControlsWidgetQt::~ACSegmentationControlsWidgetQt(){
    this->cleanPluginList();
}

void ACSegmentationControlsWidgetQt::on_comboBoxPlugins_activated(const QString & text)
{
    std::cout << "Chosen segmentation plugin: " << text.toStdString() << std::endl;
    //std::cout << "Media type " << ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_cycle->getMediaType()) << std::endl;
    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;
    media_cycle->getPluginManager()->setActiveSegmentPlugin(text.toStdString());
    emit this->rebuildPluginList();
}

void ACSegmentationControlsWidgetQt::changeMediaType(ACMediaType _media_type)
{
    if (media_cycle == 0) return;
    this->cleanPluginList();
    emit this->rebuildPluginList();
}

void ACSegmentationControlsWidgetQt::updatePluginsSettings()
{
    this->cleanPluginList();
    emit this->rebuildPluginList();
}

void ACSegmentationControlsWidgetQt::resetPluginsSettings()
{
    this->cleanPluginList();
}

void ACSegmentationControlsWidgetQt::resetMediaType(ACMediaType _media_type)
{
    this->cleanPluginList();
    emit this->rebuildPluginList();
}

void ACSegmentationControlsWidgetQt::cleanPluginList()
{
    comboBoxPlugins->clear();
    if(parametersContainer){
        vboxLayout->removeWidget(parametersContainer);
        delete parametersContainer;
    }
    parametersContainer = 0;
}

void ACSegmentationControlsWidgetQt::buildPluginList()
{
    this->cleanPluginList();

    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;

    int activePlugins = media_cycle->getPluginManager()->getActiveSegmentPluginsSize(media_cycle->getMediaType());
    std::string current_plugin_name("");
    if(activePlugins == 1){
        current_plugin_name = media_cycle->getPluginManager()->getActiveSegmentPluginsNames(media_cycle->getMediaType()).front();
        std::cout << "Already active plugin: " << current_plugin_name << std::endl;
    }
    else if(activePlugins == 0){
        current_plugin_name = media_cycle->getPluginManager()->getAvailableSegmentPluginsNames(media_cycle->getMediaType()).front();
        media_cycle->getPluginManager()->setActiveSegmentPlugin( current_plugin_name );
        std::cout << "Default active plugin: " << current_plugin_name << std::endl;
    }

    // Build plugin list
    std::vector<std::string> availableSegmentPlugins = media_cycle->getPluginManager()->getAvailableSegmentPluginsNames( media_cycle->getMediaType() );
    for (std::vector<std::string>::iterator availableSegmentPlugin = availableSegmentPlugins.begin(); availableSegmentPlugin != availableSegmentPlugins.end(); availableSegmentPlugin++){
        QString plugin_name = QString((*availableSegmentPlugin).c_str());
        comboBoxPlugins->addItem(plugin_name);
        comboBoxPlugins->setEnabled(true);
        ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(*availableSegmentPlugin);

        QFont font = QFont(QApplication::font().defaultFamily(),10);
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
                float init = plugin->getNumberParameterInit(*numParamName);
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
#else
                    slider->setBackgroundStyle(QwtSlider::Groove);
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

    // If no segmentation plugin exist
    if(comboBoxPlugins->count()==0){
        comboBoxPlugins->addItem("None");
        comboBoxPlugins->setEnabled(false);
    }
    emit this->readjustHeight();
}

void ACSegmentationControlsWidgetQt::adjustHeight(){
    if(comboBoxPlugins){
        if(parametersContainer){
            parametersContainer->adjustSize();
            comboBoxPlugins->setMinimumHeight( comboBoxPlugins->height() );
            this->setMinimumHeight( comboBoxPlugins->minimumHeight() + parametersContainer->minimumHeight() );
            parametersContainer->adjustSize();
        }
        else{
            this->setMinimumHeight( comboBoxPlugins->minimumHeight() );
        }
    }
    else{
        this->setMinimumHeight( 0 );
    }
    this->adjustSize();
}
