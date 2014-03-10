/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "goldensectionsearch.hh"
#include "costfunction.hh"
#include "conjugategradientopt.hh"
#include "distancedata.hh"

#include <iostream>

#include <math.h>


class testFunction:public CostFunction
{
public:

  /* Evaluates the gradient at data and returns the result in target. */

  virtual void getGradient (Matrix & data, Matrix & target);

  /* Evaluates the cost function at data. */

  virtual double evaluate (Matrix & data);

  virtual void updateDynamicParameters (size_t currentRound,
                                        size_t totalRounds)
  {
  }

  virtual void updateDataRepresentation (Matrix & newData)
  {
  }
};


double
testFunction::evaluate (Matrix & data)
{
  return sin (data (0, 0)) + sin (data (0, 1));
}


void
testFunction::getGradient (Matrix & data, Matrix & target)
{
  target (0, 0) = cos (data (0, 0));
  target (0, 1) = cos (data (0, 1));
}


int
main (void)
{
  testFunction func;
  GoldenSectionSearch linesearch;
  ConjugateGradientOpt opt (func, linesearch);

  Matrix piste (1, 2);

  piste (0, 0) = 1.0;
  piste (0, 1) = -1.0;

  opt.setIterationsPerStep (20);

  opt.perform (piste);

  std::cout << "The minimum is at (" << piste (0, 0) << "," << piste (0, 1)
  << ").\n";

  return 0;
}
