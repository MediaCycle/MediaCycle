/*
 *  ACAboutDialogQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 18/02/11
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

#include "ACAboutDialogQt.h"

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

ACAboutDialogQt::ACAboutDialogQt( QWidget* parent)
    : ACAbstractAboutDialogQt(parent)
{
    ui.setupUi(this);
    coreAbout = ui.multiLineEditAbout->document()->toHtml();
    //this->updateAbout();
    this->updateLicenses();
    this->updateTeam();
    ui.appName->setText(QString("MediaCycle ") + QString(STRINGIZE_VALUE_OF(MC_VERSION)));
}

void ACAboutDialogQt::setMediaCycle(MediaCycle* _media_cycle)
{
    this->media_cycle = _media_cycle;
    this->updateAbout();
    this->updateLicenses();
}

void ACAboutDialogQt::updateAbout(){
    if(!media_cycle)
        return;
    ACAbstractDefaultConfig* currentConfig =  media_cycle->getCurrentConfig();
    QString html(coreAbout);
    if(currentConfig){
        html.append("<br/>");
        html.append(QString(currentConfig->description().c_str()));
    }
    ui.multiLineEditAbout->document()->setHtml(html);
}

void ACAboutDialogQt::updateLicenses(){
    QString html;
    html.append ("The license of the MediaCycle framework is not yet determined. We should opt for a dual commercial/community license soon.");
    html.append ("<br/><br/>");

    html.append ("Core dependencies:");
    html.append ("<ul>");
    html.append ("<li><a href=\"http://arma.sourceforge.net\">armadillo</a> (LGPL or GPL)</li>");
#ifndef __APPLE__ // part of the Accelerate framework
    html.append ("<li><a href=\"http://www.netlib.org/blas/\">Blas</a> (just requires attribution)</li>");
#endif//ndef __APPLE__
    html.append ("<li><a href=\"http://www.boost.org\">boost</a> (Boost Software License, attribution)</li>");
    html.append ("<li><a href=\"http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html\">fftooura</a> (&quot;You may use, copy, modify and distribute this code for any purpose (include commercial use) and without fee.&quot;)</li>");
    html.append ("<li><a href=\"http://www.aei.mpg.de/~peekas/tree/\">Kasper Peeters' STL-like templated tree class</a> (GPL)</li>");
#ifndef __APPLE__ // part of the Accelerate framework
    html.append ("<li><a href=\"http://www.netlib.org/lapack/\">LAPACK</a> (BSD)</li>");
#endif//ndef __APPLE__
    html.append ("<li><a href=\"http://sourceforge.net/projects/tinyxml\">TinyXML</a> (zlib/libpng)</li>");
    html.append ("</ul>");

    html.append ("GUI/app dependencies:");
    html.append ("<ul>");
#if defined (USE_BREAKPAD)
    html.append ("<li><a href=\"https://github.com/AlekSi/breakpad-qt\">AlekSi's breakpad-qt</a> (BSD)</li>");
    html.append ("<li><a href=\"http://code.google.com/p/google-breakpad/\">google-breakpad</a> (BSD)</li>");
#endif //defined (USE_BREAKPAD)
    html.append ("<li><a href=\"http://www.openscenegraph.org\">OpenSceneGraph</a> (OpenSceneGraph Public License based on the LGPL)</li>");
    html.append ("<li><a href=\"http://qt.nokia.com\">Qt</a> (LGPL)</li>");
    html.append ("<li><a href=\"http://qwt.sourceforge.net\">Qwt</a>: Qt Widgets for Technical Applications (LGPL)</li>");
    html.append ("</ul>");

    if(media_cycle){
        std::vector<ACPluginLibrary *> pluginLibraries = media_cycle->getPluginManager()->getPluginLibrary();
        if(pluginLibraries.size()>1){ // discarding default plugins
            html.append ("Plugins dependencies (additionnally to the above):");
            html.append ("<ul>");
            for(std::vector<ACPluginLibrary *>::iterator pluginLibrary = pluginLibraries.begin(); pluginLibrary != pluginLibraries.end();++pluginLibrary){
                QString plugin_name( (*pluginLibrary)->getName().c_str() );
                plugin_name = plugin_name.remove("mc_");
                std::vector<ACThirdPartyMetadata> tpmds = (*pluginLibrary)->getThirdPartyMetadata();
                if(tpmds.size()>0){

                    html.append ("<li>" + plugin_name + ":</li>");
                    html.append ("<ul>");
                    for(std::vector<ACThirdPartyMetadata>::iterator tpmd = tpmds.begin(); tpmd != tpmds.end();++tpmd){
                        html.append ("<li><a href=\"" + QString( (*tpmd).url.c_str() ) + "\">" + QString( (*tpmd).name.c_str() ) + "</a> (" + QString( (*tpmd).license.c_str() ) + ")</li>");
                    }
                    html.append ("</ul>");
                }
                else{
                    html.append ("<li>" + plugin_name + "</li>");
                }
            }
            html.append ("</ul>");
        }
        else
            html.append ("Plugins not yet loaded.");
    }
    ui.multiLineEditLicenses->document()->setHtml(html);
}

void ACAboutDialogQt::updateTeam(){
    QString html;
    html.append ("The current core team features (alphabetical order):<ul>");
    html.append ("<li>St&eacute;phane Dupont</li>");
    html.append ("<li>Christian Frisson</li>");
    html.append ("<li>Alexis Moinet</li>");
    html.append ("<li>Thierry Ravet</li>");
    html.append ("<li>Xavier Siebert</li>");
    html.append ("</ul><br/>Among past contributors:<ul>");
    html.append ("<li>Julien Biral</li>");
    html.append ("<li>Julien Dubois</li>");
    html.append ("<li>Thomas Dubuisson</li>");
    html.append ("<li>Sidi Ahmed Mahmoudi</li>");
    html.append ("<li>C&eacute;cile Picard-Limpens</li>");
    html.append ("<li>Fran&ccedil;ois Rocca</li>");
    html.append ("<li>Rapha&euml;l Sebbe</li>");
    html.append ("<li>Damien Tardieu</li>");
    html.append ("<li>J&eacute;r&ocirc;me Urbain</li>");
    html.append ("</ul>");
    html.append ("</ul><br/>Our dedicated users (alphabetical order):<ul>");
    html.append ("<li>Bud Blumenthal</li>");
    html.append ("<li>Bernard Delcourt</li>");
    html.append ("<li>Rudi Giot</li>");
    html.append ("<li>Gauthier Keyaerts</li>");
    html.append ("<li>Matthieu Michaux</li>");
    html.append ("<li>Jean-Louis Poliart</li>");
    html.append ("<li>Laszlo Umbreit</li>");
    html.append ("</ul>");
    ui.multiLineEditTeam->document()->setHtml(html);
}
