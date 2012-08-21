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
    this->updateLicenses();
    this->updateTeam();
    ui.appName->setText(QString("MediaCycle ") + QString(STRINGIZE_VALUE_OF(MC_VERSION)));
}

void ACAboutDialogQt::updateLicenses(){
    QString html;
    html.append ("The license of the MediaCycle framework is not yet determined. We should opt for a dual commercial/community license soon.<br/><br/>This current release uses the following dependencies:<br/><ul>");
#if defined (USE_BREAKPAD)
	html.append ("<li><a href=\"https://github.com/AlekSi/breakpad-qt\">AlekSi's breakpad-qt</a>: BSD License</li>");
#endif //defined (USE_BREAKPAD)	
	html.append ("<li><a href=\"http://arma.sourceforge.net\">armadillo</a>: LGPL (or GPL) License</li>");
#ifndef __APPLE__ // part of the Accelerate framework
    html.append ("<li><a href=\"http://www.netlib.org/blas/\">Blas</a>: just requires attribution</li>");
#endif//ndef __APPLE__
    html.append ("<li><a href=\"http://www.boost.org\">Boost</a>: Boost Software License (attribution)</li>");
#if defined (SUPPORT_TEXT)
    html.append ("<li><a href=\"http://sourceforge.net/projects/clucene/\">clucene</a>: LGPL License</li>");
#endif //defined (SUPPORT_TEXT)
#if defined (SUPPORT_VIDEO) || defined (SUPPORT_IMAGE)
    html.append ("<li><a href=\"http://www.ffmpeg.org\">ffmpeg</a>: LGPL (without GPL and non free codecs)</li>");
#endif //defined (SUPPORT_VIDEO) || defined (SUPPORT_IMAGE)
    html.append ("<li><a href=\"http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html\">fftooura</a>: &quot;You may use, copy, modify and distribute this code for any purpose (include commercial use) and without fee. Please refer to this package when you modify this code.&quot;</li>");
#if defined (SUPPORT_VIDEO) || defined (SUPPORT_IMAGE)
    html.append ("<li><a href=\"http://www.fftw.org\">fftw3</a>: GPL License</li>");
#endif //defined (SUPPORT_VIDEO) || defined (SUPPORT_IMAGE)
    html.append ("<li><a href=\"http://www.aei.mpg.de/~peekas/tree/\">Kasper Peeters' STL-like templated tree class</a>: GPL License</li>");
#ifndef __APPLE__ // part of the Accelerate framework
    html.append ("<li><a href=\"http://www.netlib.org/lapack/\">LAPACK</a>: BSD License</li>");
#endif//ndef __APPLE__
#if defined (USE_OSC)
    html.append ("<li><a href=\"http://liblo.sourceforge.net\">liblo</a>: LPGL License</li>");
#endif //defined (USE_OSC)
#if defined (SUPPORT_AUDIO)
	#if defined (USE_AUDIOFEAT)
    html.append ("<li><a href=\"http://www.mega-nerd.com/SRC/\">libsamplerate</a>: GPL License</li>");
	#endif //defined (USE_AUDIOFEAT)
    html.append ("<li><a href=\"http://www.mega-nerd.com/libsndfile/\">libsndfile</a>: LGPL (or GPL) License</li>");
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_AUDIO) && defined (USE_OPENAL)
    html.append ("<li><a href=\"http://kcat.strangesoft.net/openal.html\">OpenAL-soft</a>: LPGL License</li>");
#endif //defined (SUPPORT_AUDIO) || defined (USE_OPENAL)
#if defined (USE_OCTAVE)
    html.append ("<li><a href=\"http://www.octave.org\">Octave</a>: GPL License</li>");
#endif //defined (USE_OCTAVE)
#if defined (SUPPORT_VIDEO) && defined (SUPPORT_IMAGE)
    html.append ("<li><a href=\"http://opencv.willowgarage.com\">OpenCV</a>: BSD License</li>");
