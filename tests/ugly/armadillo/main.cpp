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

using namespace std;
using namespace arma;


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
    double aux_mem2[30] ={
        1  ,   4  ,   6   , 35  ,   8   , 25  ,   8  ,  61  ,  76 ,    1,
        10  ,   7  ,  10  ,   4  ,   2  ,  77  ,   3  ,   6  ,  53  ,   2,
        20  ,   9  ,  76  ,   1  ,   7  ,   1   ,  2  ,  90  ,  98 ,    3};
    mat X(aux_mem2, 10, 3, false);
    cout<<"X:"<<X<<endl;
    Isomap algo;
    algo.setFeatureMatrix(X,'k',2);
   // algo.setDistanceMatrix(H);
    mat result=algo.compute(2);
    cout<<"result:"<<endl;
    cout<<result;
    return (EXIT_SUCCESS);
}
