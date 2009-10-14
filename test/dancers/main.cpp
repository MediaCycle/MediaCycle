/* 
 * File:   main.cpp
 * Author: Alexis Moinet
 *
 * @date 15 juillet 2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#include "MediaCycle.h"

#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <math.h>
//#include "Armadillo-utils.h"
//#include "fftsg_h.c"
#include <time.h>

using namespace std;

int main(int argc, char** argv) {
  //     MediaCycle *mediacycle;
  // 	mediacycle = new MediaCycle(MEDIA_TYPE_VIDEO);
  //     //string libpath("/home/alexis/NetBeansProjects/MediaCycle/lib/Caltech101-a.acl");
  
  //     cout<<"new MediaCycle"<<endl;
  //     mediacycle->addPlugin("/Users/dtardieu/src/Numediart/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/eyesweb/Debug/mc_eyesweb.dylib");
  //     mediacycle->importDirectory("/Users/dtardieu/data/DANCERS/Video/Front/",0);
  //     mediacycle->saveAsLibrary("/Users/dtardieu/data/DANCERS/dancers.acl");
  //     return (EXIT_SUCCESS);
  //   mat A = rand<mat>(64,64);
  //   mat B = rand<mat>(500,500);
  //   mat C(10,10);
  
  //   C = conv2(B,A);
  //   C.save("C.txt", arma_ascii);
  MediaCycle *media_cycle = new MediaCycle(MEDIA_TYPE_VIDEO,"/tmp/","mediacycle.acl");
  media_cycle->openLibrary("/Users/dtardieu/Desktop/dancers-dt-3.acl");
  
  ACNavigationState state;
  state = media_cycle->getBrowser()->getCurrentNavigationState();
  std::cout << state.mSelectedLoop << std::endl;
  std::cout << state.mNavigationLevel << std::endl;
  
  ACLoopAttribute loopAttribute;
  loopAttribute = media_cycle->getLoopAttributes(0);
  
  mat A,B,C;
  //  D.load("sin.txt");
  //D.print();
  C = randn<mat>(512,2);
  A = randn<mat>(512,2);
  //A.load("A.txt", raw_ascii);
  B = randn<mat>(64,64);
  //B = ones<mat>(4,4);
  A.save("A.txt", arma_ascii);
  B.save("B.txt", arma_ascii);
  
  vector<double> Avec1;
  vector<double> Avec2;
  
  for (int d1=0; d1 < A.n_rows; d1++){
    Avec1.push_back(A(d1,0));
    Avec2.push_back(A(d1,1));
  }
  clock_t start = clock();
  for (int d1=0; d1 < A.n_rows; d1++){
    C(d1,0) = Avec1[d1];
    C(d1,1) = Avec2[d1];
  }
  std::cout << "Time elapsed: " << ((double)clock() - start) / CLOCKS_PER_SEC << std::endl;

//C.save("C.txt", arma_ascii);
  
//   A.set_size(1,1);
//   A(0,0)=1;
//   C.set_size(1,1);
//   C = conv2(A,A);
//   C.print("C");
}