#endif //defined (SUPPORT_VIDEO) || defined (SUPPORT_IMAGE)
    html.append ("<li><a href=\"http://www.openscenegraph.org\">OpenSceneGraph</a>: OpenSceneGraph Public License based on the LGPL License</li>");
#if defined (SUPPORT_PDF)
    html.append ("<li><a href=\"http://podofo.sourceforge.net\">PoDoFo</a>: LPGL License</li>");
#endif //defined (SUPPORT_PDF)
#if defined (SUPPORT_AUDIO) && defined (USE_PORTAUDIO)
    html.append ("<li><a href=\"http://www.portaudio.com\">PortAudio</a>: MIT License</li>");
#endif //defined (SUPPORT_AUDIO) || defined (USE_PORTAUDIO)
    html.append ("<li><a href=\"http://qt.nokia.com\">Qt</a>: LGPL License</li>");
    html.append ("<li><a href=\"http://qwt.sourceforge.net\">Qwt</a> - Qt Widgets for Technical Applications: LGPL License</li>");
#if defined (SUPPORT_AUDIO)
    html.append ("<li><a href=\"http://sp-tk.sourceforge.net\">The Speech Signal Processing Toolkit (SPTK)</a>: BSD License</li>");
#endif //defined (SUPPORT_AUDIO)
    html.append ("<li><a href=\"http://sourceforge.net/projects/tinyxml\">TinyXML</a>: zlib/libpng License</li>");
#if defined (USE_TORCH3)
    html.append ("<li><a href=\"http://www.torch.ch\">Torch3</a>: BSD License</li>");
#endif //defined (USE_TORCH3)
#if defined (USE_VAMP)
    html.append ("<li><a href=\"http://vamp-plugins.org\">Vamp plugin SDK</a>: BSD License</li>");
#endif //defined (USE_VAMP)	
#if defined (SUPPORT_AUDIO) && defined (USE_YAAFE)
    html.append ("<li><a href=\"http://yaafe.sourceforge.net\">Yet another audio features extraction (Yaafe)</a>: LPGL License</li>");
#endif //defined (SUPPORT_AUDIO) && defined (USE_YAAFE)
    html.append ("</ul>");
#if defined (USE_OCTAVE)
    html.append ("<br/><br/>This current release packages as well the following Octave/Matlab libraries, used through an Octave wrapper:<br/><ul>");
#if defined (USE_MAKAM)
    html.append ("<li><a href=\"ftp://ftp.iyte.edu.tr/share/ktm-nota/TuningMeasurement.html\">Makam Toolbox</a> from Bari&#351; Bozkurt et al, ported to Octave by Onur Babacan</li>");
    html.append ("<li><a href=\"http://cognition.ens.fr/audition/adc/sw/yin.zip\">YIN</a> mex libraries from Alain de Cheveign&eacute; et al: copyright for research purposes only)</li>");
#endif //defined (USE_MAKAM)
    html.append ("</ul>");
#endif //defined (USE_OCTAVE)
    ui.multiLineEditLicenses->document()->setHtml(html);
}

void ACAboutDialogQt::updateTeam(){
    QString html;
    html.append ("The current core team features (alphabetical order):<ul>");
    html.append ("<li>St&eacute;phane Dupont</li>");
    html.append ("<li>Christian Frisson</li>");
    html.append ("<li>Alexis Moinet</li>");
    html.append ("<li>C&eacute;cile Picard-Limpens</li>");
    html.append ("<li>Thierry Ravet</li>");
    html.append ("<li>Xavier Siebert</li>");
    html.append ("</ul><br/>Among past contributors:<ul>");
    html.append ("<li>Julien Dubois</li>");
    html.append ("<li>Thomas Dubuisson</li>");
    html.append ("<li>Sidi Ahmed Mahmoudi</li>");
    html.append ("<li>Rapha&euml;l Sebbe</li>");
    html.append ("<li>Damien Tardieu</li>");
    html.append ("<li>J&eacute;r&ocirc;me Urbain</li>");
    html.append ("</ul>");
    ui.multiLineEditTeam->document()->setHtml(html);
}
