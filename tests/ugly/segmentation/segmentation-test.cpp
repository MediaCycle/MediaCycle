/**
 * @brief segmentation-test.cpp
 * @author Christian Frisson
 * @date 11/04/2013
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

#include <string>
#include <iostream>
#include <fstream>
#include "stdlib.h"

#include "MediaCycle.h"

#include "ACBicSegmentationPlugin.h"
#include "ACSelfSimSegmentation.h"
#include "gnuplot_i.hpp"
#include "Armadillo-utils.h"
#ifdef SUPPORT_AUDIO
#include "ACAudioFeatures.h"
#include "ACAudioSelfSimSegmentationPlugin.h"
#endif


//sleep()
#include <iostream>
#ifdef __MINGW32__
#include <windows.h>
#define sleep(n) Sleep(1000 * n)
#endif

//const string videodir = "/Users/xavier/numediart/Project7.3-DancersCycle/VideosSmall/TestSmallSize/";
const string audiodir = "/Users/ccl/data/Projects/numediart/MoDyVa/DB/Wav_MoDyVA_CAT/Events_adjust/selection/";//ccl

void test_single_bic(std::string _dir, std::string _fname) {
    arma::fmat M;
    M.set_size(1, 100);
    for (int i = 0; i < 30; i++) {
        M(0, i) = 20 + double(rand()) / RAND_MAX;
    }
    for (int i = 30; i < 80; i++) {
        M(0, i) = 10 + double(rand()) / RAND_MAX;
    }
    for (int i = 80; i < 100; i++) {
        M(0, i) = 5 + double(rand()) / RAND_MAX;
    }
    M.print();

    //default: lambda(1), sampling_rate(1), Wmin(20), bic_thresh(0.5), jump_width(5)
    /*ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();
    clock_t start = clock();
    std::vector<int> seg = P->testSegment(M, 1, 1, 20, 1, 5);
    std::cout << "Time elapsed (segmentation alone): " << ((double) clock() - start) / CLOCKS_PER_SEC << std::endl;

    //plotting in gnuplot
    vector<double> m;
    for (unsigned int i = 0; i < M.n_cols; i++) {
        m.push_back((double) M(0, i));
    }

    Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();
    g1.plot_x(m, "M");

    std::vector<double> seg_d(seg.begin(), seg.end());
    std::vector<double> seg_i; // segments heights
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(100);
    }

    // plotting segments
    g1.set_style("impulses");
    if (seg_d.size() > 0)
        g1.plot_xy(seg_d, seg_i, "segments");

    // for output in postscript file:
    // uncomment the following lines for output in terminal
    g1.cmd("set terminal postscript eps");
    string fout = "set output \"" + _dir + _fname + "\"";
    //string fout = "set output " + _fname + "";
    g1.cmd(fout.c_str());
    g1.cmd("replot");
    //


    sleep(1);

    cout << "Enter char to exit" << endl;
    char c;
    cin >> c;

    delete P;*/
}

void test_double_bic(std::string _dir, std::string _fname) {
    arma::fmat M;
    M.set_size(2, 100);
    for (int i = 0; i < 20; i++) {
        M(0, i) = 20 + double(rand()) / RAND_MAX;
    }
    for (int i = 20; i < 80; i++) {
        M(0, i) = 16 + double(rand()) / RAND_MAX;
    }
    for (int i = 80; i < 100; i++) {
        M(0, i) = 10 + double(rand()) / RAND_MAX;
    }

    for (int i = 0; i < 30; i++) {
        M(1, i) = 19 + double(rand()) / RAND_MAX;
    }
    for (int i = 30; i < 70; i++) {
        M(1, i) = 15 + double(rand()) / RAND_MAX;
    }
    for (int i = 70; i < 100; i++) {
        M(1, i) = 9 + double(rand()) / RAND_MAX;
    }

    M.print();

    /*ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();
    clock_t start = clock();
    //default: lambda(1), sampling_rate(1), Wmin(20), bic_thresh(0.5), jump_width(5)

    std::vector<int> seg = P->testSegment(M, 1, 1, 15, 2, 5);

    std::cout << " -- end double bic segmentation --" << std::endl;
    std::cout << "Time elapsed (segmentation alone): " << ((double) clock() - start) / CLOCKS_PER_SEC << std::endl;

    //plotting in gnuplot
    vector<double> m0;
    vector<double> m1;

    for (unsigned int i = 0; i < M.n_cols; i++) {
        m0.push_back((double) M(0, i));
        m1.push_back((double) M(1, i));
    }

    std::vector<double> seg_d(seg.begin(), seg.end());
    std::vector<double> seg_i; // segments heights
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(20);
    }

    Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();

    g1.plot_x(m0, "M0");
    g1.plot_x(m1, "M1");

    // plotting segments
    g1.set_style("impulses");
    if (seg_d.size() > 0)
        g1.plot_xy(seg_d, seg_i, "segments");

    // for output in postscript file:
    // uncomment the following lines for output in terminal
    g1.cmd("set terminal postscript eps");
    string fout = "set output \"" + _dir + _fname + "\"";
    g1.cmd(fout.c_str());
    g1.cmd("replot");
    //

    cout << "Enter char to exit" << endl;
    char c;
    cin >> c;

    delete P;*/
}

