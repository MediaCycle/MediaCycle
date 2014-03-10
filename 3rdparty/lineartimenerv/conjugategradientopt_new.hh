/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef CONJUGATEGRADIENTOPT_HH
#define CONJUGATEGRADIENTOPT_HH

#include "optimizationstepbatch.hh"
#include "distancedata.hh"
#include "dataset.hh"
#include "costfunction.hh"
#include "linesearch.hh"

#include <iostream>


#define DEFAULT_ITERATIONS 5

class ConjugateGradientOpt:public OptimizationStepBatch
{
private:
  size_t iterationsPerStep;
  CostFunction & costFunc;
  LineSearch & linesearch;

  std::ostream & feedback;

  double previousStepSize;

public:
  ConjugateGradientOpt (CostFunction & costFunc, LineSearch & lineSearch,
                        std::ostream & feedback);

  void perform (Matrix & projData);

  void updateDynamicParameters (size_t currentRound, size_t totalRounds);

  void setIterationsPerStep (size_t number);
};

#endif
