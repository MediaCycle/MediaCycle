/*
   QtKinectWrapper - Qt Kinect Wrapper Class
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "QKinectWrapper.h"
#include <math.h>
//#include <cmath>
//#include "precisetimer.h"
#include <iostream>
#include <sstream>


#define FFTTIMEWINDOWS 2.f
#define FFTNSAMPLES 512
#define TIMESAMPLE (1.0/30.0)
#define NTEMPOBUFFER 5
#define OVERLAP 0.5
// XS to calculate median easily
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

double time1,time2;
#include <sys/time.h>

float tempo,zMem;
static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};

    gettimeofday(&tv, &tz);

    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}
double median(double a,double b, double c);

//KINECT CALLBACK
void print_complex_1d(fftw_complex* arr, int size, int normalize) {
    for (int i = 0; i < size; i++) {
        double a = arr[i][0] / (normalize ? size : 1);
        double b = arr[i][1] / (normalize ? size : 1);
        cout << a << " " << b << endl;
    }
}

namespace QKinect
{

int BodyColors[][3] =
{
    {0,127,127},
    {0,0,127},
    {0,127,0},
    {127,127,0},
    {127,0,0},
    {127,63,0},
    {63,127,0},
    {0,63,127},
    {63,0,127},
    {127,127,63},
    {127,127,127}
};


/******************************************************************************
*******************************************************************************
* QKinectWrapper   QKinectWrapper   QKinectWrapper   QKinectWrapper   QKinectWrapper
*******************************************************************************
******************************************************************************/


/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/

bool calibration_done = false;

/**
   \brief Constructor - does nothing with the kinect driver but inits some internal vars
**/
QKinectWrapper::QKinectWrapper()
{
    Nsamples = 512; // number of hands positions to consider for tempo computation
    // 50 -> 2 seconds (to test)
    // 100 -> 4 seconds (may be too long)
    Zhands=new vector<double>;
    TimeHands=new vector<double>;


    VzHandsComplete=new double[FFTNSAMPLES];
    for (int i=0; i< FFTNSAMPLES ; i++){
        VzHandsComplete[i] = 0.0;
    }

    current_index=0;

    tempo=0;
    zMem=0;
    beginWindowsIndex=0;
    midWindowIndex=0;

    // Register types used in signal/slot mechanism
    qRegisterMetaType<QKinect::KinectStatus>( "QKinect::KinectStatus" );
    qRegisterMetaType<QKinect::CalibrationStatus>( "QKinect::CalibrationStatus" );

    status=QKinect::Idle;
    g_bNeedPose=FALSE;
    strcpy(g_strPose,"");
    displayInfoDepth = true;
    displayInfoImage = true;
    displaySkeletonDepth = true;
    displaySkeletonImage = true;

    osc_feedback = 0;

    setSkeletonPen(QPen(QBrush(Qt::white),5));
    setTextPen(QPen(QBrush(Qt::white),5));
    QFont font;
    font.setPointSize(16);
    setFont(font);
}

/**
   \brief Destructor - calls stop - which terminates the kinect-dedicated reading thread - blocks until terminated.
**/
QKinectWrapper::~QKinectWrapper()
{
    stop();
    delete Zhands;
    delete []  VzHandsComplete;
    delete TimeHands;
}
void QKinectWrapper::start()
{
    t_requeststop=false;
    status=QKinect::Idle;
    QThread::start();
}

/**
   \brief Request the the kinect-dedicated reading thread to terminate, and thus
   release all kinect handles.
   Sets requeststop, which makes the main reading loop terminate (thread ended) at
   some later time. Blocks until terminated.
**/
void QKinectWrapper::stop()
{
    t_requeststop=true;

    wait();
}


/**
   \brief Sets the pen used to draw the skeleton
**/
void QKinectWrapper::setSkeletonPen(const QPen & pen)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::skeletonPen = pen;
}

/**
   \brief Sets the pen used to draw text
**/
void QKinectWrapper::setTextPen(const QPen & pen)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::textPen = pen;
}

/**
   \brief Sets the font used to draw the skeleton
**/
void QKinectWrapper::setFont(const QFont &font)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::font = font;
}

/**
   \brief Display info about the state of the objects (tracked, ...) on the depth map
**/
void QKinectWrapper::setDisplayInfoDepth(bool draw)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::displayInfoDepth = draw;
}

/**
   \brief Display info about the state of the objects (tracked, ...) on the camera image
**/
void  QKinectWrapper::setDisplayInfoImage(bool draw)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::displayInfoImage = draw;
}

/**
   \brief Display the skeletonon the depth image
**/
void QKinectWrapper::setDisplaySkeletonDepth(bool draw)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::displaySkeletonDepth = draw;
}

