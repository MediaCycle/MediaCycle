/*
 *  ACMultiMediaCycleSeneffe.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 17/05/11
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

#include "ACMultiMediaCycleSeneffe.h"

ACMultiMediaCycleSeneffe::ACMultiMediaCycleSeneffe() : ACMultiMediaCycleOsgQt() {
	count = 0;
	// delay after which we change media_files (if it's ok)
	attente = 1*60*1000; // in ms, so 1 800 000 is for 30 min

	this->useSegmentationByDefault(false);

	//XMLfiles.push_back("/usr/local/share/mediacycle/data/audio_all/zero-g-pro-pack_a/Brass Elements/Brass_076_BPM.xml");
	//XMLfiles.push_back("/usr/local/share/mediacycle/data/audio_all/zero-g-pro-pack_a/Brass Elements/Brass_096_BPM.xml");
	//XMLfiles.push_back("/usr/local/share/mediacycle/data/audio_all/zero-g-pro-pack_a/Brass Elements/Brass_100_BPM.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/AdamKeshen44.xml");
	XMLfiles.push_back("/numediart/datasets/2011-05-MusicTechMagazine98-new.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/AdamKeshen44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/AkitoVanTroyer44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/AndresCabrera44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/AndrewSorkin44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/AnthonyKozar44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/BehindTheGreenDoor-Cecilia-44k.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44Barkovich.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44BoulangerFX.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44Forsyth.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v1.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v2.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v3.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v4.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v5.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v6.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v7.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v8.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v9.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v10.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v11.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v12.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v13.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v14.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v15.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Berklee44v16.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Boulanger44CsoundBookSelected.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Boulanger44CsoundSystemSounds.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/BoulangerAruba44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/BoulangerMarion44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/BoulangerPhilip44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/BT44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/ChenSokolovsky44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/ColetteBoulanger44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/ColmanOReilly44.xml");
	XMLfiles.push_back("/numediart/datasets/olpc-sound-samples-v2-xml/Conversations-SlawomirZubrzycki-Piano-44k.xml");

	timer = new QTimer(this);
	connect( timer, SIGNAL(timeout()), this, SLOT(loopXML()) );
}

ACMultiMediaCycleSeneffe::~ACMultiMediaCycleSeneffe(){
	delete timer;
}

void ACMultiMediaCycleSeneffe::startLoopXML(){
	loopXML(); // start first one before time starts so you don't wait for 30 min for the app to run
	timer->start(attente);
}

void ACMultiMediaCycleSeneffe::loopXML(){
	cout << "hello seneffe : " << count << endl;
	// going through all files again
	if (count >= XMLfiles.size()) count=0;
	cout << "opening : " << XMLfiles[count] << endl;
	this->clean(true);
	this->readXMLConfig(XMLfiles[count]);
	count++;
}
