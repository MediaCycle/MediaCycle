/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef NUMERICALGRADIENTCOMPUTER_HH
#define NUMERICALGRADIENTCOMPUTER_HH

#include "datamatrix.hh"
#include "costfunction.hh"

class NumericalGradientComputer
{
public:
  void compute (CostFunction & function, DataMatrix & data, double delta,
                DataMatrix & gradient);
};

#endif
