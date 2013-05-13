/**
 * @brief Plugin for adding browser navigation tools
 * @author Christian Frisson
 * @date 5/03/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACBrowserTaggingToolsPlugin.h"
#include<MediaCycle.h>

#include <iostream>

using namespace std;

ACBrowserTaggingToolsPlugin::ACBrowserTaggingToolsPlugin()
    : QObject(), ACPluginQt(), ACClientPlugin(),
    resetSelectedMediaTagIdAction(0),transferClassToTagAction(0),cleanAllTagsAction(0)
{
    this->mName = "Browser Tagging Tools";
    this->mDescription ="Plugin for adding browser tagging tools";
    this->mMediaType = MEDIA_TYPE_ALL;

    //QSignalMapper *changeSelectedMediaTagIdMapper = new QSignalMapper();
    for (int i=0;i<10;i++){
        changeSelectedMediaTagIdAction.push_back( new ACInputActionQt(tr("Set Cluster Id"), this));
        changeSelectedMediaTagIdAction[i]->setToolTip(tr("Change Cluster Id of the clicked Node"));
        changeSelectedMediaTagIdAction[i]->setShortcut(Qt::Key_0+i);
        changeSelectedMediaTagIdAction[i]->setKeyEventType(QEvent::KeyPress);
        //changeSelectedMediaTagIdAction[i]->setMouseEventType(QEvent::MouseButtonPress);
 //       changeSelectedMediaTagIdMapper->setMapping(changeSelectedMediaTagIdAction[i], i);
 //       connect(changeSelectedMediaTagIdMapper, SIGNAL(mapped(const int &)), this, SLOT(changeSelectedMediaTagId(const int &)));
    }
    connect(changeSelectedMediaTagIdAction[0], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId0(bool)));
    connect(changeSelectedMediaTagIdAction[1], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId1(bool)));
    connect(changeSelectedMediaTagIdAction[2], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId2(bool)));
    connect(changeSelectedMediaTagIdAction[3], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId3(bool)));
    connect(changeSelectedMediaTagIdAction[4], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId4(bool)));
    connect(changeSelectedMediaTagIdAction[5], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId5(bool)));
    connect(changeSelectedMediaTagIdAction[6], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId6(bool)));
    connect(changeSelectedMediaTagIdAction[7], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId7(bool)));
    connect(changeSelectedMediaTagIdAction[8], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId8(bool)));
    connect(changeSelectedMediaTagIdAction[9], SIGNAL(triggered(bool)), this, SLOT(changeSelectedMediaTagId9(bool)));

    resetSelectedMediaTagIdAction = new ACInputActionQt(tr("Reset Cluster Id"), this);
    resetSelectedMediaTagIdAction->setToolTip(tr("Reset Cluster Id of the clicked Node"));
    resetSelectedMediaTagIdAction->setShortcut(Qt::Key_U);
    resetSelectedMediaTagIdAction->setKeyEventType(QEvent::KeyPress);
    //resetSelectedMediaTagIdAction->setMouseEventType(QEvent::MouseButtonPress);
    connect(resetSelectedMediaTagIdAction, SIGNAL(triggered(bool)), this, SLOT(resetSelectedMediaTagId(bool)));

    transferClassToTagAction = new ACInputActionQt(tr("Transfer classification to the tags"), this);
    transferClassToTagAction->setToolTip(tr("Record node classification in the tag field of the medias"));
    transferClassToTagAction->setShortcut(Qt::CTRL+Qt::Key_M);
    transferClassToTagAction->setKeyEventType(QEvent::KeyPress);
    connect(transferClassToTagAction, SIGNAL(triggered(bool)), this, SLOT(transferClassToTag(bool)));

    cleanAllTagsAction = new ACInputActionQt(tr("Clean the tags"), this);
    cleanAllTagsAction->setToolTip(tr("Clean the tag in every loaded media"));
    cleanAllTagsAction->setShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_M);
    cleanAllTagsAction->setKeyEventType(QEvent::KeyPress);
    connect(cleanAllTagsAction, SIGNAL(triggered(bool)), this, SLOT(cleanAllTags(bool)));
}

ACBrowserTaggingToolsPlugin::~ACBrowserTaggingToolsPlugin(){
    /*if(changeSelectedMediaTagIdAction.size()>0){
        for (int i=0;i<10;i++){
            if (changeSelectedMediaTagIdAction[i])
                delete changeSelectedMediaTagIdAction[i];
            changeSelectedMediaTagIdAction[i] = 0;
        }
        changeSelectedMediaTagIdAction.clear();
    }
    if(resetSelectedMediaTagIdAction) delete resetSelectedMediaTagIdAction; resetSelectedMediaTagIdAction = 0;
    if(transferClassToTagAction) delete transferClassToTagAction; transferClassToTagAction = 0;
    if(cleanAllTagsAction) delete cleanAllTagsAction; cleanAllTagsAction = 0;*/
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId0(bool trig){
    if (media_cycle == 0) return;
    int newId=0;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId1(bool trig){
    if (media_cycle == 0) return;
    int newId=1;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId2(bool trig){
    if (media_cycle == 0) return;
    int newId=2;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId3(bool trig){
    if (media_cycle == 0) return;
    int newId=3;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId4(bool trig){
    if (media_cycle == 0) return;
    int newId=4;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId5(bool trig){
    if (media_cycle == 0) return;
    int newId=5;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId6(bool trig){
    if (media_cycle == 0) return;
    int newId=6;
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId7(bool trig){
    if (media_cycle == 0) return;
    int newId=7;
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId8(bool trig){
    if (media_cycle == 0) return;
    int newId=8;
    if (trig==false)
    media_cycle->changeSelectedMediaTagId(newId);
}

void ACBrowserTaggingToolsPlugin::changeSelectedMediaTagId9(bool trig){
    if (media_cycle == 0) return;
    int newId=9;
    media_cycle->changeSelectedMediaTagId(newId);
}
void ACBrowserTaggingToolsPlugin::resetSelectedMediaTagId(bool trig){
    if (media_cycle == 0) return;
    int newId=-1;
    media_cycle->changeSelectedMediaTagId(newId);
}
void ACBrowserTaggingToolsPlugin::transferClassToTag(bool trig){
    if (media_cycle == 0) return;
    int newId=-1;
    media_cycle->transferClassToTag();
}
void ACBrowserTaggingToolsPlugin::cleanAllTags(bool trig){
    if (media_cycle == 0) return;
    int newId=-1;
    media_cycle->cleanAllMediaTag();
}

std::map<std::string,ACMediaType> ACBrowserTaggingToolsPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    return media_actions;
}

std::vector<ACInputActionQt*> ACBrowserTaggingToolsPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    actions.push_back(resetSelectedMediaTagIdAction);
    actions.push_back(transferClassToTagAction);
    actions.push_back(cleanAllTagsAction);
    for (int i=0;i<10;i++){
        actions.push_back(changeSelectedMediaTagIdAction[i]);
    }
    return actions;
}
