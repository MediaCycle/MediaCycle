
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
#include "Armadillo-utils.h"

#include "boost/filesystem.hpp"

// USE_SDIF is off by default :
#ifdef USE_SDIF
#include "sdif.h"
#endif

using namespace arma;
using namespace std;

ACMediaTimedFeature::ACMediaTimedFeature(){
}

ACMediaTimedFeature::ACMediaTimedFeature(long iSize, int iDim, string name){
	this->time_v.set_size(iSize);
	this->value_m.set_size(iSize, iDim);
	this->name = name;
}

ACMediaTimedFeature::~ACMediaTimedFeature(){
	seg_v.clear();
	name.clear();
}

ACMediaTimedFeature::ACMediaTimedFeature(fcolvec time_v, fmat value_m){
	this->time_v = time_v;
	this->value_m = value_m;
}

ACMediaTimedFeature::ACMediaTimedFeature(fcolvec time_v, fmat value_m, string name, vector<float> *seg_v){
	this->time_v = time_v;
	this->value_m = value_m;
	this->name = name;
	if (seg_v !=0) this->seg_v = *seg_v;	
}

ACMediaTimedFeature::ACMediaTimedFeature( const vector<float> &time, const vector< vector<float> > &value, string name, const vector<float> *seg_v){
	if (time.size() != value.size()) {
		cerr << "<ACMediaTimedFeature::ACMediaTimedFeature> Time and value vectors do not have the same size" << endl;
		exit(-1);
	}
	this->time_v = fcolvec(time.size());
	this->value_m = fmat((int) value.size(), (int) value[0].size());
	for (unsigned int Itime=0; Itime<time.size(); Itime++){
		this->time_v(Itime) = time[Itime];
		for (unsigned int Idim=0; Idim<value[0].size(); Idim++){
			this->value_m(Itime, Idim) = value[Itime][Idim];
		}
	}
	this->name = name;
	if (seg_v !=0) this->seg_v = *seg_v;	
}

ACMediaTimedFeature::ACMediaTimedFeature(const vector<float> &time, const vector<float> &value, string name, const vector<float> *seg_v){
	unsigned int ts=time.size();
	if (ts != value.size()) {
		cerr << "<ACMediaTimedFeature::ACMediaTimedFeature> Time and value vectors do not have the same size" << endl;
		exit(-1);
	}
	
	this->time_v = fcolvec(ts);
	this->value_m = fmat((int) ts,1);
	for (unsigned int Itime=0; Itime<ts; Itime++){
		this->time_v(Itime) = time[Itime];
		this->value_m(Itime, 0) = value[Itime]; // XS 0 pas 1
	}
	this->name = name;
	if (seg_v !=0) this->seg_v = *seg_v;	
}

// ***
// XS 06/11/09 : commented this one because confusion in ordering of value 
// i.e.: columns and rows are inversed compared to previous constructor !

//ACMediaTimedFeature::ACMediaTimedFeature( float *time, int length, float **value, int dim, string name, vector< float > seg_v ){
//	this->time_v = fcolvec( length );
//	// value is assumed to be dim (first dim) by length (2nd dim) 
//	this->value_m = fmat( (int) length, (int) dim );
//	for (int Itime=0; Itime<length; Itime++){
//		this->time_v(Itime) = time[Itime];
//		for (int Idim=0; Idim<dim; Idim++){
//			this->value_m(Itime, Idim) = value[Idim][Itime];
//		}
//	}
//	this->name = name;
//	//vector< float > seg_v( seg_fp, seg_fp + nBursts );
//	this->seg_v = seg_v;
//}


fcolvec ACMediaTimedFeature::getTime() {
	return time_v;
}

float ACMediaTimedFeature::getTime(int index) {
	return time_v(index);
}

vector<float> ACMediaTimedFeature::getSegments(){
	return seg_v;
}

float ACMediaTimedFeature::getSegments(int index){
	return seg_v[index];
}