/**
   \brief Display the skeletonon the camera image
**/
void QKinectWrapper::setDisplaySkeletonImage(bool draw)
{
    QMutexLocker locker(&mutex);
    QKinectWrapper::displaySkeletonImage = draw;
}


unsigned QKinectWrapper::getFrameID()
{
    QMutexLocker locker(&mutex);
    return frameid;
}
double QKinectWrapper::getTimestamp()
{
    QMutexLocker locker(&mutex);
    return timestamp;
}

/**
   \brief Return the latest depth image
   Call this afer a dataNotification signal
**/
QImage QKinectWrapper::getDepth()
{
    QMutexLocker locker(&mutex);
    return imageDepth;
}

/**
   \brief Return the latest camera image
   Call this afer a dataNotification signal
**/
QImage QKinectWrapper::getCamera()
{
    QMutexLocker locker(&mutex);
    return imageCamera;
}

/**
   \brief Return the latest bodies
   Call this afer a dataNotification signal
**/
Bodies QKinectWrapper::getBodies()
{
    QMutexLocker locker(&mutex);
    return bodies;
}

/**
   \brief Return the latest camera and depth images and bodies, together with their timestamp.
   Call this afer a dataNotification signal
**/
void QKinectWrapper::getCameraDepthBodies(QImage &camera,QImage &depth,Bodies &bodies, double &ts, unsigned &fid)
{
    QMutexLocker locker(&mutex);
    camera = imageCamera;
    depth = imageDepth;
    bodies = QKinectWrapper::bodies;
    ts = timestamp;
    fid = frameid;
}
/**
   \brief Return the latest camera and depth images, together with their timestamp.
   Call this afer a dataNotification signal
**/
void QKinectWrapper::getCameraDepth(QImage &camera,QImage &depth, double &ts, unsigned &fid)
{
    QMutexLocker locker(&mutex);
    camera = imageCamera;
    depth = imageDepth;
    ts = timestamp;
    fid = frameid;
}

/**
   \brief Return the last error message.
   This should only be called if statusNotification is called and indicates an error
**/
QString QKinectWrapper::getErrorMsg()
{
    QMutexLocker locker(&mutex);
    return errorMsg;

}

/**
   \brief Returns if running
   Runing comprises initializing and reading data (i.e. not running is idle or error)
   We query the thread state to get this.
**/
bool QKinectWrapper::isRunning()
{
    if(QThread::isRunning())
        return true;
    return false;

}


/**
   \brief Returns if stopped
   Runing comprises initializing and reading data (i.e. not running is idle or error)
   We query the thread state to get this.
**/
bool QKinectWrapper::isStopped()
{
    if(QThread::isRunning())
        return false;
    return true;

}

/******************************************************************************
* PRIVATE/PROTECTED   PRIVATE/PROTECTED   PRIVATE/PROTECTED   PRIVATE/PROTECTED
******************************************************************************/


/**
   \brief Starts the kinect-dedicated reading thread.
   Does the following:
   1. Notify the start of the initialization and initialize
   2. Notify the initialization outcomes (error, or running if initialization was successful)
   3. If successful, does a continous:
   3.1. Wait/update kinect data
   3.2. Protected by a mutex, generate the images and data structures, made available to the user
   3.3. Check if a stop has been requested
   4. Stop the kinect reading and release resources, notify.

   The variable accessed outside of this thread are protected by mutexes. This includes:
   - The status
   - The QImages for depth, camera
   - The bodies
   - etc.
**/
void QKinectWrapper::run()
{
    mutex.lock();
    status=QKinect::Initializing;
    emit statusNotification(status);
    mutex.unlock();

    bool ok = initialize();

    if(!ok)
    {
        mutex.lock();
        status = QKinect::ErrorStop;
        emit statusNotification(status);
        mutex.unlock();
        return;
    }

    mutex.lock();
    status = QKinect::OkRun;
    emit statusNotification(status);
    mutex.unlock();

    frameid=0;
    while(!t_requeststop)
    {

        //
        UpdateUserData();

        //double t1,t2;
        //t1 = PreciseTimer::QueryTimer();
        XnStatus status = g_Context.WaitAndUpdateAll();
        //msleep(100+(rand()%100));   // simulate some shit delay
        //if( (frameid%100) > 50)
        // msleep(((frameid%100)-50)*10);           // simulate some slowing down delay delay
        //t2 = PreciseTimer::QueryTimer();
        //printf("Waitandupdate: %lf. %s\n",(t2-t1)*1000.0,xnGetStatusString(status));

        // Prepare the data to export outside of the thread
        mutex.lock();
        xn::DepthMetaData depthMD;
        g_DepthGenerator.GetMetaData(depthMD);
        //frameid = depthMD.FrameID();
        frameid++;
        //printf("frame id: %d %d\n",frameid,depthMD.FrameID());
        timestamp = (double)depthMD.Timestamp()/1000000.0;
        // Must create the bodies first
        bodies = createBodies();
        // Then can create the images, which may overlay the bodies
        imageCamera = createCameraImage();
        imageDepth = createDepthImage();
        emit dataNotification();
        mutex.unlock();


    }
    g_Context.Shutdown();

    mutex.lock();
    status = QKinect::Idle;
    emit statusNotification(status);
    mutex.unlock();
}


