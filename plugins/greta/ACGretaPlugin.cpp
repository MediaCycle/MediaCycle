/*
 *
 */

#include "ACGretaPlugin.h"

using namespace std;

CppPoster::CppPoster(JString name, JString host, int port) : CppAIRPlug(name, host, port) {
}

CppPoster::~CppPoster() {
}

ACGretaPlugin::ACGretaPlugin() {
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_CLIENT;
    this->mName = "Greta";
    this->mDescription = "Greta plugin - sends request to Greta through Psyclone";
    this->mId = "";

    //calculate("example\\thierry");
}

ACGretaPlugin::~ACGretaPlugin() {

}

int ACGretaPlugin::initialize() {
    std::cout << "ACGretaPlugin initialized" << std::endl;
    return 0;
}

ACMediaFeatures* ACGretaPlugin::calculate() {
    return NULL;
}

ACMediaFeatures* ACGretaPlugin::calculate(std::string aFileName) {
    cout << "Greta : " << aFileName << endl;

    int pos = aFileName.find_last_of(".");
    std::string basename = aFileName.substr(0,pos);
    //std::string extension = aFileName.substr(pos+1,aFileName.size()-1);

    JString host = "192.168.3.191";
    int port = 10000;
    CppPoster* tv = new CppPoster("CppPoster", host, port);

    if (!tv->init()) {
        printf("Could not connect to Psyclone on '%s' port %d, exiting...\n\n", (char*) host, port);
        return 0;
    }

    if (tv->isServerAvailable())
        tv->start();
    else {
        printf("Could not connect to Psyclone on '%s' port %d, exiting...\n\n", (char*) host, port);
        delete(tv);
        exit(0);
    }

    JTime now;
    int count = 0;
    JString content = ACGretaPlugin::generateBML(basename).c_str();
    cout << "Greta - sending : " << content.c_str()<<endl;
    while (true) {
        count++;
        if (tv->isServerAvailable()) {
            if (!tv->postMessage("Greta.Whiteboard", "Greta.Data.BML", content, "", ""))
                printf("Could not post message to Psyclone on '%s' port %d...\n\n", (char*) host, port);
            else {
                now = JTime();
                printf("%s: CppPoster posted message of type Greta.Data.BML to Greta.Whiteboard ...\n", (char*) now.printTimeMS());
                break;
            }
        } else {
            //	printf("Connection to Psyclone on '%s' port %d lost, exiting...\n\n", (char*) host, port);
            //	return 0;
        }
    }

    cout << "Greta : " << count << endl;

    return NULL;
}

std::string ACGretaPlugin::generateBML(std::string basename) {
    string bml;

    bml = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    bml += "<!DOCTYPE bml SYSTEM \"bml.dtd\">";
    bml += "<bml>";
    bml += "<speech id=\"s1\" start=\"0\" language=\"english\" voice=\"realspeech\" text=\"\">";
    bml += "<description level=\"1\" type=\"gretabml\">";
    bml += "<reference>" + basename + ".wav</reference>";
    bml += "</description>";
    bml += "</speech>";

    bml += "<face id=\"emotion-1\" start=\"0\">";
    bml += "<description level=\"1\" type=\"gretabml\">";
    bml += "<reference>faceexp=fapfile:" + basename + ".fap</reference>";
    bml += "</description>";
    bml += "</face>";
    bml += "</bml>";
    
    return bml;
}