void test_multiple_bic(int n) {
    arma::fmat M;
    M.set_size(n, 100);
    float tmp;

    srand(time(NULL));


    for (int k = 0; k < n; k++) {
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 0; i < 30; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 30; i < 80; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 80; i < 100; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
    }


    M.print();

    /*ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();
    clock_t start = clock();
    //default: lambda(1), sampling_rate(1), Wmin(20), bic_thresh(0.5), jump_width(5), discard_borders(5)

    //std::vector<int> seg = P->segment(M, 1, 5, 15, 0, 0);
    //default: lambda=1, Wmin=20, bic_thresh(-Inf), discard_borders(0), bic_thresh_DAC(0)
    std::vector<int> seg = P->testSegmentDAC(M, 1, 15, -1.0 / 0.0, 0, 0);

    std::cout << " -- end multiple bic segmentation --" << std::endl;
    std::cout << "Time elapsed (segmentation alone): " << ((double) clock() - start) / CLOCKS_PER_SEC << std::endl;

    //plotting in gnuplot


    std::vector<double> seg_d(seg.begin(), seg.end());
    std::vector<double> seg_i; // segments heights
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(100);
    }

    Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();

    vector<double> m;
    //       char buffer[3];
    for (int p = 0; p < n; p++) {
        m.clear();
        for (unsigned int i = 0; i < M.n_cols; i++) {
            m.push_back((double) M(p, i));
        }

        g1.plot_x(m, "M");
    }

    // plotting segments
    g1.set_style("impulses");
    if (seg_d.size() > 0)
        g1.plot_xy(seg_d, seg_i, "segments");

    cout << "Enter char to exit" << endl;
    char c;
    cin >> c;

    delete P;*/
}

void test_multiple_selfsim(int n) {
    arma::fmat M;
    M.set_size(n, 100);
    float tmp;

    srand(time(NULL));


    for (int k = 0; k < n; k++) {
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 0; i < 30; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 30; i < 50; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 50; i < 80; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
        tmp = 100 * double(rand()) / RAND_MAX;
        for (int i = 80; i < 100; i++) {
            M(k, i) = tmp + 5 * double(rand()) / RAND_MAX;
        }
    }


    M.print();
#ifdef SUPPORT_AUDIO
    ACAudioSelfSimSegmentationPlugin* P = new ACAudioSelfSimSegmentationPlugin();
    clock_t start = clock();
    //default: float _SelfSimThresh=0.8, _L=8, _Wmin=8, KernelType=SELFSIMSTEP, KernelDistance=COSINE;

    std::vector<int> seg = P->testSegment(M, 0.01, 16, 10, SELFSIMGAUSSIAN, MANHATTAN);

    std::cout << " -- end multiple self sim segmentation --" << std::endl;
    std::cout << "Time elapsed (segmentation alone): " << ((double) clock() - start) / CLOCKS_PER_SEC << std::endl;

    //plotting in gnuplot


    std::vector<double> seg_d(seg.begin(), seg.end());
    std::vector<double> seg_i; // segments heights
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(100);
    }

    Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();

    vector<double> m;
    //       char buffer[3];
    for (int p = 0; p < n; p++) {
        m.clear();
        for (unsigned int i = 0; i < M.n_cols; i++) {
            m.push_back((double) M(p, i));
        }

        g1.plot_x(m, "M");
    }

    // plotting segments
    g1.set_style("impulses");
    if (seg_d.size() > 0)
        g1.plot_xy(seg_d, seg_i, "segments");

    cout << "Enter char to exit" << endl;
    char c;
    cin >> c;

    delete P;
#endif
}