int ACMediaTimedFeature::getNearestTimePosition(float time, int mode){
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

void ACMediaTimedFeature::setTime(fcolvec time_v) {
	this->time_v = time_v;
}

double* ACMediaTimedFeature::getTimeAsDouble() {
	double* timeDouble;
	timeDouble = new double[this->getLength()];  
	for (int i=0; i < this->getLength(); i++) {
		timeDouble[i] = this->time_v(i);
	}
	return timeDouble;
}

vector<float> ACMediaTimedFeature::getTimeAsVector() {
	vector<float> time;
	for (int i=0; i<this->getLength(); i++)
		time.push_back(this->getTime()(i));
	return time;
}


fmat ACMediaTimedFeature::getValue() {
	return value_m;
}

float ACMediaTimedFeature::getValue(float index, float dim){
	return this->value_m(index, dim);
}

// XS TODO return value
vector< vector<float> > ACMediaTimedFeature::getValueAsVector(){
	// XS TODO duh ?
}

void ACMediaTimedFeature::setValue(fmat value_m) {
	this->value_m = value_m;
}

void ACMediaTimedFeature::setTimeAndValueForIndex(long iIndex, double iTime, frowvec iVal_v) {
	if (iVal_v.n_cols != this->getDim()){
		std::cerr << "setValueForTime : Wrong dimension" << std::endl;
		exit(1);
	}
	this->time_v(iIndex) = iTime;
	this->value_m.row(iIndex) = iVal_v;
}

void ACMediaTimedFeature::setTimeAndValueForIndex(long iIndex, double iTime, vector<float> iVal) {
	if (iVal.size() != this->getDim()){
		std::cerr << "setValueForTime : Wrong dimension" << std::endl;
		exit(1);
	}
	this->time_v(iIndex) = iTime;
	for (int i=0; i < this->getDim(); i++)
		this->value_m(iIndex, i) = iVal[i];
}

size_t ACMediaTimedFeature::getLength() {
	return time_v.n_rows;
}

size_t ACMediaTimedFeature::getDim() {
	return value_m.n_cols;
}


fmat ACMediaTimedFeature::getValueAtTime(fcolvec iTime_v) {
	// return interpolated values at time_v
	
	int nearPosL, nearPosR;
	float v1, v2, t1, t2, a, b;
	fmat outValue_m = fmat(iTime_v.n_rows,this->getDim());
	for (int i=0; i < iTime_v.n_rows; i++){
		nearPosL = this->getNearestTimePosition(iTime_v(i),0);
		nearPosR = this->getNearestTimePosition(iTime_v(i),1);
		if (nearPosR == nearPosL){
			outValue_m.row(i) = this->value_m.row(nearPosL);
		}
		else{
			t1  = this->time_v(nearPosL);
			t2  = this->time_v(nearPosR);
			for (int j=0; j<this->getDim(); j++){
				v1  = this->value_m(nearPosL,j);
				v2  = this->value_m(nearPosR,j);
				a = (v1-v2)/(t1-t2);
				b = v1 - a*t1;
				outValue_m(i,j) = a * iTime_v(i) + b;
			}
		}
	}
	
	return outValue_m;
}

ACMediaFeatures* ACMediaTimedFeature::weightedMean(ACMediaTimedFeature* weight){
	ACMediaFeatures* weightedMean_mf = new ACMediaFeatures();
	fmat weightVal = weight->getValueAtTime(this->getTime());
	float sumWeight = as_scalar(sum(weightVal));
	fmat tmp_m;
	string name = "Weighted mean of ";
	weightVal = weightVal / sumWeight;
	tmp_m = trans(weightVal) * this->getValue();
	if (! tmp_m.is_finite()){
		std::cout<<"Weighted not finite" << std::endl;
		exit(1);
	}
	name += this->getName();
	weightedMean_mf->setName(name);
	for (int i=0; i<tmp_m.n_cols; i++)
		weightedMean_mf->addFeatureElement(tmp_m(0,i));
	return weightedMean_mf;
}

ACMediaFeatures* ACMediaTimedFeature::mean(){
	ACMediaFeatures* mean_mf = new ACMediaFeatures();  
	fmat mean_m = arma::mean(this->getValue());
	string name = "Mean of ";
	name += this->getName();
	mean_mf->setName(name);
	for (int i=0; i<mean_m.n_cols; i++){
		mean_mf->addFeatureElement(mean_m(0,i)); // 0 = per column
	}
	return mean_mf;
}

ACMediaFeatures* ACMediaTimedFeature::max(){
	ACMediaFeatures* max_mf = new ACMediaFeatures();  
	fmat max_m = arma::max(this->getValue());
	string name = "Max of ";
	name += this->getName();
	max_mf->setName(name);
	for (int i=0; i<max_m.n_cols; i++){
		max_mf->addFeatureElement(max_m(0,i)); // 0 = per column
	}
	return max_mf;
}

// vector< float > ACMediaTimedFeature:: meanAsVector(){
//   fmat tmpMean = this->mean()->getValue();
//   vector< float > meanVec;
//   for (int i=0; i<tmpMean.n_cols; i++)
//     meanVec.push_back(tmpMean(0,i));
//   return meanVec;
// }

ACMediaFeatures* ACMediaTimedFeature::weightedStdDeviation(ACMediaTimedFeature* weight){
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
	string name = "Weighted standard deviation of ";
	name += this->getName();
	wstd_mf->setName(name);
	for (int i=0; i<wstd_m.n_cols; i++)
		wstd_mf->addFeatureElement(wstd_m(0,i));
	return wstd_mf;
}

ACMediaFeatures* ACMediaTimedFeature::std(){  
	fmat std_m = stddev(this->getValue());
	string name = "Standard deviation of ";
	ACMediaFeatures* std_mf = new ACMediaFeatures();
	name += this->getName();
	std_mf->setName(name);
	for (int i=0; i<std_m.n_cols; i++)
		std_mf->addFeatureElement(std_m(0,i));  
	return std_mf;
}


ACMediaFeatures* ACMediaTimedFeature::toMediaFeatures(){
	if (this->getDim() > 1){
		std::cerr << "Error : toMediaFeatures can only be applied to unidimensionnal features" << std::endl;
		exit(1);
	}
	else{
		ACMediaFeatures* desc_mf = new ACMediaFeatures();
		desc_mf->setName(this->getName());
		for (int i=0; i < this->getLength(); i++)
			desc_mf->addFeatureElement(value_m(i,0));  
		return desc_mf;
	}	
}
// vector< float > ACMediaTimedFeature:: stdAsVector(){
//   fmat tmpStd = this->std();
//   vector< float > stdVec;
//   for (int i=0; i<tmpStd.n_cols; i++)
//     stdVec.push_back(tmpStd(0,i));
//   return stdVec;
// }

ACMediaTimedFeature* ACMediaTimedFeature::delta(){
	ACMediaTimedFeature* deltaf;
	if (this->getLength() > 2){
		deltaf = new ACMediaTimedFeature(this->time_v.rows(1,this->getLength()-1), diff(this->value_m, 1, 0), "Delta "+this->getName());
	}
	else{
		deltaf = new ACMediaTimedFeature(this->time_v, zeros<fmat>(this->getLength(), this->getDim()), "Delta "+this->getName());
	}
	return deltaf;
}

vector<ACMediaTimedFeature*> ACMediaTimedFeature::segment(){
	vector<ACMediaTimedFeature*> acmfSegmented;
	ACMediaTimedFeature* tmpAtmf;
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
		
		tmpAtmf = new ACMediaTimedFeature(this->time_v.rows(leftPos,rightPos), this->value_m.rows(leftPos, rightPos));
		acmfSegmented.push_back(tmpAtmf);
	}  
	return acmfSegmented;
}

