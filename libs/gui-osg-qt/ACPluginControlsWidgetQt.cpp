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
    vboxLayout = new QVBoxLayout();
    vboxLayout->setMargin(0);
    comboBoxPlugins = new QComboBox();
    comboBoxPlugins->addItem("None");
    comboBoxPlugins->setEnabled(false);
    vboxLayout->addWidget(comboBoxPlugins);
    this->setLayout(vboxLayout);
    //this->show();
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
    if (media_cycle == 0)
        if (media_cycle->getPluginManager() == 0) return;
    std::string pluginName = text.toStdString();
    if(pluginName != "None")
        media_cycle->changeActivePlugin(this->pluginType,pluginName);
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
        //vboxLayout->removeWidget(parametersContainer);
        delete parametersContainer;
        //delete parametersContainer->layout();
        //qDeleteAll(parametersContainer->children());
    }
    parametersContainer = 0;

    comboBoxPlugins->addItem("None");
    comboBoxPlugins->setEnabled(false);
}

void ACPluginControlsWidgetQt::buildPluginList()
{
    if (media_cycle == 0) return;
    if (media_cycle->getPluginManager() == 0) return;

    std::vector<std::string> availablePlugins = media_cycle->getAvailablePluginNames( this->pluginType , media_cycle->getMediaType() );

    std::string current_plugin_name("");
    /*if(this->pluginType == PLUGIN_TYPE_SEGMENTATION){
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
        //std::cout << "Current plugin name " << current_plugin_name << std::endl;
        if(current_plugin_name == ""){
            current_plugin_name = comboBoxPlugins->currentText().toStdString();
            if(current_plugin_name == "" || current_plugin_name == "None")
                if(availablePlugins.size()>0)
                    current_plugin_name = availablePlugins.front();
            media_cycle->changeActivePlugin(this->pluginType,current_plugin_name);
        }
    }*/

    current_plugin_name = comboBoxPlugins->currentText().toStdString();
    std::vector<std::string> active_plugin_names = media_cycle->getActivePluginNames( this->pluginType );
    if(current_plugin_name == "" || current_plugin_name == "None"){
        if(active_plugin_names.size() == 0){
            if(availablePlugins.size()>0){
                current_plugin_name = availablePlugins.front();
                media_cycle->changeActivePlugin(this->pluginType,current_plugin_name);
            }
        }
        else if(active_plugin_names.size() >= 1){
            current_plugin_name = active_plugin_names.front();
        }
    }

    this->cleanPluginList();
    int index = -1;
    int count = -1;

    // Build plugin list
    comboBoxPlugins->clear();
    for (std::vector<std::string>::iterator availablePlugin = availablePlugins.begin(); availablePlugin != availablePlugins.end(); availablePlugin++){
        QString plugin_name = QString((*availablePlugin).c_str());

        ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(*availablePlugin);

        //QFont font = QFont(QApplication::font().defaultFamily(),10);
        QFont font = comboBoxPlugins->font();
        font.setPointSize(10);
        comboBoxPlugins->setFont(font);

        // For plugin types with one active plugin at a time, the combox box is used to set the active plugin
        // For other types, we don't need to list plugins that don't have parameters
        if(plugin->getParametersCount()>0
                || plugin->implementsPluginType(PLUGIN_TYPE_SEGMENTATION)
                || plugin->implementsPluginType(PLUGIN_TYPE_CLUSTERS_METHOD)
                || plugin->implementsPluginType(PLUGIN_TYPE_CLUSTERS_POSITIONS)
                || plugin->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_METHOD)
                || plugin->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_POSITIONS)
                ){
            comboBoxPlugins->addItem(plugin_name);
            count++;
            if(plugin_name.toStdString() == current_plugin_name)
                index=count;
        }

        if(plugin_name.toStdString() == current_plugin_name && plugin->getParametersCount()>0){

            if(!parametersContainer)
                parametersContainer = new QWidget(this);
            QGridLayout* layout = new QGridLayout();
            layout->setMargin(0);
            layout->setHorizontalSpacing(1);
            layout->setVerticalSpacing(5);
            int minHeight = 0;
            QPushButton* pushbutton = new QPushButton("Reset");
            pushbutton->setFont(font);

            int paramIdx = 0;

            std::vector<std::string> callbacksNames = plugin->getCallbacksNames();
            std::vector<std::string> strParamNames = plugin->getStringParametersNames();
            std::vector<std::string> numParamNames = plugin->getNumberParametersNames();

            // Check if parameters contain repeated first words
            std::map<std::string,std::string> parameter_word;
            std::map<std::string,int> word_count;
            std::map<std::string,int> widget_count;
            for(std::vector<std::string>::iterator callbackName=callbacksNames.begin();callbackName!=callbacksNames.end();callbackName++){
                std::string word(*callbackName);
                size_t space = word.find_first_of(" ");
                if(space != string::npos){
                    word = word.substr(0,space);
                    parameter_word[*callbackName] = word;
                }
            }
            for(std::vector<std::string>::iterator strParamName=strParamNames.begin();strParamName!=strParamNames.end();strParamName++){
                std::string word(*strParamName);
                size_t space = word.find_first_of(" ");
                if(space != string::npos){
                    word = word.substr(0,space);
                    parameter_word[*strParamName] = word;
                }
            }
            for(std::vector<std::string>::iterator numParamName=numParamNames.begin();numParamName!=numParamNames.end();numParamName++){
                std::string word(*numParamName);
                size_t space = word.find_first_of(" ");
                if(space != string::npos){
                    word = word.substr(0,space);
                    parameter_word[*numParamName] = word;
                }
            }
            for(std::map<std::string,std::string>::iterator word = parameter_word.begin();word != parameter_word.end();word++){
                std::map<std::string,int>::iterator count = word_count.find(word->second);
                if(count==word_count.end()){
                    word_count[word->second]=0;
                }
                word_count[word->second] += 1;
                widget_count[word->second] = 0;
            }
            // Create a tab for each first word repeated at least twice, to group parameters
            QTabWidget* tabWidget = 0;
            std::map<std::string,int> tab_index;
            int _count = 0;
            for(std::map<std::string,int>::iterator count = word_count.begin();count != word_count.end();count++){
                if(count->second > 1){
                    if(!tabWidget){
                        tabWidget = new QTabWidget();
                        tabWidget->setFont(font);
                        //connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(adjustHeight()));
                    }
                    QGridLayout* _layout = new QGridLayout();
                    _layout->setMargin(0);
                    _layout->setHorizontalSpacing(1);
                    _layout->setVerticalSpacing(5);
                    QWidget* _widget = new QWidget();
                    _widget->setLayout(_layout);
                    tabWidget->addTab(_widget,QString(count->first.c_str()));
                    tab_index[count->first]=_count++;
                    //std::cout << "ACPluginControlsWidgetQt::buildPluginList: plugin '" << current_plugin_name << "' has a parameter group '" << count->first << "' with " << count->second << " elements" << std::endl;
                }
            }

            //std::cout << "Number of callbacks " << clbParamNames.size() << std::endl;
            for(std::vector<std::string>::iterator callbackName=callbacksNames.begin();callbackName!=callbacksNames.end();callbackName++){
                QString name = QString((*callbackName).c_str());
                QString desc = QString( plugin->getCallbackDesc(*callbackName).c_str());

                QPushButton* callbackButton = new QPushButton(name);
                callbackButton->setFont(font);

                if(tabWidget && word_count[parameter_word[*callbackName]] > 1){
                    dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*callbackName]])->layout())->addWidget(callbackButton,widget_count[parameter_word[*callbackName]],0);
                    widget_count[parameter_word[*callbackName]] += 1;
                }
                else{
                    layout->addWidget(callbackButton,paramIdx++,0);
                    callbackButton->adjustSize();
                    minHeight += callbackButton->height();
                }

                ACPluginParameterQt* callbackParameter = new ACPluginParameterQt(plugin,name);
                connect(callbackButton,SIGNAL(clicked()),callbackParameter,SLOT(triggerCallback()));
            }

            //std::cout << "Number of string parameters " << strParamNames.size() << std::endl;
            for(std::vector<std::string>::iterator strParamName=strParamNames.begin();strParamName!=strParamNames.end();strParamName++){
                //std::cout << "String parameter: " << (*strParamName) << std::endl;//" = " << (*strParamName).name << std::endl;

                QString name = QString((*strParamName).c_str());
                std::vector<std::string> names = plugin->getStringParameterValues(*strParamName);
                QString desc = QString( plugin->getStringParameterDesc(*strParamName).c_str());

                QLabel* label = new QLabel(name);
                label->setFont(font);

                ACPluginParameterQt* stringParameter = new ACPluginParameterQt(plugin,name);

                if(names.size() > 0){
                    QComboBox* combobox = new QComboBox();
                    for (std::vector<std::string>::iterator name = names.begin();name!=names.end();name++){
                        combobox->addItem(QString((*name).c_str()));
                    }
                    combobox->setCurrentIndex( plugin->getStringParameterValueIndex(*strParamName) );
                    combobox->setFont(font);


                    if(tabWidget && word_count[parameter_word[*strParamName]] > 1){
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*strParamName]])->layout())->addWidget(label,widget_count[parameter_word[*strParamName]],0);
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*strParamName]])->layout())->addWidget(combobox,widget_count[parameter_word[*strParamName]],1,1,2);
                        widget_count[parameter_word[*strParamName]] += 1;
                    }
                    else{
                        layout->addWidget(label,paramIdx,0);
                        layout->addWidget(combobox,paramIdx++,1,1,2);
                        combobox->adjustSize();
                        minHeight += combobox->height();
                    }

                    connect(combobox,SIGNAL(currentIndexChanged(QString)),stringParameter,SLOT(updateStringParameter(QString)) );
                    connect(pushbutton,SIGNAL(clicked()),stringParameter,SLOT(resetStringParameter()));
                    connect(stringParameter,SIGNAL(stringParameterIndexChanged(int)),combobox,SLOT(setCurrentIndex(int)));

                }
                else{
                    QLineEdit* lineedit = new QLineEdit();
                    lineedit->setText( QString( plugin->getStringParameterInit(*strParamName).c_str()) );
                    lineedit->setFont(font);

                    if(tabWidget && word_count[parameter_word[*strParamName]] > 1){
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*strParamName]])->layout())->addWidget(label,widget_count[parameter_word[*strParamName]],0);
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*strParamName]])->layout())->addWidget(lineedit,widget_count[parameter_word[*strParamName]],1,1,2);
                        widget_count[parameter_word[*strParamName]] += 1;
                    }
                    else{
                        layout->addWidget(label,paramIdx,0);
                        layout->addWidget(lineedit,paramIdx++,1,1,2);
                        lineedit->adjustSize();
                        minHeight += lineedit->height();
                    }

                    connect(lineedit,SIGNAL( textChanged(QString)),stringParameter,SLOT(updateStringParameter(QString)) );
                    connect(pushbutton,SIGNAL(clicked()),stringParameter,SLOT(resetStringParameter()));

                }
            }

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

                if(min==0 && max==1 && step==1){

                    QLabel* label = new QLabel(name);
                    label->setFont(font);

                    QCheckBox* checkbox = new QCheckBox("");
                    checkbox->setChecked((bool)init);

                    if(tabWidget && word_count[parameter_word[*numParamName]] > 1){
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*numParamName]])->layout())->addWidget(label,widget_count[parameter_word[*numParamName]],0);
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*numParamName]])->layout())->addWidget(checkbox,widget_count[parameter_word[*numParamName]],1);
                        widget_count[parameter_word[*numParamName]] += 1;
                    }
                    else{
                        layout->addWidget( label,paramIdx,0);
                        layout->addWidget( checkbox,paramIdx++,1);
                        checkbox->adjustSize();
                        minHeight += checkbox->height();
                    }

                    connect(checkbox,SIGNAL(toggled(bool)),numberParameter,SLOT(updateNumberParameter(bool)));
                    connect(numberParameter,SIGNAL(numberParameterChanged(bool)),checkbox,SLOT(setChecked(bool)));

                    /*QPushButton* callbackButton = new QPushButton(name);
                    callbackButton->setCheckable(true);
                    if(init == 1)
                        callbackButton->setChecked(true);
                    callbackButton->setFont(font);
                    layout->addWidget(callbackButton,paramIdx++,0);

                    ACPluginParameterQt* callbackParameter = new ACPluginParameterQt(plugin,name);

                    connect(callbackButton,SIGNAL(toggled(bool)),callbackParameter,SLOT(updateNumberParameter(bool)));
                    callbackButton->adjustSize();
                    minHeight += callbackButton->height();*/
                }
                else{

                    QLabel* label = new QLabel(name);
                    label->setFont(font);

                    QwtSlider* slider = new QwtSlider(0);
#if QWT_VERSION > 0x060000
                    slider->setLowerBound(min);
                    slider->setUpperBound(max);
                    slider->setScaleStepSize(step);
#else
                    slider->setRange(min,max,step);
#endif
                    slider->setValue(init);

#if QWT_VERSION > 0x060000
                    slider->setGroove(true);
                    slider->setHandleSize(QSize(12,12));
                    slider->setOrientation(Qt::Horizontal);
                    slider->setScalePosition(QwtSlider::NoScale);
                    slider->setTrough(false);
#elif QWT_VERSION < 0x060000
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

                    if(tabWidget && word_count[parameter_word[*numParamName]] > 1){
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*numParamName]])->layout())->addWidget(label,widget_count[parameter_word[*numParamName]],0);
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*numParamName]])->layout())->addWidget(slider,widget_count[parameter_word[*numParamName]],1);
                        dynamic_cast<QGridLayout*>(tabWidget->widget(tab_index[parameter_word[*numParamName]])->layout())->addWidget(spinbox,widget_count[parameter_word[*numParamName]],2);
                        widget_count[parameter_word[*numParamName]] += 1;
                    }
                    else{
                        layout->addWidget( label,paramIdx,0);
                        layout->addWidget( slider,paramIdx,1);
                        layout->addWidget( spinbox,paramIdx++,2);
                        spinbox->adjustSize();
                        minHeight += spinbox->height();
                    }

                    connect(slider,SIGNAL(valueChanged(double)),spinbox,SLOT(setValue(double)));
                    connect(spinbox,SIGNAL(valueChanged(double)),slider,SLOT(setValue(double)));

                    connect(slider,SIGNAL(valueChanged(double)),numberParameter,SLOT(updateNumberParameter(double)));
                    connect(numberParameter,SIGNAL(numberParameterChanged(double)),slider,SLOT(setValue(double)));
                }
            }

            if(tabWidget){
                layout->addWidget(tabWidget, paramIdx++,0,1,3);
                if(tabWidget->currentWidget())
                    tabWidget->currentWidget()->adjustSize();
                tabWidget->adjustSize();
                minHeight += tabWidget->height();
            }

            layout->addWidget(pushbutton, paramIdx,0);
            pushbutton->adjustSize();
            minHeight += pushbutton->height();

            if(!parametersContainer)
                parametersContainer = new QWidget(this);
            parametersContainer->setLayout(layout);
            vboxLayout->addWidget(parametersContainer);
            parametersContainer->setMinimumHeight(minHeight);
        }
    }

    //int index = -1;

    /*std::vector<std::string>::iterator availablePlugin = std::find(availablePlugins.begin(),availablePlugins.end(),current_plugin_name);
    if(availablePlugin != availablePlugins.end()){
        index = std::distance(availablePlugins.begin(),availablePlugin);
        comboBoxPlugins->setCurrentIndex(index);
    }*/
    /*index = comboBoxPlugins->findText(QString(current_plugin_name.c_str()));*/
    if (index != -1){
        comboBoxPlugins->setCurrentIndex(index);
    }

    comboBoxPlugins->setEnabled(false);
    // If no plugin exists
    if(comboBoxPlugins->count()==0)
        comboBoxPlugins->addItem("None");
    else if(comboBoxPlugins->count()>1)
        comboBoxPlugins->setEnabled(true);
    usleep(10000);
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