#ifdef SUPPORT_AUDIO
void test_segmentation_from_laughter_file(std::string _dir, std::string _fname) { //ccl
    SF_INFO sfinfo;
    SNDFILE* testFile;
    float* data;

    int bflag, ch, fd;
    int mfccNbChannels = 32;
    int mfccNb = 13;
    int windowSize = 512;
    bool extendSoundLimits = false;
    std::string fileInput = _dir+_fname+".wav";
    std::string fileOut;

    bflag = 0;


    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Sound file : " << fileInput << std::endl;
    std::cout << "Window size : " << windowSize << std::endl;
    std::cout << "Number of MFCC channels : " << mfccNbChannels << std::endl;
    std::cout << "Number of MFCC : " << mfccNb << std::endl;

    if (!(testFile = sf_open(fileInput.c_str(), SFM_READ, &sfinfo))) {
        /* Open failed so print an error message. */
        printf("Not able to open input file %s\n", fileInput.c_str());
        /* Print the error message from libsndfile. */
        puts(sf_strerror(NULL));
        return;
    }

    std::vector<ACMediaTimedFeature*> desc;
    ACMediaTimedFeature* descM; //ccl
    std::cout << "Length : " << sfinfo.frames << std::endl;
    std::cout << "Sampling Rate : " << sfinfo.samplerate << std::endl;
    std::cout << "Channels : " << sfinfo.channels << std::endl;
    data = new float[(long) sfinfo.frames * sfinfo.channels];
    std::cout << "Read " << sf_read_float(testFile, data, sfinfo.frames * sfinfo.channels) << " frames" << std::endl;
    desc = computeFeatures(data, sfinfo.samplerate, sfinfo.channels, sfinfo.frames, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);
    descM = computeFeature(data, _fname+"_features", sfinfo.samplerate, sfinfo.channels, sfinfo.frames, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);//ccl

    //segmentation
    /*ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();
    clock_t start = clock();
    //default: lambda(1), sampling_rate(1), Wmin(20), bic_thresh(0.5), jump_width(5)

    //std::vector<int> seg = P->segment(desc, 1, 5, 15, -1000, 0);
    //std::vector<int> seg = P->segmentDAC(desc, 1, 15, -1.0/0.0, 0, 0);*/

    ACSelfSimSegmentationPlugin* P = new ACSelfSimSegmentationPlugin();
    clock_t start = clock();
    //default: float _SelfSimThresh=0.8, _L=8, _Wmin=8, KernelType=SELFSIMSTEP, KernelDistance=COSINE;
    //std::vector<int> seg = P->testSegment(desc, 0.01, 32, 25, SELFSIMGAUSSIAN, COSINE);
    //std::vector<int> seg = P->testSegment(desc, 0.01, 32, 240, SELFSIMGAUSSIAN, COSINE);//200 ccl
    fileOut = "SelfSim"; //ccl

    std::cout << " -- end multiple self sim segmentation --" << std::endl;
    std::cout << "Time elapsed (segmentation alone): " << ((double) clock() - start) / CLOCKS_PER_SEC << std::endl;

    //plotting in gnuplot
    arma::fmat M = P->get_features();
    int n = M.n_rows;
    arma::fcolvec t = desc[0]->getTime();
    std::vector<double> temps;
    for (int i = 0; i < t.n_elem; i++) {
        temps.push_back(t(i));
    }
    std::vector<double> seg_d;

   // for (int i = 0; i < seg.size(); i++) {
     //   seg_d.push_back(t[seg[i]]);
    //}
    std::vector<double> seg_i; // segments heights
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(n + 5);
    }



    Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();

    vector<double> m;
    //char buffer[3];
    arma::fcolvec maxval = arma::max(arma::abs(M), 1);
    for (int p = 0; p < n; p++) {
        m.clear();
        //maxval=arma::max(arma::abs(M.row(p)));
        for (unsigned int i = 0; i < M.n_cols; i++) {
            m.push_back(p + (double) M(p, i) / maxval(p));
        }
        //        g1.plot_xy(temps, m, "M");

    }

    // plotting segments
    g1.set_style("impulses");
    if (seg_d.size() > 0) {
        g1.plot_xy(seg_d, seg_i, "segments");
        // for output in postscript file:
        // uncomment the following lines for output in terminal
        g1.cmd("set terminal postscript eps");
        string fout = "set output \"" + _dir + _fname + fileOut + ".eps\"";
        g1.cmd(fout.c_str());
        g1.cmd("replot");
    }
    

    cout << "Enter char to exit" << endl;
    char c;
    cin >> c;

    delete P;
}
#endif