ACMediaTimedFeature* ACMediaTimedFeature::meanSegment(){
	fcolvec weightValue = ones<fcolvec>(this->getLength());
	//weightValue = weightValue / sum(weightValue);   //unecessary at this time?
	ACMediaTimedFeature* weight = new ACMediaTimedFeature(this->getTime(),weightValue);
	return weightedMeanSegment(weight);
}

ACMediaTimedFeature* ACMediaTimedFeature::weightedMeanSegment(ACMediaTimedFeature* weight){
	vector<ACMediaTimedFeature*> acmtv;
	acmtv = this->segment();
	ACMediaTimedFeature *outAcmtv;
	ACMediaFeatures* tmpMean_mf = new ACMediaFeatures;
	//  cout << (int) this->getSegments().size() << endl;
	fcolvec outAcmtvTime((int)this->getSegments().size()-1);
	fmat outAcmtvValue((int)this->getSegments().size()-1,(int)this->getDim());
	
	for (int i=0; i<acmtv.size(); i++){
		//acmtv[i]->getTime().print("time seg");    
		tmpMean_mf = acmtv[i]->weightedMean(weight);//1 line * dim column
		for (int Icol=0; Icol<this->getDim(); Icol++)
			outAcmtvValue(i, Icol) = tmpMean_mf->getFeatureElement(Icol);
		//outAcmtvTime(i) = (this->getSegments()[i]+this->getSegments()[i+1])/2;
		outAcmtvTime(i) = (acmtv[i]->getTime(0) + acmtv[i]->getTime(acmtv[i]->getLength()-1) )/2;
	}
	outAcmtv = new ACMediaTimedFeature(outAcmtvTime, outAcmtvValue);
	//outAcmtv->setName("raf");   //??
	delete(tmpMean_mf);
	return outAcmtv;
}

