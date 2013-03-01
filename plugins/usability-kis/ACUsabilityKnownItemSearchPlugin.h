/**
 * @brief Plugin for contesting in the video browser showdown
 * @author Christian Frisson
 * @date 2/01/2013
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
#include <ACPluginControlsDockWidgetQt.h>

#include <stdio.h>
#include <pthread.h>


#ifndef ACUsabilityKnownItemSearchPlugin_H
#define	ACUsabilityKnownItemSearchPlugin_H

class ACUsabilityKnownItemSearchPlugin : public QObject, public ACPluginQt, virtual public ACClientPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACUsabilityKnownItemSearchPlugin();
    virtual ~ACUsabilityKnownItemSearchPlugin();

    // From ACPluginQt
    virtual std::vector<ACInputActionQt*> providesInputActions();
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::string value="");
    virtual std::map<std::string,ACMediaType> availableMediaActions();

public slots:
    void changeServer();
    void changePort();
    void changeTeam();
    void submitCallback();
    void triggerMediaHover(bool trigger); // audio hover, image/video ... -> need a panel to configure the behaviour of hover

private:
    ACInputActionQt* submitAction;
    ACInputActionQt* triggerMediaHoverAction;
    std::string url;
    int port;
    int team;
};

#endif