#ifdef SUPPORT_AUDIO
void test_ALLsegmentation_from_Audiofile(std::string _dir, std::string _fname) { //ccl

    SF_INFO sfinfo;
    SNDFILE* testFile;
    float* data;

    int bflag, ch, fd;
    int mfccNbChannels = 32;
    int mfccNb = 13;
    int windowSize = 1024;
    bool extendSoundLimits = false;
    std::string fileInput = _dir+_fname+".wav";
    std::string fileOut;

    bflag = 0;


    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Sound file : " << fileInput << std::endl;
    std::cout << "Window size : " << windowSize << std::endl;
    std::cout << "Number of MFCC channels : " << mfccNbChannels << std::endl;
    std::cout << "Number of MFCC : " << mfccNb << std::endl;

    if (!(testFile = sf_open(fileInput.c_str(), SFM_READ, &sfinfo))) {
        /* Open failed so print an error message. */
        printf("Not able to open input file %s\n", fileInput.c_str());
        /* Print the error message from libsndfile. */
        puts(sf_strerror(NULL));
        return;
    }

    std::vector<ACMediaTimedFeature*> desc;
    ACMediaTimedFeature* descM; //ccl
    std::cout << "Length : " << sfinfo.frames << std::endl;
    std::cout << "Sampling Rate : " << sfinfo.samplerate << std::endl;
    std::cout << "Channels : " << sfinfo.channels << std::endl;
    data = new float[(long) sfinfo.frames * sfinfo.channels];
    std::cout << "Read " << sf_read_float(testFile, data, sfinfo.frames * sfinfo.channels) << " frames" << std::endl;
    desc = computeFeatures(data, sfinfo.samplerate, sfinfo.channels, sfinfo.frames, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);


    //from main.cpp of mediacyle-cli
    MediaCycle *media_cycle;

    ACMediaType _media_type = MEDIA_TYPE_AUDIO;

    media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");

    string smedia = "none";
#if defined (SUPPORT_AUDIO)
    smedia="audio";
#endif //defined (SUPPORT_AUDIO)


    // -- media-specific features plugin + generic segmentation and visualisation plugins--
    std::string f_plugin, s_plugin, v_plugin;

    char c_path[2048];
    // use the function to get the path
    getcwd(c_path, 2048);
    std::string s_path = c_path;

    std::string build_type ("Release");
#ifdef USE_DEBUG
    build_type = "Debug";
#endif //USE_DEBUG

#if defined(__APPLE__)
#if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
    f_plugin = "@executable_path/../MacOS/mc_" + smedia +".dylib";
    v_plugin = "@executable_path/../MacOS/mc_visualisation.dylib";
    s_plugin = "@executable_path/../MacOS/mc_segmentation.dylib";
#else
    f_plugin = s_path + "/../../../plugins/"+ smedia + "/" + build_type + "/mc_" + smedia +".dylib";
    v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib";
    s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib";
#endif
    // common to all media, but only for mac...
#elif defined (__WIN32__)
    f_plugin = s_path + "\..\..\..\plugins\\" + smedia + "\mc_"+smedia+".dll";
    v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.dll";
    s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dll";
#else
#if not defined (USE_DEBUG) // needs "make package" to be ran to work
    f_plugin = "/usr/lib/mc_"+smedia+".so";
    v_plugin = "/usr/lib/mc_visualisation.so";
    s_plugin = "/usr/lib/mc_segmentation.so";
#else
    f_plugin = s_path + "/../../plugins/"+smedia+"/mc_"+smedia+".so";
    v_plugin = s_path + "/../../plugins/visualisation/mc_visualisation.so";
    s_plugin = s_path + "/../../plugins/segmentation/mc_segmentation.so";
#endif
#endif

    media_cycle->addPluginLibrary(f_plugin);
    //media_cycle->addPluginLibrary(v_plugin);
    media_cycle->addPluginLibrary(s_plugin);

    //cerr<<"features dim " << desc.getLength() << " x " << descM->getDim() << endl;

    ACMedia* ACM;
    ACPluginManager *acpl;
    ACM = ACMediaFactory::getInstance().create(MEDIA_TYPE_AUDIO);
    acpl = media_cycle->getPluginManager();
    int i_o = ACM->import(fileInput, 0, acpl, true);
    cerr << i_o << endl;
    descM = ACM->getTimedFeatures();

    // BICsegmentation
    ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();
    clock_t start = clock();
    std::vector<ACMedia*> seg = P->segment(descM, ACM);
    fileOut = "BIC";

    // SelfSIMsegmentation
    /*ACSelfSimSegmentationPlugin* P = new ACSelfSimSegmentationPlugin();
    clock_t start = clock();
 std::vector<ACMedia*> seg = P->segment(desc,ACM);
 fileOut = "SelfSim"; //ccl*/


    //Different Segmentation Types
    //std::vector<ACMedia*> ACAudioSegmentationPlugin::segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia)
    /*
 //plotting in gnuplot
    arma::fmat M = P->get_features();
    int n = M.n_rows;
 arma::fcolvec t = desc[0]->getTime();


 std::vector<double> seg_d;
    for (int i = 0; i < seg.size(); i++) {
        seg_d.push_back(t[seg[i]]);
    }
    std::vector<double> seg_i; // segments heights
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(n + 5);
    }

 // plotting segments
 Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();

    g1.set_style("impulses");
    if (seg_d.size() > 0) {
        g1.plot_xy(seg_d, seg_i, "segments");
  // for output in postscript file:
  // uncomment the following lines for output in terminal
  g1.cmd("set terminal postscript eps");
  string fout = "set output \"" + _dir + _fname + fileOut + ".eps\"";
  g1.cmd(fout.c_str());
  g1.cmd("replot");
 }
 */
    cout << "Enter char to exit" << endl;
    char c;
    cin >> c;

    delete P;

}
#endif

