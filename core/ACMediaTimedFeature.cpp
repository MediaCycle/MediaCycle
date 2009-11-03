
/*
 *  ACMediaTimedFeature.cpp
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 22/06/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "ACMediaTimedFeature.h"
//#include <gsl/gsl_interp.h>

using namespace arma;
using namespace std;

ACMediaTimedFeatures::ACMediaTimedFeatures(){
}

ACMediaTimedFeatures::~ACMediaTimedFeatures(){
}

ACMediaTimedFeatures::ACMediaTimedFeatures(fcolvec time_v, fmat value_m){
  this->time_v = time_v;
  this->value_m = value_m;
}

ACMediaTimedFeatures::ACMediaTimedFeatures(fcolvec time_v, fmat value_m, string name, vector<float> seg_v){
  this->time_v = time_v;
  this->value_m = value_m;
  this->name = name;
  this->seg_v = seg_v;
}

ACMediaTimedFeatures::ACMediaTimedFeatures( const vector<float> &time, const vector< vector<float> > &value, string name, const vector<float> *seg_v){
	this->time_v = fcolvec(time.size());
	this->value_m = fmat((int) value[0].size(), (int) value.size());
	for (unsigned int Itime=0; Itime<time.size(); Itime++){
		this->time_v(Itime) = time[Itime];
		for (unsigned int Idim=0; Idim<value.size(); Idim++){
			this->value_m(Itime, Idim) = value[Idim][Itime];
		}
	}
	this->name = name;
	if (seg_v !=0) this->seg_v = *seg_v;	
}

ACMediaTimedFeatures::ACMediaTimedFeatures(const vector<float> &time, const vector<float> &value, string name, const vector<float> *seg_v){
  if (time.size() != value.size()) {
    cerr << "Time and value vectors does not have the same size" << endl;
    exit(-1);
  }

  this->time_v = fcolvec(time.size());
  this->value_m = fmat((int) time.size(),1);
  for (int Itime=0; Itime<time.size(); Itime++){
    this->time_v(Itime) = time[Itime];
    this->value_m(Itime, 1) = value[Itime];
  }
  this->name = name;
  if (seg_v !=0) this->seg_v = *seg_v;	
}

// We pass by adresse now
// ACMediaTimedFeatures::ACMediaTimedFeatures(vector<float> time, vector< vector<float> > value, string name, vector<float> seg_v){
//   this->time_v = fcolvec(time.size());
//   this->value_m = fmat((int) value[0].size(), (int) value.size());
//   for (int Itime=0; Itime<time.size(); Itime++){
//     this->time_v(Itime) = time[Itime];
//     for (int Idim=0; Idim<value.size(); Idim++){
//       this->value_m(Itime, Idim) = value[Idim][Itime];
//     }
//   }
//   this->name = name;
//   this->seg_v = seg_v;
// }

ACMediaTimedFeatures::ACMediaTimedFeatures( float *time, int length, float **value, int dim, string name, vector< float > seg_v ){
  this->time_v = fcolvec( length );
  // value is assumed to be dim (first dim) by length (2nd dim) 
  this->value_m = fmat( (int) length, (int) dim );
  for (int Itime=0; Itime<length; Itime++){
    this->time_v(Itime) = time[Itime];
    for (int Idim=0; Idim<dim; Idim++){
      this->value_m(Itime, Idim) = value[Idim][Itime];
    }
  }
  this->name = name;
  //vector< float > seg_v( seg_fp, seg_fp + nBursts );
  this->seg_v = seg_v;
}

// We pass by adresse now
// ACMediaTimedFeatures::ACMediaTimedFeatures(vector<float> time, vector<float> value, string name, vector<float> seg_v){
//   if (time.size() != value.size()) {
//     cerr << "Time and value vectors does not have the same size" << endl;
//     exit(-1);
//   }

//   this->time_v = fcolvec(time.size());
//   this->value_m = fmat((int) time.size(),1);
//   for (int Itime=0; Itime<time.size(); Itime++){
//     this->time_v(Itime) = time[Itime];
//     this->value_m(Itime, 1) = value[Itime];
//   }
//   this->name = name;
//   this->seg_v = seg_v;
// }

void ACMediaTimedFeatures::importFromFile(string filename){ //see also readFile
  fmat tmp_m;
  tmp_m.load(filename);
  //  tmp_m.print();
  this->value_m = tmp_m.cols(1,tmp_m.n_cols-1);
  this->time_v = tmp_m.col(0);
}

void ACMediaTimedFeatures::importSegmentsFromFile(string filename){
  ifstream inf(filename.c_str(),ifstream::in);
  int i=0;
  float tmp;
  while (true){
    inf >> tmp;
    this->seg_v.push_back(tmp);
    if (inf.eof()) break;
    if (i >= 1) //otherwise there is a problem if tmp is equal to 0
    {
        if (seg_v[i] < seg_v[i-1]){
            cerr << "badly formed segments " << endl;
            exit(-1);
        }
        //cout << seg_v.size() << " ; " << seg_v[i] << endl;
    }
    i++;
  }
  inf.close();
}


fcolvec ACMediaTimedFeatures::getTime() {
  return time_v;
}

float ACMediaTimedFeatures::getTime(int index) {
  return time_v(index);
}

vector<float> ACMediaTimedFeatures::getSegments(){
  return seg_v;
}

float ACMediaTimedFeatures::getSegments(int index){
  return seg_v[index];
}

int ACMediaTimedFeatures::getNearestTimePosition(float time, int mode){
  // mode = 0, nearest left
  // mode = 1, nearest right
  // mode = 2, nearest
  // return an index !

  int i=0;
  float currTime = this->time_v(0);
  int nearPos;
  while (currTime < time && i < this->getLength()-1){
    i++;
    currTime = this->time_v(i);
  }

    switch(mode)
    {
    case 0:
        if (i>0)
          if (i == this->getLength()-1)
            nearPos = i;
          else
            nearPos = i-1;
        else
          nearPos = 0;
        break;

    case 1:
        nearPos = i;
        break ;

    case 2:
        int i_minus, i_plus;
        float t_minus, t_plus;
        i_minus = getNearestTimePosition(time, 0);
        t_minus = this->time_v(i_minus);
        i_plus = getNearestTimePosition(time, 1);        
        t_plus = this->time_v(i_plus);
        if (time - t_minus <= t_plus - time)
            nearPos = i_minus;
        else
            nearPos = i_plus;
        break;

    default:
        break;
    }

////before the switch case and the add of mode 2
//  if (mode==0) {
//    if (i>0)
//      if (i == this->getLength()-1)
//	nearPos = i;
//      else
//	nearPos = i-1;
//    else
//      nearPos = 0;
//  }
//  else
//    nearPos = i;
  
  /*if (currTime == time)   //OK ??
    nearPos=i;*/

    return nearPos;
}

