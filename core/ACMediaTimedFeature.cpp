
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

ACMediaTimedFeatures::ACMediaTimedFeatures(vector<float> time, vector< vector<float> > value, string name, vector<float> seg_v){
  this->time_v = fcolvec(time.size());
  this->value_m = fmat((int) value[0].size(), (int) value.size());
  for (int Itime=0; Itime<time.size(); Itime++){
    this->time_v(Itime) = time[Itime];
    for (int Idim=0; Idim<value.size(); Idim++){
      this->value_m(Itime, Idim) = value[Idim][Itime];
    }
  }
  this->name = name;
  this->seg_v = seg_v;
}

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

ACMediaTimedFeatures::ACMediaTimedFeatures(vector<float> time, vector<float> value, string name, vector<float> seg_v){
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
  this->seg_v = seg_v;
}

void ACMediaTimedFeatures::importFromFile(string filename){
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
    if (seg_v[i] < seg_v[i-1]){
      cerr << "badly formed segments " << endl;
      exit(-1);
    }
    //    cout << seg_v.size() << " ; " << seg_v[i] << endl;
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

int ACMediaTimedFeatures::getNearestTimePosition(float time, int mode){
  // mode = 0, nearest left
  // mode = 1, nearest right
  //return an index !

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
        if (time - t_minus < t_plus - time)
            nearPos = i_minus;
        else
            nearPos = i_plus;
        break;

    default:
        break;
    }

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
  
  if (currTime == time)
    nearPos=i;

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
  double valueij;
  double valueDouble[this->getLength()];
  double timeDouble[this->getLength()];
  int nearPos;
  fmat outValue_m = fmat(time_v.n_rows,this->getDim());
  for (int i=0; i<time_v.n_rows;i++){
    nearPos = this->getNearestTimePosition(this->time_v(i),0);
    cout<<"nearPos : " << nearPos<<endl;
    for (int j=0; j<this->getDim(); j++){
        outValue_m(i,j) = this->value_m(nearPos,j);
        cout<<"j : " << j<<endl;
    }
  }

  /*for (int i=0; i<this->getLength();i++){
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


fmat ACMediaTimedFeatures::weightedMean(ACMediaTimedFeatures* weight){  
  fmat weightVal = weight->getValueAtTime(this->getTime());
  float sumWeight = conv_to<float>::from(sum(weightVal));
  weightVal = weightVal / sumWeight;
  return trans(weightVal) * this->getValue() ;
}

fmat ACMediaTimedFeatures::mean(){
  fcolvec weightValue = ones<fcolvec>(this->getLength());
  ACMediaTimedFeatures* weight = new ACMediaTimedFeatures(this->getTime(),weightValue);
  return weightedMean(weight);
}

vector< float > ACMediaTimedFeatures:: meanAsVector(){
  fmat tmpMean = this->mean();
  vector< float > meanVec;
  for (int i=0; i<tmpMean.n_cols; i++){
    meanVec.push_back(tmpMean(0,i));
  } 
  return meanVec;
}

fmat ACMediaTimedFeatures::weightedStdDeviation(ACMediaTimedFeatures* weight){
}


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
    //    cout << "left : " << leftPos << endl;
    // cout << "right : " << rightPos << endl;
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
  weightValue = weightValue / sum(weightValue);
  ACMediaTimedFeatures* weight = new ACMediaTimedFeatures(this->getTime(),weightValue);
  return weightedMeanSegment(weight);
}

ACMediaTimedFeatures* ACMediaTimedFeatures::weightedMeanSegment(ACMediaTimedFeatures* weight){
  vector<ACMediaTimedFeatures*> acmtv;
  acmtv = this->segment();
  ACMediaTimedFeatures *outAcmtv;
  fmat tmpMean_m(1, (int)this->getDim());
  //  cout << (int) this->getSegments().size() << endl;
  fcolvec outAcmtvTime((int)this->getSegments().size()-1);
  fmat outAcmtvValue((int)this->getSegments().size()-1,(int)this->getDim());
  
  for (int i=0; i<acmtv.size(); i++){
    tmpMean_m = acmtv[i]->weightedMean(weight);//1 line * dim column
    for (int Icol=0; Icol<this->getDim(); Icol++)
      outAcmtvValue(i, Icol) = tmpMean_m(0,Icol);
    //    outAcmtvTime(i) = (this->getSegments()[i]+this->getSegments()[i+1])/2;
    outAcmtvTime(i) = (acmtv[i]->getTime()(0) + acmtv[i]->getTime()(acmtv[i]->getLength()-1) )/2;
    //    cout << "Time" << endl;
    //cout << outAcmtvTime(i) << endl;
  }
  outAcmtv = new ACMediaTimedFeatures(outAcmtvTime, outAcmtvValue);
  outAcmtv->setName("raf");
  return outAcmtv;
}

// ACMediaTimedFeatures* ACMediaTimedFeatures::simpleSplineModeling(ACMediaTimedFeatures* weight, ACMediaSegment* segment){
//}

void ACMediaTimedFeatures::readFile(std::string fileName){
    fmat data;
    data.load(fileName, raw_ascii);
    this->setTime(data.col(0));          //time information : first column of the file
    this->setValue(data.cols(1,(data.n_cols)-1));   //data : other columns of the file
    //value_m.print();
    //time_v.print();
    //fmat result = mean(value_m,0);
    //result.print("Mean arma");
}

