  /*Copyright (C) Kristian Nybo, Jarkko Venna
   *Modified for tcNeRV by Jaakko Peltonen
   *This software is released under the GNU Lesser General Public
   *License. See the included file LICENSE for details.*/

#ifndef GEANERVOPTSTRAT_HH
#define GEANERVOPTSTRAT_HH

#include "optimizationstrategybatch.hh"
#include "optimizationstepbatch.hh"
#include "sompackexporter.hh"

  class geaNeRVOptStrat:public OptimizationStrategyBatch
  {
  private:
    const size_t TOTAL_ROUNDS;

    const size_t STEPS_PER_ROUND;
    const size_t STEPS_ON_LAST_ROUND;

  public:

    void optimize (const DistanceMatrix & origDist,
                   DataMatrix & initialProjData,
                   OptimizationStepBatch & optStep, CostFunction & costFunc,
                   std::ostream & feedback);
    
    void geanervoptimize (/*const DistanceMatrix & origDist,*/
                   DataMatrix & initialProjData,
                   OptimizationStepBatch & optStep, CostFunction & costFunc,
                   std::ostream & feedback);

    geaNeRVOptStrat (size_t totalIterations, size_t stepsPerRound, size_t stepsOnLastRound, const char* outputFilename):TOTAL_ROUNDS (totalIterations), STEPS_PER_ROUND (stepsPerRound), STEPS_ON_LAST_ROUND(stepsOnLastRound)
  {
  }

    geaNeRVOptStrat (size_t totalIterations, size_t stepsPerRound, size_t stepsOnLastRound, const std::string& outputFilename):TOTAL_ROUNDS (totalIterations), STEPS_PER_ROUND (stepsPerRound), STEPS_ON_LAST_ROUND(stepsOnLastRound)
    {
    }
};

#endif