void ACMediaTimedFeatures::setTime(fcolvec time_v) {
  this->time_v = time_v;
}

double* ACMediaTimedFeatures::getTimeAsDouble() {
  double* timeDouble;
  timeDouble = new double[this->getLength()];  
  for (int i=0; i < this->getLength(); i++) {
    timeDouble[i] = this->time_v(i);
  }
  return timeDouble;
}

vector<float> ACMediaTimedFeatures::getTimeAsVector() {
  vector<float> value;
  for (int i=0; i<this->getLength(); i++)
    value.push_back(this->getTime()(i));
  return value;
}


fmat ACMediaTimedFeatures::getValue() {
  return value_m;
}

float ACMediaTimedFeatures::getValue(float index, float dim){
  return this->value_m(index, dim);
}

vector< vector<float> > ACMediaTimedFeatures::getValueAsVector(){
}

void ACMediaTimedFeatures::setValue(fmat value_m) {
  this->value_m = value_m;
}

size_t ACMediaTimedFeatures::getLength() {
  return time_v.n_rows;
}

size_t ACMediaTimedFeatures::getDim() {
  return value_m.n_cols;
}

// fcolvec getValueAtColumn(size_t index){
//   gsl vector* column;  
//   gsl_matrix_get_col (column, this->getValue(), index);
//   return column;
// }