ACMediaTimedFeature* ACMediaTimedFeature::interpN(int n){
	ACMediaTimedFeature* interp_amtf;
	float minTime = min(time_v);
	float maxTime = arma::max(time_v);
	fcolvec newTime_v = linspace<fcolvec>(minTime, maxTime, n);
	fmat newVal_m = this->getValueAtTime(newTime_v);
	interp_amtf = new ACMediaTimedFeature(newTime_v, newVal_m, "Interpolated "+this->getName());
	return interp_amtf;
}

ACMediaFeatures* ACMediaTimedFeature::temporalModel(double start_sec, double stop_sec){
	
	if (value_m.n_cols > 1){
		std::cout << "error : simpleSegmentModel can only be applied to unidimensional features." << std::endl;
		exit(1);
	}

	ACMediaFeatures* temporalModel_mf = new ACMediaFeatures();
	fcolvec value_v;
	long start_sp = this->getNearestTimePosition(start_sec,2);
	long stop_sp = this->getNearestTimePosition(stop_sec,2);
	value_v = value_m.col(0);
	value_v = value_v.rows(start_sp, stop_sp);
	
	fcolvec time2_v = time_v.rows(start_sp, stop_sp);
	
	unsigned long lMaxp = max_index(value_v);
	unsigned long lMinp = min_index(value_v);
	rowvec seg1_v(4);
	rowvec seg_v(3);

	seg1_v(0) = 0;
	seg1_v(1) = lMinp;
	seg1_v(2) = lMaxp;
	seg1_v(3) = value_v.n_rows-1;
	seg1_v = sort(seg1_v);
	
	rowvec seglength_v = diff(seg1_v,1,1);
	unsigned long minSegLength_pos = min_index(seglength_v);
	
	seg_v(0) = seg1_v(0);
	seg_v(2) = seg1_v(3);
	
	if (minSegLength_pos == 0){
		seg_v(1) = seg1_v(2);
	}
	else if (minSegLength_pos == 1){
		seg_v(1) = seg1_v(1);	
	}
	else if (minSegLength_pos == 2){
		seg_v(1) = seg1_v(1);
	}

	float d1 = time2_v(seg_v(1)) - time2_v(seg_v(0));
	float s1;
	if (d1 > 0)
		s1 = (value_v(seg_v(1)) - value_v(seg_v(0)))/d1;
	else
		s1 = 0;
	
	float d2 = time2_v(seg_v(2)) - time2_v(seg_v(1));
	float s2;
	if (d2 > 0)
		s2 = (value_v(seg_v(2)) - value_v(seg_v(1))) / d2;
	else
		s2 = 0;

	float slopeDiff = s2-s1;
	//	float sa, sd;
	frowvec sm_v(5);
	if (s1 > 0){ // if the first segment is ascending
		sm_v(0) = s1; //sa = s1; // slope ascending part
		sm_v(1) = d1; //da = d1; // duration ascending part
		sm_v(2) = s2; //sd = s2; // slope descending
		sm_v(3) = d2; //dd = d2; // duration descending
		sm_v(4) = slopeDiff; //dd = d2; // duration descending
	}
	else{
		sm_v(0) = s2; //sa = s2; 
		sm_v(1) = d2; //da = d2;
		sm_v(2) = s1; //sd = s1;
		sm_v(3) = d1; //dd = d1;
		sm_v(3) = slopeDiff; //dd = d1;
	}
	std::cout << "temporal model : " << sm_v << std::endl;
	if (!sm_v.is_finite()){
		std::cout << "Temporal model is not finite" << std::endl;
		exit(1);
	}
	
	string name = "Temporal Model of ";
	name += this->getName();
	temporalModel_mf->setName(name);
	for (int i = 0; i < sm_v.n_cols; i++){
		temporalModel_mf->addFeatureElement(sm_v(i)); 
	}
	return temporalModel_mf;
}

