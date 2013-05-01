/* 
 * File:   main.cpp
 * Author: jerome
 *
 * @date February 15, 2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include <stdlib.h>
#include <string>
#include <mds.h>
#include <Isomap.h>
#include <Sne.h>

#include "Knn.h"
#include "ClassificatorErrorMeasure.h"
//#include "Trustworthiness.h"

#include "Batch.h"
using namespace std;
using namespace arma;

class testStatic{
private:
    static int *wq;
public:
    static int *getWq(){return wq;};
    static void setWq(int *param){wq=param;};
};

/*
 * 
 */
int main(int argc, char** argv) {
    
   
    double test=1e39;
    double test2=1e40;
    
    bool testb=1e35<test2;
    double aux_mem[100]={
        0  , 11.7898 ,  56.2228  , 39.4081  , 16.7929 ,  73.6614  , 20.5426 ,  92.2822 , 116.4388 ,  18.7883,
        11.7898    ,     0,   67.0969 ,  32.1559  ,  6.7082 ,  73.5187  ,  9.0000  , 99.0505 , 123.3734,    8.3666,
        56.2228 ,  67.0969 ,        0  , 80.6350,   69.4910 , 102.3474  , 74.3572 ,  56.8946 ,  85.0470 ,  73.6071,
        39.4081  , 32.1559 ,  80.6350,         0  , 27.7308  , 73.6817 ,  27.0370   ,92.7416 , 116.1508   ,34.1174,
        16.7929  ,  6.7082 ,  69.4910,   27.7308  ,       0 ,  77.1362  ,  5.0990  , 98.5596 , 124.5231  ,  8.0623,
        73.6614  , 73.5187  ,102.3474 ,  73.6817 ,  77.1362     ,    0  , 75.9342  ,119.4069 , 112.1873   ,78.7718,
        20.5426 ,   9.0000   ,74.3572  , 27.0370   , 5.0990  , 75.9342,         0 , 102.7716,  127.8280 ,   7.1414,
        92.2822  , 99.0505 ,  56.8946  , 92.7416  , 98.5596 , 119.4069  ,102.7716  ,       0  , 49.9800 , 105.7592,
        116.4388 , 123.3734 ,  85.0470,  116.1508,  124.5231 , 112.1873 , 127.8280 ,  49.9800   ,      0 , 131.3431,
        18.7883  ,  8.3666  , 73.6071 ,  34.1174  ,  8.0623 ,  78.7718  ,  7.1414 , 105.7592 , 131.3431  ,       0};
    
    mat H(aux_mem, 10, 10, false);
    cout<<"H:"<<endl<<H<<endl;
    rowvec testVec= H.row(2);
    rowvec testVec2=H.row(4);
    
    testVec.subvec(0,2)=testVec2.subvec(3,5);
    cout<<"testVec:"<<testVec<<endl;
    
    
    double aux_mem2[60] ={
        1  ,   4  ,   6   , 35  ,   8   , 2.5  ,   8  ,  61  ,  76 ,    1,
        10  ,   7  ,  10  ,   4  ,   2  ,  77  ,   3  ,   6  ,  53  ,   2,
        20  ,   9  ,  76  ,   1  ,   7  ,   1   ,  2  ,  90  ,  98 ,    3,
        4  ,   5  ,   10   , 30  ,   6  , 10  ,   8  ,  2  ,  81 ,    1,
        15  ,   6  ,  40  ,   4.2  ,   2.5  ,  77  ,   34  ,   6  ,  10  ,   2,
        20  ,   9  ,  71  ,   1  ,   43  ,   1   ,  2  ,  92  ,  91 ,    2.2};
    mat X(aux_mem2, 20, 3, false);
    uword aux_mem3[40] ={
        0,1,2,3,7,8,10,12,15,18,20,22,24,33,36,38,41,43,45,48,0,1,2,0,2,2,1,1,2,1,2,1,1,1,0,0,1,0,1,2};
    umat L(aux_mem3,20, 2, false);
    
    
    cout<<"L:"<<L<<endl;
   // Trustworthiness algo;
    
    ucolvec Ltemp=L.col(1);
    /*
    algo.setFeatureMatrixHighDim(X);
    algo.setFeatureMatrixLowDim(X);
    //algo.setDistanceMatrix(H);
    cout<<algo.compute(10)<<endl;*/
   // cout<<"result:"<<endl;
     //   cout<<result;}
    return (EXIT_SUCCESS);
}