// fcolvec ACMediaTimedFeatures::getValueAtColumn(size_t index){
//   return this->getValue().col(index);
// }

/*double* ACMediaTimedFeatures::getValueAtColumnAsDouble(size_t index){
  double valueDouble[this->getLength()];  
  for (int i=0; i < this->getLength(); i++){
    valueDouble[i] = this->value_m(i, index);
  }
  return valueDouble;
  }*/


fmat ACMediaTimedFeatures::getValueAtTime(fcolvec time_v) {
  // return interpolated values at time_v

  int nearPos;
  fmat outValue_m = fmat(time_v.n_rows,this->getDim());
  for (int i=0; i<time_v.n_rows;i++){
    nearPos = this->getNearestTimePosition(this->time_v(i),0);
    //cout<<"nearPos : " << nearPos<<endl;
    for (int j=0; j<this->getDim(); j++){
        outValue_m(i,j) = this->value_m(nearPos,j);
        //cout<<"j : " << j<<endl;
    }
  }

  /*
  double valueij;
  double valueDouble[this->getLength()];
  double timeDouble[this->getLength()];
  for (int i=0; i<this->getLength();i++){
    timeDouble[i] = this->time_v(i);
  }
  
  outValue_m = fmat(time_v.n_rows,this->getDim());
  const gsl_interp_type* ltype = gsl_interp_linear;
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_interp* interp = gsl_interp_alloc(ltype, this->getLength());
  for (size_t Icol = 0; Icol < this->getDim(); Icol++){
    
    for (int i=0; i<this->getLength();i++){
      valueDouble[i] = this->value_m(i,Icol);
    }

    gsl_interp_init (interp, timeDouble, valueDouble, this->getLength());
    for (size_t Itime=0; Itime<time_v.n_rows; Itime++){
      valueij = gsl_interp_eval(interp, timeDouble, valueDouble, time_v(Itime), acc);    
      outValue_m(Itime, Icol) = valueij;
    }
  }
  gsl_interp_free (interp);*/
  return outValue_m;
}

ACMediaFeatures* ACMediaTimedFeatures::weightedMean(ACMediaTimedFeatures* weight){
  ACMediaFeatures* weightedMean_mf = new ACMediaFeatures();
  fmat weightVal = weight->getValueAtTime(this->getTime());
  float sumWeight = conv_to<float>::from(sum(weightVal));
  fmat tmp_m;
  string name = "Weighted mean of ";
  weightVal = weightVal / sumWeight;
  tmp_m = trans(weightVal) * this->getValue();
  weightedMean_mf->resize(1);
  name += this->getName();
  weightedMean_mf->setName(name);
  for (int i=0; i<tmp_m.n_cols; i++)
    weightedMean_mf->setFeature(i, tmp_m(0,i));
  return weightedMean_mf;
}

ACMediaFeatures* ACMediaTimedFeatures::mean(){
  ACMediaFeatures* mean_mf = new ACMediaFeatures();  
  fmat mean_m = arma::mean(this->getValue());
  string name = "Mean of ";
  name += this->getName();
  mean_mf->setName(name);
  mean_mf->resize(1);
  for (int i=0; i<mean_m.n_cols; i++)
    mean_mf->setFeature(i, mean_m(0,i));  
  return mean_mf;
}

// vector< float > ACMediaTimedFeatures:: meanAsVector(){
//   fmat tmpMean = this->mean()->getValue();
//   vector< float > meanVec;
//   for (int i=0; i<tmpMean.n_cols; i++)
//     meanVec.push_back(tmpMean(0,i));
//   return meanVec;
// }