void QKinectWrapper::UpdateUserData(){

    XnUserID aUsers[15];
    XnUInt16 nUsers = 15;
    g_UserGenerator.GetUsers(aUsers, nUsers);
    int dx,dy,dz;

    XnPoint3D com[nUsers];	//vecteur de points 3D pour les différents centres de gravité

    // Z global de la main droite
    bool flagHandTrack=false;
    float Ztemp=0.f;

    for (int i=0; i<nUsers; i++)
    {
        g_UserGenerator.GetCoM(aUsers[i], com[i]);

        ostringstream oss;
        oss << i+1;
        std::string result = oss.str();

        string a="userpos"+result;
        string b="userdelta"+result;
        // don't send 0,0,0
        // limit to a square 3x3 for Seneffe
        // XS TODO: read values from a config file
        std::stringstream tmp_i;
        tmp_i << aUsers[i];
        if (com[i].X!=0 && com[i].Y!=0 && com[i].Z > 1500 && com[i].X > -1500 && com[i].X < 1500 && com[i].Z < 3500)
        {
            string adres = "/mediacycle/browser/" + tmp_i.str() + "/hover/xy" ;
            //cout << adres ;//<< endl;
            time1=getTime();
            float x = (-1.0)*(com[i].X) * 1.0 / 1500;
            float y = (2500 - com[i].Z) * 1.0 / 1000;//(2500 - com[i].Z) * 1.0 / 1000;
            //cout <<"time"<<time1-time2  <<"\t"<< (com[i].X) * 1.0 / 1500 <<"\t"<< (2500 - com[i].Z) * 1.0 / 1500 << endl;

            if(osc_feedback){
                if(osc_feedback->isActive())
                    lo_send(osc_feedback->getAddress(),adres.c_str(),"ff",x,y);
            }
        }
        else{
            string adres = "/mediacycle/browser/" + tmp_i.str() + "/released" ;
            //cout << adres << endl;
            if(osc_feedback){
                if(osc_feedback->isActive())
                    osc_feedback->sendMessage( adres );
            }
        }

        XnPoint3D pt;
        //cout<<"com Y"<<com[i].Y<<endl;
        //cout<<"tracked: "<< g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])<<endl;
        if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]) && com[i].Z!=0)
        {
            flagHandTrack=true;
            XnSkeletonJointPosition joint1;
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, joint1);
            pt=joint1.position;

            // XS do not set hands positions of all users.
            // accumulate Z of right hand and try to get a common tempo from it.
            if (pt.X !=0 && pt.Y !=0 && pt.Z!=0) {
                Ztemp+=pt.Y;
            }

            //			string c="handR"+result;
            //			if (pt.X !=0 && pt.Y !=0 && pt.Z!=0) {
            //				ofxOscMessage m2;
            //				m2.setAddress( "/SendHandRight" );
            //				m2.addStringArg( c );
            //				m2.addIntArg( aUsers[i] );
            //				m2.addIntArg( pt.X );
            //				m2.addIntArg( pt.Y );
            //				m2.addIntArg( pt.Z );
            //				sender.sendMessage( m2 );
            //			}

            // XS240711 : removed left hand
            //			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, joint1);
            //			pt=joint1.position;
            //
            //			string d="handL"+result;
            //
            //			if (pt.X !=0 && pt.Y !=0 && pt.Z!=0)
            //			{
            //				ofxOscMessage m3;
            //				m3.setAddress( "/SendHandLeft" );
            //				m3.addStringArg( d );
            //				m3.addIntArg(aUsers[i]);
            //				m3.addIntArg( pt.X );
            //				m3.addIntArg( pt.Y );
            //				m3.addIntArg( pt.Z );
            //				sender.sendMessage( m3 );
            //			}
        }
    }

    // filter to reduce noise
    // XS TODO testme !!!
    //	this->median_filter();
    if (flagHandTrack) {
        TimeHands->push_back(getTime());
        Zhands->push_back(Ztemp);
        current_index=Zhands->size()-1;

        int j=0;

        if (TimeHands->size()>0){
            zMem=Zhands->at(current_index);

            if (TimeHands->at(current_index)-TimeHands->at(0)>FFTTIMEWINDOWS){
                float locTimer=0.f;
                std::vector<double> *ZhandsTemp=new std::vector<double>;
                std::vector<double> *TimeHandsTemp=new std::vector<double>;

                for (int k=0;k<(TimeHands->size()-1);k++) {
                    if ((TimeHands->at(k)-TimeHands->at(0))>(FFTTIMEWINDOWS*OVERLAP)){
                        ZhandsTemp->push_back(Zhands->at(k));
                        TimeHandsTemp->push_back(TimeHands->at(k));
                    }

                    while ((locTimer<=(TimeHands->at(k+1)-TimeHands->at(0)))&&(locTimer<2.0)) {
                        VzHandsComplete[j]=(Zhands->at(k+1)-Zhands->at(k))/(TimeHands->at(k+1)-TimeHands->at(k));
                        locTimer+=TIMESAMPLE;
                        j++;
                        if (j==FFTNSAMPLES)
                        {
                            delete ZhandsTemp;
                            delete TimeHandsTemp;
                            return;
                        }
                    }

                }
                ZhandsTemp->push_back(Zhands->at(current_index));
                TimeHandsTemp->push_back(TimeHands->at(current_index));
                delete Zhands;
                delete TimeHands;
                int test=ZhandsTemp->size();
                int test2=TimeHandsTemp->size();
                Zhands=ZhandsTemp;
                TimeHands=TimeHandsTemp;
                ZhandsTemp=0;
                TimeHandsTemp=0;
                //cout <<"Z:" <<endl;

                //	for (int i=0;i<512;i++)
                //		cout <<Zhands[i] <<endl;
                //cout <<"Zcomplete:" <<endl;
                //for (int i=0;i<512;i++)
                //	cout <<VzHandsComplete[i] <<endl;

                double lTempo = (this->find_tempo());
                // ne pas envoyer de message OSC si on est sous le seuil de détection (i.e. tempo = 0)

                if (lTempo > 50.0 && lTempo<190.0){
                    if (tempoBuffer.size()>3)
                    {
                        for(int i=0;i<tempoBuffer.size()-1;i++)
                            tempoBuffer[i]=tempoBuffer[i+1];
                        tempoBuffer.pop_back();
                    }
                    int testl=tempoBuffer.size();


                    tempoBuffer.push_back(lTempo);
                    double tempofiltered=0;
                    for (int l=0;l<tempoBuffer.size();l++)
                        tempofiltered+=tempoBuffer[l];
                    tempofiltered/=tempoBuffer.size();

                    /*double tempoMedianFiltered;
                    if ((tempoBuffer.size()>1)&&(medianFilteredTempoBuffer.size()>0))
                        tempoMedianFiltered=median(tempoBuffer[tempoBuffer.size()-1],tempoBuffer[tempoBuffer.size()-2],medianFilteredTempoBuffer[medianFilteredTempoBuffer.size()-1]);
                    else {
                        tempoMedianFiltered=tempoBuffer[tempoBuffer.size()-1];
                    }

                    if (medianFilteredTempoBuffer.size()>3)
                    {
                        for(int i=0;i<medianFilteredTempoBuffer.size()-1;i++)
                            medianFilteredTempoBuffer[i]=medianFilteredTempoBuffer[i+1];
                        medianFilteredTempoBuffer.pop_back();
                    }

                    testl=medianFilteredTempoBuffer.size();
                    medianFilteredTempoBuffer.push_back(tempoMedianFiltered);
                    double tempofiltered=0;
                    for (int l=0;l<medianFilteredTempoBuffer.size();l++)
                        tempofiltered+=medianFilteredTempoBuffer[l];
                    tempofiltered/=medianFilteredTempoBuffer.size();
                    cout << "tempo"<<"\t"<<tempo <<endl ;*/

                    string mess = "/mediacycle/player/bpm";
                    tempo=tempofiltered;
                    std::cout << mess << " " << tempo << std::endl;
                    if(osc_feedback){
                        if(osc_feedback->isActive())
                            lo_send(osc_feedback->getAddress(),mess.c_str(),"f",tempo);
                    }
                }
                for (int i=0;i<FFTNSAMPLES;i++){
                    VzHandsComplete[i]=0;
                    //Zhands[i]=0;
                }
                //current_index = 0;
            }
        }
    }
    // tries to set up BPM from the position of the right hand
}

