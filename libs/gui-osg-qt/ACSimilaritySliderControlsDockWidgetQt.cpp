/**
 * @brief A dock that provides a list of features whose weights can be changed by sliders
 * @author Christian Frisson
 * @date 12/07/2012
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

#include "ACSimilaritySliderControlsDockWidgetQt.h"

#include <qwt.h>
#include <qwt_slider.h>

ACSimilaritySliderControlsDockWidgetQt::ACSimilaritySliderControlsDockWidgetQt(QWidget *parent)
    : ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACSimilaritySliderControlsDockWidgetQt")
{
    ui.setupUi(this); // first thing to do
    //connect(ui.featuresTableWidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(modifyListItem(QListWidgetItem*)));
    connect(ui.featuresTableWidget, SIGNAL(cellClicked(int,int)),this, SLOT(modifyListItem(int,int)));
    connect(this,SIGNAL(reconfigureSliders()),this,SLOT(configureSliders()));
    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));

    sliderMapper = new QSignalMapper(this);

    this->show();
    emit this->readjustHeight();
}

bool ACSimilaritySliderControlsDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACSimilaritySliderControlsDockWidgetQt::modifyListItem(int row, int column)
{
    if (osg_view && osg_view->isLibraryLoaded())
    {
        if(ui.featuresTableWidget->cellWidget(row,1)==0){
            std::cerr << "ACSimilaritySliderControlsDockWidgetQt::modifyListItem: couldn't access slider at row " << row << std::endl;
            return;
        }
        QwtSlider* slider = dynamic_cast<QwtSlider*>(ui.featuresTableWidget->cellWidget(row,1));
        if(!slider){
            std::cerr << "ACSimilaritySliderControlsDockWidgetQt::modifyListItem: couldn't access slider at row " << row << std::endl;
            return;
        }
        if(column == 1){
            float w = slider->value();
            if(w == 1.0f)
                ui.featuresTableWidget->item(row,0)->setCheckState(Qt::Checked);
            else if(w == 0.0f )
                ui.featuresTableWidget->item(row,0)->setCheckState(Qt::Unchecked);
            else
                ui.featuresTableWidget->item(row,0)->setCheckState(Qt::PartiallyChecked);
            media_cycle->setWeight(row,w);
            media_cycle->updateDisplay(true);
        }
        else if(column == 0){
            float w;
            if (ui.featuresTableWidget->item(row,0)->checkState() == Qt::Unchecked) w = 0.0f;
            else w = 1.0f;
            slider->setValue(w);
            media_cycle->setWeight(row,w);
            media_cycle->updateDisplay(true);
        }
    }
}

void ACSimilaritySliderControlsDockWidgetQt::resetMode(){
    if(!media_cycle) return;

    switch (media_cycle->getBrowserMode()){
    case AC_MODE_CLUSTERS:
    {
        ui.featuresTableWidget->setEnabled(true); //CF until changing feature weights works in neighbors mode
    }
        break;
    case AC_MODE_NEIGHBORS:
    {
        ui.featuresTableWidget->setEnabled(false); //CF until changing feature weights works in neighbors mode
    }
        break;
    default:
        break;
    }
}

void ACSimilaritySliderControlsDockWidgetQt::updatePluginsSettings()
{
    emit this->reconfigureSliders();

    if(!media_cycle) return;

    this->resetMode();

    emit this->readjustHeight();
}

void ACSimilaritySliderControlsDockWidgetQt::resetPluginsSettings()
{
    this->cleanSliders();

    if(!media_cycle) return;

    this->resetMode();

    emit this->readjustHeight();
}

void ACSimilaritySliderControlsDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    //this->updatePluginLists();
    if(!media_cycle) return;

    this->resetMode();

    emit this->readjustHeight();
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACSimilaritySliderControlsDockWidgetQt::synchronizeFeaturesWeights()
{
    if (media_cycle == 0) return;
    vector<float> w = media_cycle->getWeightVector();
    int nw = w.size();
    if (nw==0)
    {
        cout << "features not yet computed from plugins; setting all weights to 0" << endl;
        //for (int i=0; i< ui.featuresTableWidget->count(); i++)
        for (int i=0; i< ui.featuresTableWidget->rowCount(); i++)
        {
            ui.featuresTableWidget->item(i,0)->setCheckState (Qt::Unchecked);
            //ui.featuresTableWidget->item(i)->setCheckState (Qt::Unchecked);
        }
        return;
    }
    //else if (ui.featuresTableWidget->count() != nw)
    else if (ui.featuresTableWidget->rowCount() != nw)
    {
        cerr << "Warning: Sliders in GUI do not match Features in MediaCycle" << endl;
        //cerr << ui.featuresTableWidget->count() << "!=" << nw << endl;
        cerr << ui.featuresTableWidget->rowCount() << "!=" << nw << endl;
        return;
    }
    else
    {
        for(int item = 0; item < ui.featuresTableWidget->rowCount(); item++){
            if(ui.featuresTableWidget->cellWidget(item,1)){
                QwtSlider* slider = dynamic_cast<QwtSlider*>(ui.featuresTableWidget->cellWidget(item,1));
                if(slider){
                    slider->setValue(w[item]);
                }
                else
                    std::cerr << "ACSimilaritySliderControlsDockWidgetQt::on_pushButtonNone_clicked: could't access weight slider at row " << item << std::endl;
            }
        }
    }
    emit this->readjustHeight();
}

void ACSimilaritySliderControlsDockWidgetQt::configureSliders()
{
    std::cout << "ACSimilaritySliderControlsDockWidgetQt::configureSliders" << std::endl;

    // dynamic config of Sliders
    // according to plugins actually used to compute the features
    if (media_cycle == 0) return;

    vector<string> plugins_list = this->media_cycle->getListOfActivePlugins();
    vector<string> ::iterator list_iter;

    ui.featuresTableWidget->setRowCount(plugins_list.size());
    ui.featuresTableWidget->setColumnCount(2);
    ui.featuresTableWidget->horizontalHeader()->resizeSection(0, 130);
    ui.featuresTableWidget->horizontalHeader()->resizeSection(1, 80);

    int row = 0;
    for (list_iter = plugins_list.begin(); list_iter != plugins_list.end(); list_iter++)
    {
        QString s((*list_iter).c_str());
        //s = s.remove("Mean of ");
        //s = s.remove("Interpolated ");
        QTableWidgetItem* item = new QTableWidgetItem(s);
        ui.featuresTableWidget->setItem(row,0,item);

        float min(0.0f),max(1.0f),step(0.01f),init(media_cycle->getWeight(row));
        QwtSlider* slider = new QwtSlider(0);
#if QWT_VERSION >= 0x060100
        slider->setLowerBound(min);
        slider->setUpperBound(max);
        slider->setScaleStepSize(step);
#else
        slider->setRange(min,max,step);
#endif
        slider->setValue(init);

#if QWT_VERSION >= 0x060100
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

        //slider->setToolTip(desc);
        //slider->setAccessibleDescription(desc);

        sliderMapper->setMapping(slider, row);
        connect(slider, SIGNAL(sliderReleased()),
                sliderMapper, SLOT (map()));
        connect(sliderMapper, SIGNAL(mapped(int)),
                this, SLOT(changeWeight(int)));
        ui.featuresTableWidget->setCellWidget(row,1,slider);
        row++;

        if(init == 1.0f)
            item->setCheckState(Qt::Checked);
        else if(init == 0.0f )
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::PartiallyChecked);
    }

    this->synchronizeFeaturesWeights();

    connect(ui.featuresTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)),
            this, SLOT(modifyListItem(QTableWidgetItem*)));

    emit this->readjustHeight();
}

void ACSimilaritySliderControlsDockWidgetQt::changeWeight(int index){
    if(!media_cycle) return;
    QwtSlider* slider = 0;
    if(ui.featuresTableWidget->cellWidget(index,1)){
        slider = dynamic_cast<QwtSlider*>(ui.featuresTableWidget->cellWidget(index,1));
    }
    if(!slider){
        std::cerr << "ACSimilaritySliderControlsDockWidgetQt::changeWeight: couldn't access weight slider at row " << index << std::endl;
        return;
    }
    float w = slider->value();
    std::cout << "Changing weight to " << w << " for feature " << index << std::endl;

    if(w == 1.0f)
        ui.featuresTableWidget->item(index,0)->setCheckState(Qt::Checked);
    else if(w == 0.0f )
        ui.featuresTableWidget->item(index,0)->setCheckState(Qt::Unchecked);
    else
        ui.featuresTableWidget->item(index,0)->setCheckState(Qt::PartiallyChecked);

    media_cycle->setWeight(index,w);
    media_cycle->updateDisplay(true);
    //XS 250310 was: media_cycle->updateClusters(true);
    // XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
    //osg_view->updateTransformsFromBrowser(0.0);
}

void ACSimilaritySliderControlsDockWidgetQt::cleanSliders()
{
    std::cout << "ACSimilaritySliderControlsDockWidgetQt::cleanSliders" << std::endl;

    //for(int i = 0; i < ui.featuresTableWidget->count(); i++)
    //delete ui.featuresTableWidget->takeItem(i);
    for(int i = 0; i < ui.featuresTableWidget->rowCount(); i++)
        delete ui.featuresTableWidget->takeItem(i,0);

    for(int item = 0; item < ui.featuresTableWidget->rowCount(); item++){
        if(ui.featuresTableWidget->cellWidget(item,1)){
            QwtSlider* slider = dynamic_cast<QwtSlider*>(ui.featuresTableWidget->cellWidget(item,1));
            if(slider){
                sliderMapper->removeMappings(slider);
                delete slider;
            }
            else
                std::cerr << "ACSimilaritySliderControlsDockWidgetQt::cleanSliders: could't access weight slider at row " << item << std::endl;
        }
    }

    ui.featuresTableWidget->clear();
    emit this->readjustHeight();
}

void ACSimilaritySliderControlsDockWidgetQt::on_pushButtonAll_clicked(){
    for(int item = 0; item < ui.featuresTableWidget->rowCount(); item++){
        if(ui.featuresTableWidget->cellWidget(item,1)){
            QwtSlider* slider = dynamic_cast<QwtSlider*>(ui.featuresTableWidget->cellWidget(item,1));
            if(slider){
                slider->setValue(1.0f);
                ui.featuresTableWidget->item(item,0)->setCheckState(Qt::Checked);
                media_cycle->setWeight(item,1.0f);

            }
            else
                std::cerr << "ACSimilaritySliderControlsDockWidgetQt::on_pushButtonAll_clicked: could't access weight slider at row " << item << std::endl;
        }
    }
    media_cycle->updateDisplay(true);
    //osg_view->updateTransformsFromBrowser(0.0);
}

void ACSimilaritySliderControlsDockWidgetQt::on_pushButtonNone_clicked(){
    for(int item = 0; item < ui.featuresTableWidget->rowCount(); item++){
        if(ui.featuresTableWidget->cellWidget(item,1)){
            QwtSlider* slider = dynamic_cast<QwtSlider*>(ui.featuresTableWidget->cellWidget(item,1));
            if(slider){
                slider->setValue(0.0f);
                ui.featuresTableWidget->item(item,0)->setCheckState(Qt::Unchecked);
                media_cycle->setWeight(item,0.0f);
            }
            else
                std::cerr << "ACSimilaritySliderControlsDockWidgetQt::on_pushButtonNone_clicked: could't access weight slider at row " << item << std::endl;
        }
    }
    media_cycle->updateDisplay(true);
    //osg_view->updateTransformsFromBrowser(0.0);
}

void ACSimilaritySliderControlsDockWidgetQt::adjustHeight(){
}