ACMediaFeatures* ACMediaTimedFeatures::weightedStdDeviation(ACMediaTimedFeatures* weight){
  fmat weightVal = weight->getValueAtTime(this->getTime());
  float sumWeight = conv_to<float>::from(sum(weightVal));
  weightVal = weightVal / sumWeight;

  fmat meanRow = fmat(1,this->getDim());
  fmat meanMatrix = fmat(this->getLength(),this->getDim());
  frowvec onesVector = ones<frowvec>(this->getLength());

  meanRow = onesVector * this->getValue() / this->getLength();
  for (int i=0;i<this->getLength();i++)
      meanMatrix.row(i) = meanRow;
  fmat cDataSq = square(this->getValue() - meanMatrix);
  
  ACMediaFeatures* wstd_mf = new ACMediaFeatures();
  fmat wstd_m;
  wstd_m = sqrt(trans(weightVal) * cDataSq);
  wstd_mf->resize(1);
  string name = "Weighted standard deviation of ";
  name += this->getName();
  wstd_mf->setName(name);
  for (int i=0; i<wstd_m.n_cols; i++)
    wstd_mf->setFeature(i, wstd_m(0,i));
  return wstd_mf;
}

ACMediaFeatures* ACMediaTimedFeatures::std(){  
  fmat std_m = stddev(this->getValue());
  string name = "Standard deviation of ";
  ACMediaFeatures* std_mf = new ACMediaFeatures();
  name += this->getName();
  std_mf->setName(name);
  std_mf->resize(1);
  for (int i=0; i<std_m.n_cols; i++)
    std_mf->setFeature(i, std_m(0,i));  
  return std_mf;
}

// vector< float > ACMediaTimedFeatures:: stdAsVector(){
//   fmat tmpStd = this->std();
//   vector< float > stdVec;
//   for (int i=0; i<tmpStd.n_cols; i++)
//     stdVec.push_back(tmpStd(0,i));
//   return stdVec;
// }

ACMediaTimedFeatures* ACMediaTimedFeatures::delta(){
}

vector<ACMediaTimedFeatures*> ACMediaTimedFeatures::segment(){
  vector<ACMediaTimedFeatures*> acmfSegmented;
  ACMediaTimedFeatures* tmpAtmf;
  int leftPos;
  int rightPos;
  for (int i=0; i<this->seg_v.size()-1; i++){
    leftPos = this->getNearestTimePosition(seg_v[i],1);
    rightPos = this->getNearestTimePosition(seg_v[i+1],0);
    //cout << "left : " << leftPos << endl;
    //cout << "right : " << rightPos << endl;
    if (rightPos < leftPos){
      cerr << "Badly formed segments" << endl;
      exit(-1);
    }

    tmpAtmf = new ACMediaTimedFeatures(this->time_v.rows(leftPos,rightPos), this->value_m.rows(leftPos, rightPos));
    acmfSegmented.push_back(tmpAtmf);
  }  
  return acmfSegmented;
}

ACMediaTimedFeatures* ACMediaTimedFeatures::meanSegment(){
  fcolvec weightValue = ones<fcolvec>(this->getLength());
  //weightValue = weightValue / sum(weightValue);   //unecessary at this time?
  ACMediaTimedFeatures* weight = new ACMediaTimedFeatures(this->getTime(),weightValue);
  return weightedMeanSegment(weight);
}

ACMediaTimedFeatures* ACMediaTimedFeatures::weightedMeanSegment(ACMediaTimedFeatures* weight){
  vector<ACMediaTimedFeatures*> acmtv;
  acmtv = this->segment();
  ACMediaTimedFeatures *outAcmtv;
  ACMediaFeatures* tmpMean_mf = new ACMediaFeatures;
  //  cout << (int) this->getSegments().size() << endl;
  fcolvec outAcmtvTime((int)this->getSegments().size()-1);
  fmat outAcmtvValue((int)this->getSegments().size()-1,(int)this->getDim());
  
  for (int i=0; i<acmtv.size(); i++){
    //acmtv[i]->getTime().print("time seg");    
    tmpMean_mf = acmtv[i]->weightedMean(weight);//1 line * dim column
    for (int Icol=0; Icol<this->getDim(); Icol++)
      outAcmtvValue(i, Icol) = tmpMean_mf->getFeature(Icol);
    //outAcmtvTime(i) = (this->getSegments()[i]+this->getSegments()[i+1])/2;
    outAcmtvTime(i) = (acmtv[i]->getTime(0) + acmtv[i]->getTime(acmtv[i]->getLength()-1) )/2;
  }
  outAcmtv = new ACMediaTimedFeatures(outAcmtvTime, outAcmtvValue);
  //outAcmtv->setName("raf");   //??
  delete(tmpMean_mf);
  return outAcmtv;
}