int QKinectWrapper::find_peak_index(fftw_complex* Zc, int size){
    double maxi = 0.0;
    int imax = 0;
    double mean=0;0;
    for (int i = 1; i < 15; i++) { // skip 0
        double ab = pow(Zc[i][0], 2) + pow(Zc[i][1], 2);
        mean+=ab;
    }
    for (int i = 15; i < size; i++) { // skip 0
        double ab = pow(Zc[i][0], 2) + pow(Zc[i][1], 2);
        mean+=ab;
        if (ab > maxi) {
            maxi=ab;
            imax=i;
        }
    }
    mean/=size;

    // ** XS ** PARAM
    double peak_thresh = 1000; // XS TODO adapt this value !
    if ((maxi < 9*mean)||(maxi<1*400000000))//TR Empiric value with 1 subject.
        return 0;

    //	cout << "maxi = " << maxi << " at imax = " << imax << endl;
    return imax;
}

float QKinectWrapper::find_tempo(){
    int N = this->Nsamples;

    // ** XS ** PARAM
    int Nhigh = 512; // for zero-padding
    // 60 bpm = 1 bps = 1 Hz
    // pour Nhigh=512, on a une résolution de 25 / 500 = 0.05 Hz = 3 bpm
    int M = Nhigh/2 + 1;
    fftw_complex* Zfft ;
    fftw_plan plan_f;
    Zfft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * M);

    //	plan_f = fftw_plan_dft_r2c_1d(N, Zhands, Zfft, FFTW_ESTIMATE);   // no zero padding
    plan_f = fftw_plan_dft_r2c_1d(Nhigh, VzHandsComplete, Zfft, FFTW_ESTIMATE); // zero padding
    fftw_execute(plan_f);

    // XS composante continue : si elle est trop faible (seuil à tester)
    // on ne calcule pas le pic

    //	double Z0 = pow(Zfft[0][0], 2) + pow(Zfft[0][1], 2);
    //
    //	double Zthresh = 1000; // XS TODO check this one !
    //	if (Z0 < Zthresh)
    //		return 0;

    double maxi;
    int imax = find_peak_index(Zfft,M);
    float lTempo=(60.f*(float)imax/FFTNSAMPLES)/TIMESAMPLE;
    fftw_destroy_plan(plan_f);
    fftw_free(Zfft);

    return lTempo;
}

