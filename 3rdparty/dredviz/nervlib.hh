/*Copyright (C) Kristian Nybo, Jarkko Venna
 *Modified for tcNeRV by Jaakko Peltonen
 *Modified for lineartime nerv by Konstantinos Georgatzis
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include <armadillo>

#ifndef NERV
#define NERV
arma::mat nerv(arma::mat desc_m, double lambda, size_t lastNeighbor, size_t iterations, size_t stepsPerRound, size_t stepsOnLastRound);
#endif
