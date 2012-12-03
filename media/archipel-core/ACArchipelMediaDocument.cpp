/*
 *  ACArchipelMediaDocument.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 27/07/11
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

#include "ACArchipelMediaDocument.h"
#include "ACArchipelReader.h"

#include <stdio.h>
#include <stdlib.h>

ACArchipelMediaDocument::ACArchipelMediaDocument():ACMediaDocument(){
}

std::vector<std::string> ACArchipelMediaDocument::getActivableMediaKeys(){
    std::vector<std::string> keys;
    keys.push_back("Description (Text)");
    keys.push_back("Cover (Image)");
    keys.push_back("Tracks (Audio)");
    return keys;
}

int ACArchipelMediaDocument::import(std::string _filename, int _mid, ACPluginManager *acpl, bool _save_timed_feat){
    filename=_filename;
    if (_mid>=0) this->setId(_mid);
    string extension = boost::filesystem::extension(filename);
    string path=boost::filesystem::path(_filename).parent_path().string()+string("/");
    if (extension!=string(".xml"))
        return 0;
    else{
        //read the xml file. We begin with the Mediacycle xml style
        ACArchipelReader* xmlDoc=new ACArchipelReader(filename);
        if (xmlDoc->isArchipel()){

            label=xmlDoc->getArtist()+string("/")+xmlDoc->getAlbumName();

            int nbMedia=0;
            ACMediaType mediaType;
            ACMedia *media ;
            string mediaFileName;
            string mediaExtension;
            ACMediaType fileMediaType;
            string mediaKey;

            this->extractFeatures(acpl,_save_timed_feat);

            //archipelText
            mediaKey="Description (Text)";

            mediaType=MEDIA_TYPE_TEXT;
            mediaFileName=filename;
            mediaExtension = boost::filesystem::extension(mediaFileName);
            fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
            if (!(fileMediaType&mediaType)){
                cout << "ACArchipelMediaDocument::import other media type, skipping "  << endl;
                return 0;
            }
            media = ACMediaFactory::getInstance().create(mediaExtension,mediaType);
            if (media->import(mediaFileName, this->getId()+nbMedia+1, acpl, _save_timed_feat)){
                if (this->addMedia(mediaKey, media)){
                    //this->incrementMediaID();
                    //if(mediaKey=="main")
                    //	this->activeMedia = media; //CF text not as active media
                }
            }
            else
                return 0;
            nbMedia++;

            //Image
            mediaKey="Cover (Image)";
            mediaType=MEDIA_TYPE_IMAGE;
            mediaFileName=path+xmlDoc->getThumbPath();
            mediaExtension = boost::filesystem::extension(mediaFileName);
            fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
            if (fileMediaType!=mediaType){
                cout << "ACArchipelMediaDocument::import other media type, skipping "  << endl;
                return 0;

            }
            media = ACMediaFactory::getInstance().create(mediaExtension);
            if (media->import(mediaFileName, this->getId()+nbMedia+1, acpl, _save_timed_feat)){
                if (this->addMedia(mediaKey, media)){
                    //this->incrementMediaID();
                    //if(mediaKey=="main") // image as active media
                    this->setActiveSubMedia(mediaKey);
                }

                //CF hack to add the document features to the initial active submedia:
                std::cout << "ACArchipelMediaDocument::import: document features: " << this->features_vectors.size() << std::endl;
                if(ACMedia::getFeaturesVector("Year")){
                    //media->addFeaturesVector(ACMedia::getFeaturesVector("Year"));
                    media->addPreProcFeaturesVector(ACMedia::getFeaturesVector("Year"));
                }
                if(ACMedia::getFeaturesVector("First Atoll")){
                    //media->addFeaturesVector(ACMedia::getFeaturesVector("First Atoll"));
                    media->addPreProcFeaturesVector(ACMedia::getFeaturesVector("First Atoll"));
                }
            }
            else
                return 0;
            nbMedia++;

            //tracks
            vector<string> tracksPaths=xmlDoc->getTrackPath();
            unsigned int nbTracks=tracksPaths.size();
            for (int i=0;i<nbTracks;i++){
                std::stringstream out;
                out << "Track "<<i << " (Audio)";
                mediaKey=out.str() ;
                mediaType=MEDIA_TYPE_AUDIO;
                mediaFileName=path+tracksPaths[i];
                //mediaFileName=mediaFileName.substr(0, mediaFileName.size()-4)+string(".wav");
                mediaExtension = boost::filesystem::extension(mediaFileName);
                fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);

                if (fileMediaType!=mediaType){
                    cout << "ACArchipelMediaDocument::impossible to import:"<< mediaFileName << endl;
                    continue;
                }
                media = ACMediaFactory::getInstance().create(mediaExtension);
                //if (media->import(mediaFileName, this->getId()+nbMedia+1, acpl, true)){
                //CF until large audio files can be imported, the above line is substituted by:
                int _aid = this->getId()+nbMedia+1;
                std::cout << "importing..." << mediaFileName << std::endl;
                media->setFileName(mediaFileName);
                media->setThumbnailFileName(mediaFileName);
                int import_ok = 1;
                if (_aid>=0) media->setId(_aid);

                // get info about width, height, mediaData
                // computes thumbnail, ...
                // mediaData will be used by the plugin to compute features

                media->setStart(0.0f);// CF dirty hack until large audio files can be imported
                media->setEnd(1.0f);// CF idem
                /*if (!media->extractData(media->getFileName())) return 0;
                if (media->getMediaData()==0){
                    import_ok = 0;
                    cerr << "<ACMedia::import> failed accessing data for media number: " << _mid << endl;
                    return 0;
                }*/

                if(import_ok){//media import substitution ends here
                    //this->addSegment(media);//TODO TR why did we add audio track media as segment AND submedia?
                    if (this->addMedia(mediaKey, media)){
                        //this->incrementMediaID();
                        if(mediaKey=="main")
                            this->setActiveSubMedia(mediaKey);
                    }
                }
                //				else
                //					return 0;
                nbMedia++;
            }

            /*
            for (unsigned int i=0;i<nbMedia; i++){
                string s_media_type=xmlDoc->getMediaType(i);
                ACMediaType mediaType;
                stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(s_media_type);

                if( iterm == stringToMediaType.end() ) {
                    cout << "ACArchipelMediaDocument::import media type not found : " << s_media_type << endl;
                    continue;
                }
                else {
                    mediaType = iterm->second;
                    //		cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;
                }
                string mediaFileName=xmlDoc->getMediaPath(i);

                string mediaExtension = boost::filesystem::extension(mediaFileName);
                ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(mediaExtension);
                if (fileMediaType!=mediaType){
                    cout << "ACMediaDocument::import other media type, skipping " << s_media_type << endl;
                    continue;
                }
                string mediaKey=xmlDoc->getMediaReference(i);
                ACMedia *media = ACMediaFactory::getInstance().create(mediaExtension);
                if (media->import(path+mediaFileName, this->getId()+nbMedia+1, acpl, _save_timed_feat)){
                    if (this->addMedia(mediaKey, media)){
                        //this->incrementMediaID();
                        if(mediaKey=="main")
                            this->activeMedia = media;
                    }
                }
                else
                    delete media;
            }*/
            delete xmlDoc;
            return nbMedia;
        }
        else {
            return 0;
        }
    }
}

ACMediaFeatures* ACArchipelMediaDocument::getPreProcFeaturesVector(string feature_name) {
    int i;
    for (i=0;i<int(preproc_features_vectors.size());i++) {
        if (!(feature_name.compare(preproc_features_vectors[i]->getName()))) {
            return preproc_features_vectors[i];
        }
    }
#ifdef USE_DEBUG // use debug message levels instead
    std::cerr << "ACMedia::getPreProcFeaturesVector : not found feature named " << feature_name << std::endl;
#endif
    return 0;
}