double median(double a,double b, double c){
    int lMean=(a+b+c)/3;
    double da=fabs(a-lMean);
    double db=fabs(b-lMean);
    double dc=fabs(c-lMean);

    if (da<=db&&da<=dc)
        return a;
    if (db<=da&&db<=dc)
        return b;
    if (dc<db&&dc<da)
        return c;

}


/**
   \brief Setups and initializes the kinect/openni
   Returns false if any step fails.
**/
bool QKinectWrapper::initialize()
{
    XnStatus nRetVal = XN_STATUS_OK;
    XnMapOutputMode outputMode;
    XnCallbackHandle hUserCallbacks;
    //XnCallbackHandle hCalibrationCallbacks, hPoseCallbacks;
    XnCallbackHandle hCalibrationStart, hCalibrationComplete,hPoseDetected;
    XnCallbackHandle hHandCreate, hHandUpdate, hHandDestroy, hHand;

    // Initialize the context
    nRetVal = g_Context.Init();
    if(nRetVal!=XN_STATUS_OK)
    {
        mutex.lock();
        errorMsg = QString("Context creation failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }

    // Create a depth generator
    nRetVal = g_DepthGenerator.Create(g_Context);
    if(nRetVal!=XN_STATUS_OK)
    {
        mutex.lock();
        errorMsg = QString("Depth generator creation failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }
    outputMode.nXRes = 640;
    outputMode.nYRes = 480;
    outputMode.nFPS = 30;
    nRetVal = g_DepthGenerator.SetMapOutputMode(outputMode);
    if(nRetVal!=XN_STATUS_OK)
    {
        mutex.lock();
        errorMsg = QString("Depth generator SetMapOutputMode failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }


    // Create an image generator
    nRetVal = g_ImageGenerator.Create(g_Context);
    if(nRetVal!=XN_STATUS_OK)
    {
        mutex.lock();
        errorMsg = QString("Image generator creation failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }
    outputMode.nXRes = 640;
    outputMode.nYRes = 480;
    outputMode.nFPS = 30;
    nRetVal = g_ImageGenerator.SetMapOutputMode(outputMode);
    if(nRetVal!=XN_STATUS_OK)
    {
        mutex.lock();
        errorMsg = QString("Image generator SetMapOutputMode failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }

    // Creates a user generator
    nRetVal = g_UserGenerator.Create(g_Context);
    if(nRetVal!=XN_STATUS_OK)
    {
        mutex.lock();
        errorMsg = QString("User generator creation failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }

    // See if we've skeleton capabilities
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    {
        mutex.lock();
        errorMsg = QString("user generator doesn't support skeleton");
        mutex.unlock();
        return false;
    }

    g_UserGenerator.RegisterUserCallbacks(QKinectWrapper::User_NewUser, QKinectWrapper::User_LostUser, this, hUserCallbacks);
    // API v 1.0:
    //g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(QKinectWrapper::UserCalibration_CalibrationStart, QKinectWrapper::UserCalibration_CalibrationEnd, this, hCalibrationCallbacks);
    // API v 1.1+:
    g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(QKinectWrapper::UserCalibration_CalibrationStart,this,hCalibrationStart);
    g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(QKinectWrapper::UserCalibration_CalibrationComplete,this,hCalibrationComplete);

    if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
    {
        g_bNeedPose = TRUE;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            mutex.lock();
            errorMsg = QString("User generator needs pose, but not supported\n");
            mutex.unlock();
            return false;
        }

        // API v 1.0:
        //g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(QKinectWrapper::UserPose_PoseDetected, NULL, this, hPoseCallbacks);
        // API v 1.1+:
        g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(QKinectWrapper::UserPose_PoseDetected, this, hPoseDetected);

        g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);


    /*
   // Creates a hand generator
   printf("Before hand create\n");
   nRetVal = g_HandsGenerator.Create(g_Context);
   printf("After hand create\n");
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Hands generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }

   nRetVal = g_HandsGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this,hHand);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Hands generator can't register callbacks: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }


   printf("Before gesture create\n");
   nRetVal = g_GestureGenerator.Create(g_Context);
   printf("After gesture create\n");
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Gesture generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }





   nRetVal = g_GestureGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this,hHand);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Hands generator can't register callbacks: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }
*/


    // Start producting data
    nRetVal = g_Context.StartGeneratingAll();



    return true;

}

/**
   \brief User found
   Emits a user found notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
    if(!calibration_done){
        if (pthis->g_bNeedPose)
            pthis->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(pthis->g_strPose, nId);
        else
            pthis->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
    else {
        // Propagate skeleton
        pthis->g_UserGenerator.GetSkeletonCap().LoadCalibrationData(nId, 0);
        pthis->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    }
    emit pthis->userNotification(nId,true);
}

/**
   \brief User lost
   Emits a user lost notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    QKinectWrapper *pthis = (QKinectWrapper*)pCookie;

    std::stringstream tmp_i;
    tmp_i << nId;
    std::string adres = "/mediacycle/browser/" + tmp_i.str() + "/released" ;
    //std::cout << adres << std::endl;
    if(pthis->osc_feedback){
        if(pthis->osc_feedback->isActive())
            pthis->osc_feedback->sendMessage( adres );

        emit pthis->userNotification(nId,false);
    }
}

/**
   \brief Pose detected
   Emits a pose notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{

    QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
    pthis->g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    pthis->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    emit pthis->poseNotification(nId,QString(strPose));
}


/**
   \brief Start calibration
   Emits a calibration notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
    //printf("----------------- CALIBRATION STARTED for USER %d\n", nId);
    emit pthis->calibrationNotification(nId,CalibrationStart);
}

/**
   \brief Finished calibration
   Emits a calibration notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus calibrationError, void* pCookie)
{
    QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
    if (calibrationError == XN_CALIBRATION_STATUS_OK)
    {
        pthis->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
		pthis->g_UserGenerator.GetSkeletonCap().SaveCalibrationData(nId,0);
		calibration_done = true;
        emit pthis->calibrationNotification(nId,CalibrationEndSuccess);
        return;
    }
    // Calibration failed
    if (pthis->g_bNeedPose)
    {
        pthis->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(pthis->g_strPose, nId);
    }
    else
    {
        pthis->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
    emit pthis->calibrationNotification(nId,CalibrationEndFail);
}

/**
  \brief
**/
/*void XN_CALLBACK_TYPE QKinectWrapper::Hand_Create(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat time,void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;

   printf("Hand create. id %d. position: %f %f %f. time: %f\n",nId,pPosition->X,pPosition->Y,pPosition->Z,time);
}*/

/**
  \brief
**/
/*void XN_CALLBACK_TYPE QKinectWrapper::Hand_Destroy(xn::HandsGenerator& generator, XnUserID nId, XnFloat time,void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;

   printf("Hand create. id %d. time: %f\n",nId,time);
}*/

/**
  \brief
**/
/*void XN_CALLBACK_TYPE QKinectWrapper::Hand_Update(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat time,void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   printf("Hand update. id %d. position: %f %f %f. time: %f\n",nId,pPosition->X,pPosition->Y,pPosition->Z,time);
}*/

/**
  \brief Converts joint to the projective, marks if the projection is valid (joint confidence is high enough)
**/
void QKinectWrapper::getJointProj(XnSkeletonJointPosition joint,XnPoint3D &proj,bool &valid)
{
    if(joint.fConfidence<0.5)
    {
        valid = false;
    }
    else
    {
        valid = true;
        g_DepthGenerator.ConvertRealWorldToProjective(1, &joint.position, &proj);
    }
}

/**
  \brief Fill the Bodies data structure holding all the infos about the detected users.
**/
Bodies QKinectWrapper::createBodies()
{

    Bodies bodies;

    Body b;


    XnUserID aUsers[15];
    XnUInt16 nUsers = 15;
    g_UserGenerator.GetUsers(aUsers, nUsers);
    for (int i = 0; i < nUsers; ++i)
    {
        // id
        b.id = aUsers[i];
        // Get the center of mass and its projection
        g_UserGenerator.GetCoM(aUsers[i], b.com);
        g_DepthGenerator.ConvertRealWorldToProjective(1, &b.com, &b.proj_com);
        if(isnan(b.proj_com.X) || isnan(b.proj_com.Y) || isnan(b.proj_com.Z))
            b.proj_com_valid=false;
        else
            b.proj_com_valid=true;

        if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
        {
            // If the user is tracked, get the skeleton
            b.status=QKinect::Tracking;
            //b.tracked=true;
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD, b.joints[QKinect::Head]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_NECK, b.joints[QKinect::Neck]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_SHOULDER, b.joints[QKinect::LeftShoulder]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_ELBOW, b.joints[QKinect::LeftElbow]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, b.joints[QKinect::LeftHand]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_SHOULDER, b.joints[QKinect::RightShoulder]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_ELBOW, b.joints[QKinect::RightElbow]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, b.joints[QKinect::RightHand]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, b.joints[QKinect::Torso]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HIP, b.joints[QKinect::LeftHip]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_KNEE, b.joints[QKinect::LeftKnee]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_FOOT, b.joints[QKinect::LeftFoot]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HIP, b.joints[QKinect::RightHip]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_KNEE, b.joints[QKinect::RightKnee]);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_FOOT, b.joints[QKinect::RightFoot]);

            // Get the projection of the skeleton
            for(unsigned i=QKinect::Head;i<=QKinect::RightFoot;i++)
                getJointProj(b.joints[i],b.proj_joints[i],b.proj_joints_valid[i]);
        }
        else
        {
            //b.tracked=false;
            if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUsers[i]))
                b.status=QKinect::Calibrating;
            else
                b.status=QKinect::LookingForPose;
        }
        bodies.push_back(b);
    }
    return bodies;
}

/**
  \brief Creates a QImage comprising the depth map
**/
QImage QKinectWrapper::createDepthImage()
{
    // Here must mutex / run also access the data
    xn::SceneMetaData smd;
    xn::DepthMetaData dmd;
    g_DepthGenerator.GetMetaData(dmd);
    g_UserGenerator.GetUserPixels(0, smd);

    XnUInt16 g_nXRes = dmd.XRes();
    XnUInt16 g_nYRes = dmd.YRes();

    QImage image(g_nXRes,g_nYRes,QImage::Format_RGB32);


    const XnDepthPixel* pDepth = dmd.Data();
    const XnLabel* pLabels = smd.Data();

    // Compute stats
    /*unsigned max,min;
   max = pDepth[0];
   min = 0;
   for (unsigned i=0; i<g_nYRes*g_nXRes; i++)
   {
      if(pDepth[i]>max)
         max = pDepth[i];
      if(pDepth[i]!=0)
      {
         if(min==0)
            min = pDepth[i];
         else
            if(pDepth[i]<min)
               min = pDepth[i];
      }
   }
   printf("Depth min/max: %u %u\n",min,max);*/

    for (unsigned nY=0; nY<g_nYRes; nY++)
    {
        uchar *imageptr = image.scanLine(nY);

        for (unsigned nX=0; nX < g_nXRes; nX++)
        {
            unsigned depth = *pDepth;
            unsigned label = *pLabels;


            unsigned maxdist=10000;
            if(depth>maxdist) depth=maxdist;
            if(depth)
            {
                depth = (maxdist-depth)*255/maxdist+1;
            }
            // depth: 0: invalid
            // depth: 255: closest
            // depth: 1: furtherst (maxdist distance)


            if(label)
            {
                imageptr[0] = BodyColors[label][0]*2*depth/255;
                imageptr[1] = BodyColors[label][1]*2*depth/255;
                imageptr[2] = BodyColors[label][2]*2*depth/255;
                imageptr[3] = 0xff;
            }
            else
            {
                // Here we could do depth*color, to show the colored depth
                imageptr[0] = depth;
                imageptr[1] = depth;
                imageptr[2] = depth;
                imageptr[3] = 0xff;
            }
            pDepth++;
            imageptr+=4;
            pLabels++;
        }
    }


    QPainter painter;
    painter.begin(&image);
    if(displayInfoDepth)
    {
        painter.setPen(textPen);
        painter.setFont(font);
        drawInfo(&painter);
    }
    if(displaySkeletonDepth)
    {
        painter.setPen(skeletonPen);
        drawSkeleton(&painter);
    }
    painter.end();
    return image;

}


/**
  \brief Creates a QImage comprising the depth map
**/
QImage QKinectWrapper::createCameraImage()
{

    // Here must mutex / run also access the data
    xn::DepthMetaData dmd;
    xn::ImageMetaData imd;
    g_DepthGenerator.GetMetaData(dmd);
    g_ImageGenerator.GetMetaData(imd);

    XnUInt16 g_nXRes = dmd.XRes();
    XnUInt16 g_nYRes = dmd.YRes();

    QImage image(g_nXRes,g_nYRes,QImage::Format_RGB32);


    const XnUInt8 *idata = imd.Data();
    for (unsigned nY=0; nY<g_nYRes; nY++)
    {
        uchar *imageptr = image.scanLine(nY);

        for (unsigned nX=0; nX < g_nXRes; nX++)
        {
            imageptr[0] = idata[2];
            imageptr[1] = idata[1];
            imageptr[2] = idata[0];
            imageptr[3] = 0xff;

            imageptr+=4;
            idata+=3;

        }
    }
    QPainter painter;
    painter.begin(&image);
    if(displayInfoImage)
    {
        painter.setPen(textPen);
        painter.setFont(font);
        drawInfo(&painter);
    }
    if(displaySkeletonImage)
    {
        painter.setPen(skeletonPen);
        drawSkeleton(&painter);
    }
    painter.end();

    return image;

}



/**
  \brief Display infos about the detected bodies
**/
void QKinectWrapper::drawInfo(QPainter *painter)
{
    char str[256];
    for(unsigned i=0;i<bodies.size();i++)
    {
        if(bodies[i].status == QKinect::Tracking)
        {
            sprintf(str, "%d: tracking", bodies[i].id);
        }
        if(bodies[i].status == QKinect::Calibrating)
        {
            sprintf(str, "%d: calibrating...", bodies[i].id);
        }
        if(bodies[i].status == QKinect::LookingForPose)
        {
            sprintf(str, "%d: looking for pose", bodies[i].id);
        }
        painter->drawText(bodies[i].proj_com.X, bodies[i].proj_com.Y,QString(str));
    }
}

/**
  \brief Draws on painter the skeleton of all bodies
**/

void QKinectWrapper::drawSkeleton(QPainter *painter)
{
    for(unsigned i=0;i<bodies.size();i++)
    {
        drawLimb(painter,bodies[i],QKinect::Head,QKinect::Neck);

        drawLimb(painter,bodies[i],QKinect::Neck,QKinect::LeftShoulder);
        drawLimb(painter,bodies[i],QKinect::LeftShoulder,QKinect::LeftElbow);
        drawLimb(painter,bodies[i],QKinect::LeftElbow,QKinect::LeftHand);

        drawLimb(painter,bodies[i],QKinect::Neck,QKinect::RightShoulder);
        drawLimb(painter,bodies[i],QKinect::RightShoulder,QKinect::RightElbow);
        drawLimb(painter,bodies[i],QKinect::RightElbow,QKinect::RightHand);

        drawLimb(painter,bodies[i],QKinect::LeftShoulder,QKinect::Torso);
        drawLimb(painter,bodies[i],QKinect::RightShoulder,QKinect::Torso);

        drawLimb(painter,bodies[i],QKinect::Torso,QKinect::LeftHip);
        drawLimb(painter,bodies[i],QKinect::LeftHip,QKinect::LeftKnee);
        drawLimb(painter,bodies[i],QKinect::LeftKnee,QKinect::LeftFoot);

        drawLimb(painter,bodies[i],QKinect::Torso,QKinect::RightHip);
        drawLimb(painter,bodies[i],QKinect::RightHip,QKinect::RightKnee);
        drawLimb(painter,bodies[i],QKinect::RightKnee,QKinect::RightFoot);

        drawLimb(painter,bodies[i],QKinect::LeftHip,QKinect::RightHip);
    }
}


/**
  \brief Draws on painter the limb j1-j2 of body
**/
void QKinectWrapper::drawLimb(QPainter *painter,const Body &body, BodyJoints j1,BodyJoints j2)
{
    if(body.proj_joints_valid[j1]==false || body.proj_joints_valid[j2]==false)
        return;
    painter->drawLine(body.proj_joints[j1].X,body.proj_joints[j1].Y,body.proj_joints[j2].X,body.proj_joints[j2].Y);
}


}     // End namespace