fmat ACMediaTimedFeature::similarity(int mode)
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

// n1 and n2 can be different if two media have different duration
// but at each time stamp the same features have to be extracted
fmat ACMediaTimedFeature::similarity(ACMediaTimedFeature* B){
	int n1 = this->getLength();
	int n2 = B->getLength();
	
	fmat similarityM = zeros<fmat>(n1,n2); // NB: non symmetric
    for (int i1 = 0; i1 < n1; i1++) {
		for (int i2 = 0; i2 < n2; i2++) {
			fmat xtmp1 = square(this->getValue().row(i1) - B->getValue().row(i2));
			// XS for 1-D features it amounts to:
			// float xtmp2=pow((this->getValue(i1,0) - B->getValue(i2,0)),2);
			similarityM(i1,i2) = accu(xtmp1);;
		}
	}
	return similarityM;
}


float ACMediaTimedFeature::dist(fmat vector1, fmat vector2, int mode)
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

// XS new 180111
// concatenate with another timed Feature
// XS TODO: here I just check that the time vectors have same lenght; must have the same time stamps
bool ACMediaTimedFeature::appendTimedFeature(ACMediaTimedFeature* B){
	bool append_ok;
	if (this->getTime().n_rows != B->getTime().n_rows){
		append_ok = false;
	}
	else {
		this->setValue( join_rows(this->getValue(),B->getValue()) );
		append_ok = true;
	}
	return append_ok;
}

// --------------------------------------------------------------------
// I/O

