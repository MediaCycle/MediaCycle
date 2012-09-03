/*
 *  audioengine-tests.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 03/09/2012
 *  Adapted from commomACTests by Xavier Siebert
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

#include <ACAudioEngine.h>
#include <ACAudioFeedback.h>

#include <iostream>
#include <string>
using namespace std;


bool get_backend_info(ACAudioEngine* _audio_engine){
    bool ok = false;
#ifdef USE_OPENAL
    std::cout << "audioengine-tests:get_backend_info: OpenAL version: " << alGetString(AL_VERSION) << std::endl;
    std::cout << "audioengine-tests:get_backend_info: OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
    std::cout << "audioengine-tests:get_backend_info: OpenAL vendor: " << alGetString(AL_VENDOR) << std::endl;
    std::cout << "audioengine-tests:get_backend_info: OpenAL extensions: '" << alGetString(AL_EXTENSIONS) << "'" << std::endl;
    ok = true;
#else
    std::cerr << "audioengine-tests:get_backend_info: test not implemented for the PortAudio backend" << std::endl;
#endif
    return ok;
}

bool test_speaker_configuration(ACAudioEngine* _audio_engine){
    bool ok = false;

#ifdef USE_OPENAL
    std::vector<std::string> _configs;
    _audio_engine->getFeedback()->getSpeakerConfigurationsList(_configs);
    for(std::vector<std::string>::iterator _config = _configs.begin();_config!=_configs.end();_config++)
        std::cout << "audioengine-tests:speaker_configuration: available speaker config " << (*_config) << std::endl;
    std::cout << "audioengine-tests:speaker_configuration: " << _configs.size() << " available speaker configs" << std::endl;
    return ( _configs.size() > 0 );
#else
    std::cerr << "audioengine-tests:speaker_configuration: multi speaker test not implemented for the PortAudio backend" << std::endl;
#endif
    return ok;
}

int main(int argc, char *argv[]){
    cout << "------------  Testing the Audio Engine  ------------------" << endl;

    cout << "audioengine-tests: backend: ";
#ifdef USE_OPENAL
    cout << "OpenAL";
#endif
#ifdef USE_PORTAUDIO
    cout << "PortAudio";
#endif
    cout << endl;

    cout << "audioengine-tests: creating an AudioEngine instance" << endl;
    ACAudioEngine* audio_engine = new ACAudioEngine();
    if(!audio_engine){
        std::cerr << "audioengine-tests: couldn't create an AudioEngine instance" << std::endl;
        return 1;
    }
    if(!audio_engine->getFeedback()){
        std::cerr << "audioengine-tests: AudioFeedback not properly created within the AudioEngine instance" << std::endl;
        return 1;
    }

    std::cout << "audioengine-tests:get_backend_info: starting" << std::endl;
    bool backend_info = get_backend_info(audio_engine);
    std::cout << "audioengine-tests:get_backend_infon: passed: " << backend_info << std::endl;

    std::cout << "audioengine-tests:speaker_configuration: starting" << std::endl;
    bool speaker_configuration = test_speaker_configuration(audio_engine);
    std::cout << "audioengine-tests:speaker_configuration: passed: " << speaker_configuration << std::endl;

    cout << "audioengine-tests: deleting the AudioEngine instance" << endl;
    delete audio_engine;
    return 0;
}