// ACMediaTimedFeatures* ACMediaTimedFeatures::simpleSplineModeling(ACMediaTimedFeatures* weight, ACMediaSegment* segment){
//}

int ACMediaTimedFeatures::readFile(std::string fileName){
    fmat data;
    data.load(fileName, raw_ascii);
    if(data.n_elem != 0)
    {
        this->setTime(data.col(0));          //time information : first column of the file
        this->setValue(data.cols(1,(data.n_cols)-1));   //data : other columns of the file
        return 1;
    }
    else
    {
        cout<<"Error : "<<fileName<< " does not exist"<<endl;
        return 0;
    }
}



fmat ACMediaTimedFeatures::similarity(int mode)
{
    //mode : distance type    // 0 = euclidian distance

    int samplingFactor = 1;
    fmat data = zeros<fmat>(((int)floor(this->getLength()/samplingFactor))+1, this->getDim());
    for(int i=0; i<this->getLength()-1; i++)
    {
        if ((i%samplingFactor) == 0)
            data.row(i/samplingFactor) = this->getValue().row(i);
    }
    //data.print("data");    //ok

    int n = data.n_rows;
    int p = this->getDim();

    fmat similarityM = zeros<fmat>(n,n);

    for (int i = 0; i < n-1; i++)
    {
        fcolvec dsq = zeros<fcolvec>(n-1-i);
        fcolvec tmpResult = zeros<fcolvec>(n-1-i);

        for (int q = 0; q < p; q++)
        {
            fcolvec tmp2 = data.col(q);
            fcolvec tmp = tmp2.rows(i+1,n-1);
            dsq = dsq + square(data(i,q) - tmp);
        }
        tmpResult = sqrt(dsq);
        similarityM.submat(i+1,i,n-1,i) = tmpResult;
        similarityM.submat(i,i+1,i,n-1) = trans(tmpResult);
    }
    
    return similarityM;

//-------------------------------------
////  no sampling factor !!
//    fmat similarityM = zeros<fmat>(this->getLength(),this->getLength());
//
//    int n = this->getLength();
//    int p = this->getDim();
//
//    for (int i = 0; i < n-1; i++)   //n-1  //i++
//    {
//        fcolvec dsq = zeros<fcolvec>(n-1-i);
//        fcolvec tmpResult = zeros<fcolvec>(n-1-i);
//        for (int q = 0; q < p; q++)
//        {
//            fcolvec tmp2 = this->getValue().col(q);
//            fcolvec tmp = tmp2.rows(i+1,n-1);
//            dsq = dsq + square(this->getValue(i,q) - tmp);
//        }
//        tmpResult = sqrt(dsq);
//        similarityM.submat(i+1,i,n-1,i) = tmpResult;
//        similarityM.submat(i,i+1,i,n-1) = trans(tmpResult);
//    }

//----------------------------------
////  slow version !!
//    for(int i=0;i<this->getLength();i++)
//    {
//        for (int j=i;j<this->getLength();j++)
//        {
//            if (i != j)
//            {
//                float tmp = dist(this->getValue().row(i),this->getValue().row(j),mode);
//                similarityM(i,j) = tmp;
//                similarityM(j,i) = tmp;
//            }
//        }
//    }

}

float ACMediaTimedFeatures::dist(fmat vector1, fmat vector2, int mode)
{
    fmat squareV;
    fmat distM = fmat(1,1);
    switch(mode)
    {
    case 0:   //Euclidian distance
        squareV = square(vector1 - vector2);
        distM = sqrt(sum(squareV,1));
        break;
    //case 1:
    //    break;
    default:
        break;
    }
    return distM(0,0);
}
