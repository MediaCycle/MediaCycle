/*Copyright (C) Kristian Nybo, Jarkko Venna
 *Modified for tcNeRV by Jaakko Peltonen
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "geanervoptstrat.hh"
#include "sompackexporter.hh"
#include "geanervcostfunction.hh"

#include <sstream>


void
geaNeRVOptStrat::optimize (const DistanceMatrix & origDist,
                        DataMatrix & initialProjData,
                        OptimizationStepBatch & optStep,
                        CostFunction & costFunc, std::ostream & feedback)
{
  printf("ERROR: This version of geaNervOptStrat::optimize should never be called!\n");  
}


void
geaNeRVOptStrat::geanervoptimize (/*const DistanceMatrix & origDist,*/
                        DataMatrix & initialProjData,
                        OptimizationStepBatch & optStep,
                        CostFunction & costFunc, std::ostream & feedback)
{
  double initialcost;
  
  optStep.setIterationsPerStep (STEPS_PER_ROUND);
  
  costFunc.updateDynamicParameters (TOTAL_ROUNDS, TOTAL_ROUNDS,
                                    initialProjData);

  
  printf("Computing initial cost\n");
  initialcost = costFunc.evaluate (initialProjData);
  printf("Initial cost: %e\n", initialcost);

  
  for (size_t roundsLeft = TOTAL_ROUNDS; roundsLeft > 0; roundsLeft--)
  {
    optStep.updateDynamicParameters (TOTAL_ROUNDS - roundsLeft, TOTAL_ROUNDS);

    costFunc.updateDynamicParameters (TOTAL_ROUNDS - roundsLeft, TOTAL_ROUNDS,
                                      initialProjData);

    feedback << "Starting round " << TOTAL_ROUNDS - roundsLeft << "...\n";

    optStep.perform (initialProjData);

    feedback << "Done.\n" << std::endl;
  }

  feedback << "Starting final round, performing " << STEPS_ON_LAST_ROUND
  << " optimization steps.\n";

  
  optStep.updateDynamicParameters (TOTAL_ROUNDS, TOTAL_ROUNDS);

  costFunc.updateDynamicParameters (TOTAL_ROUNDS, TOTAL_ROUNDS,
                                    initialProjData);

  optStep.setIterationsPerStep (STEPS_ON_LAST_ROUND);
  optStep.perform (initialProjData);
  
  
  feedback << "Optimization done.\n" << std::endl;

  
  feedback << "Evaluating one more time.\n" << std::endl;
  //geanerv_printcostfunctionterms = 1;
  costFunc.evaluate (initialProjData);
  
  feedback << "Optimize function finished, exiting.\n" << std::endl;
  
}