int ACMediaTimedFeature::readFile(std::string fileName){
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

void ACMediaTimedFeature::dump(){ // in cout
	cout << "ACMediaTimedFeature named " << this->getName() << endl;
	fmat data = join_rows(this->getTime(), this->getValue());
	data.print();
	cout << "--------------------------" << endl;
}

//XS new
bool ACMediaTimedFeature::saveInFile(string _fname, bool _binary){ 
	bool save_ok = false;
	fmat data = join_rows(this->getTime(), this->getValue());
	if (_binary){
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			save_ok = data.save(_fname, arma_binary); // default format = arma_binary
		#else
			data.save(_fname, arma_binary); // default format = arma_binary
			if ( boost::filesystem::exists( _fname ) )
				save_ok = true;
		#endif
	}
	else {
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			save_ok = data.save(_fname, arma_ascii);
		#else
			data.save(_fname, arma_ascii);
			if ( boost::filesystem::exists( _fname ) )
				save_ok = true;
		#endif	
	}
	return save_ok;
}

//XS new
bool ACMediaTimedFeature::loadFromFile(string _fname, bool _binary){
	fmat tmp_m;
	bool load_ok = false;
	if (_binary){
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			load_ok = tmp_m.load(_fname, arma_binary); // default format = arma_binary
		#else
			tmp_m.load(_fname, arma_binary); // default format = arma_binary
			if ( tmp_m.n_rows > 0 && tmp_m.n_cols > 0 )
				load_ok = true;
		#endif
	}
	else {
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			load_ok = tmp_m.load(_fname, arma_ascii);
		#else
			tmp_m.load(_fname, arma_ascii);
			if ( tmp_m.n_rows > 0 && tmp_m.n_cols > 0 )
				load_ok = true;
		#endif
	}
	
	if (load_ok) { 
		//  tmp_m.print();
		this->value_m = tmp_m.cols(1,tmp_m.n_cols-1);
		this->time_v = tmp_m.col(0);
	}
	return load_ok;
}


int ACMediaTimedFeature::saveAsTxt(string fname){ // in file
	fmat data = join_rows(this->getTime(), this->getValue());
	ofstream library_file (fname.c_str());
	library_file << this->getName() << endl;
	library_file << this->getLength() << " " << this->getDim() << endl;
	library_file << data << endl;
	library_file.close();
	return 1;
}

//deprecated
void ACMediaTimedFeature::importFromFile(string filename){
	this->loadFromFile(filename);
}

void ACMediaTimedFeature::importSegmentsFromFile(string filename){
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


// ------------------------------------------------------------------------
// by default USE_SDIF is off
#ifdef USE_SDIF
int ACMediaTimedFeature::saveAsSdif(const char *name){
	SdifGenInit ("");
	SdifFileT *file = SdifFOpen(name, eWriteFile);  /* Open file for writing */
	/* Here, you could add some text data to the name-value tables */
	SdifFWriteGeneralHeader  (file);    /* Write file header information */
	SdifFWriteAllASCIIChunks (file);    /* Write ASCII header information */
	
	SdifUInt4 SizeFrameW = 0;
	int mystream = 1;
	
	
	for (int t=0; t < this->getLength(); t++){
		SdifFSetCurrFrameHeader (file, SdifSignatureConst('1', 'F', 'Q', '0'), _SdifUnknownSize, 1, mystream, this->time_v(t));
		SizeFrameW += SdifFWriteFrameHeader (file);
		
		/* Write matrix header */
		SdifFSetCurrMatrixHeader (file, SdifSignatureConst('1','F','Q','0'), eFloat4, 1, this->getDim());
		SizeFrameW += SdifFWriteMatrixHeader (file);
		
		/* Write matrix data */
		for (int col = 1; col <= this->getDim(); col++){
			SdifFSetCurrOneRowCol (file, col, this->value_m(t,col-1));
		}
		SizeFrameW += SdifFWriteOneRow (file);
		/* Write matrix padding */
		SizeFrameW += SdifFWritePadding (file, SdifFPaddingCalculate (file->Stream, SizeFrameW));
		
		SdifUpdateChunkSize (file, SizeFrameW - 8);  /* Set frame size */
	}
	SdifFClose (file);
	SdifGenKill ();
	return 1;
}
#endif

arma::fmat vectorACMTF2fmat(std::vector <ACMediaTimedFeature*> _ACMTF)
{
    int i, n_ACMTF, n_rows=_ACMTF[0]->getLength(), n_cols=0;
    arma::fmat features_m;
    for(i=0;i<_ACMTF.size();i++)
    {
        //cout << "Feature: " << _ACMTF[i]->getName() << "; Nframes: " << _ACMTF[i]->getLength() << "; Size: " << _ACMTF[i]->getDim() << endl;
        n_cols+=_ACMTF[i]->getDim();
        if(_ACMTF[i]->getLength() > n_rows)
        {
            n_rows=_ACMTF[i]->getLength();
            //cout << "WARNING: ALL features do not have the same number of frames!!!" << endl;
        }
    }
    //features_m.zeros(n_rows,n_cols);
    cout << "Full matrix: Nrows: " << n_rows << "; n_cols: " << n_cols << endl;

    int p=0;
    while(_ACMTF[p]->getLength()<n_rows)
    {
        p++;
    }
    features_m=_ACMTF[p]->getTime();

    /*if(_ACMTF[0]->getLength()<n_rows)
    {
        arma::fmat tmp;
        tmp.zeros(n_rows-_ACMTF[0]->getLength(),_ACMTF[0]->getDim());
        features_m=arma::join_cols(tmp,_ACMTF[0]->getValue());
    }
    else
    {
        features_m=_ACMTF[0]->getValue();
    }*/
	for(i=0;i<_ACMTF.size();i++)
	{
        if(_ACMTF[i]->getLength()<n_rows)
        {
            cout << "WARNING: ALL features do not have the same number of frames!!! " << n_rows-_ACMTF[i]->getLength() << " zero rows added at the beginning of feature: " << _ACMTF[i]->getName() << endl;
            arma::fmat tmp;
            tmp.zeros(n_rows-_ACMTF[i]->getLength(),_ACMTF[i]->getDim());
            features_m=arma::join_rows(features_m,arma::join_cols(tmp,_ACMTF[i]->getValue()));
        }
        else
        {
            features_m=arma::join_rows(features_m,_ACMTF[i]->getValue());
        }
	}
	
    return features_m;
}