void test_bic_from_file(std::string _dir, std::string _fname) {
    ifstream data_file;
    data_file.open((_dir + _fname).c_str());
    if (!data_file) {
        cerr << "<test_single_bic_from_filey> error reading file " << _fname << endl;
        return;
    } else {
        cout << "opening " << _fname << endl;
    }

    double in;
    vector<double> m;

    while (data_file >> in) {
        m.push_back(in);
    }

    arma::fmat M;
    int s = m.size();
    M.set_size(1, s);

    for (int i = 0; i < s; i++) {
        M(0, i) = m[i];
    }

    float moy = mean(M.row(0));
    float stv = stddev(M.row(0));

    if (stv != 0) M = (M - moy) / stv;
    data_file.close();

    M.print();

    /*ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();

    //default: lambda(1), sampling_rate(1), Wmin(20), bic_thresh(0.5), jump_width(5)
    clock_t start = clock();
    std::vector<int> seg = P->testSegment(M, 1, 1, 20, .5, 1);
    std::cout << "Time elapsed (segmentation alone): " << ((double) clock() - start) / CLOCKS_PER_SEC << std::endl;

    std::vector<double> seg_d(seg.begin(), seg.end());
    std::vector<double> seg_i;
    for (int i = 0; i< int(seg_d.size()); i++) {
        seg_i.push_back(3 * moy);
    }
    delete P;
    Gnuplot g1 = Gnuplot("lines");
    //	g1.cmd("set terminal postscript") ;
    g1.cmd("set terminal x11");
    g1.reset_plot();
    //	string fout = "set output \""+_dir+_fname+"_segments.ps\"";
    //    g1.cmd(fout.c_str()) ;
    g1.plot_x(m, "data");
    g1.set_style("impulses");
    g1.plot_xy(seg_d, seg_i, "segments");
    sleep(10);*/
}

int main(int argc, char *argv[]) {
    //cout << "testing BIC segmentation" << endl;
    //string sdir = "/Users/xavier/numediart/Project11.1-MediaBlender/results/";
    //string sdir = "/home/xavier/development/tmp_test_files/";
    //string sdir = "/Users/ccl/data/Projects/numediart/MoDyVa/DB/Wav_MoDyVA_CAT/Events_adjust/";
    string sdir = "/Users/ccl/data/Projects/numediart/MoDyVa_DameBlanche/DB/Wav_MoDyVA_CAT/Events_adjust/selection/";
    //test_single_bic(sdir, "segment_1D.eps");
    //test_double_bic(sdir,"segment_2D_mix.eps");

    //	int n=2;
    //	if(argc>1)
    //	{
    //		if(atoi(argv[1])>0)
    //		{
    //			n=atoi(argv[1]);
    //		}
    //	}
    //	test_multiple_bic(4);
    //test_multiple_selfsim(4);
    //test_bic_from_file ( sdir, "test_seg_from_file.in" );
    //	test_bic_from_file ( sdir, "arma_mtf_test.txt" );
#ifdef SUPPORT_AUDIO
    //test_segmentation_from_laughter_file("/home/jerome/NetBeansProjects/MediaCycle/3_619609_621620.wav");
    //test_segmentation_from_laughter_file(sdir, "Event9_44100");
    test_ALLsegmentation_from_Audiofile(sdir, "Event9_44100");
#endif
    return 0;
}

