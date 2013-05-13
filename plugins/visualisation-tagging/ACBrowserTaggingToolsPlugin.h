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

#include <MediaCycle.h>
#include <ACPlugin.h>
#include <ACPluginQt.h>

#ifndef ACBrowserTaggingToolsPlugin_H
#define	ACBrowserTaggingToolsPlugin_H

class ACBrowserTaggingToolsPlugin : public QObject, public ACPluginQt, virtual public ACClientPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACBrowserTaggingToolsPlugin();
    virtual ~ACBrowserTaggingToolsPlugin();

    // From ACPluginQt
    virtual std::vector<ACInputActionQt*> providesInputActions();
    virtual std::map<std::string,ACMediaType> availableMediaActions();

public slots:
    void changeSelectedMediaTagId0(bool trig);
    void changeSelectedMediaTagId1(bool trig);
    void changeSelectedMediaTagId2(bool trig);
    void changeSelectedMediaTagId3(bool trig);
    void changeSelectedMediaTagId4(bool trig);
    void changeSelectedMediaTagId5(bool trig);
    void changeSelectedMediaTagId6(bool trig);
    void changeSelectedMediaTagId7(bool trig);
    void changeSelectedMediaTagId8(bool trig);
    void changeSelectedMediaTagId9(bool trig);
    void resetSelectedMediaTagId(bool trig);
    void transferClassToTag(bool trig);
    void cleanAllTags(bool trig);

private:
    ACInputActionQt* resetSelectedMediaTagIdAction;
    ACInputActionQt* transferClassToTagAction;
    ACInputActionQt* cleanAllTagsAction;
    std::vector<ACInputActionQt*> changeSelectedMediaTagIdAction;
};

#endif

