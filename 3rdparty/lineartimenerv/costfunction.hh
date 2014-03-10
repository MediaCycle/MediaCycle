/*Copyright (C) Kristian Nybo, Jarkko Venna, Jaakko Peltonen
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef COSTFUNCTION_HH
#define COSTFUNCTION_HH

#include "datamatrix.hh"
#include <string>

class CostFunction
{
protected:
  //  Matrix gradient;
public:

  /* Evaluates the gradient at data and returns the result in target. */

  virtual double getGradient (const DataMatrix & currentparams, DataMatrix & target) =
    0;
  /* Evaluates the cost function at currentparams. */

  virtual double evaluate (const DataMatrix & currentparams) = 0;

  virtual ~ CostFunction ()
  {
  }

  /* Reports the parameters of the cost function as a string. */
  virtual void reportParameters(std::string& target) = 0;

  /*Updates any parameters that change as the optimization progresses */

  virtual void updateDynamicParameters (size_t currentRound,
                                        size_t totalRounds,
                                        const DataMatrix & projData) = 0;

  virtual void updateDataRepresentation (const DataMatrix & newData) = 0;

};

#endif
