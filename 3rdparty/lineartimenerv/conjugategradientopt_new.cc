/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "conjugategradientopt.hh"

#include <math.h>
#include <limits>

void
ConjugateGradientOpt::perform (Matrix & projData)
{
  const double EPS = 1.0e-8;
  double previousCost = 1.0, cost = 0;
  double beta = 0.0;
  Matrix oldGradient (projData.getRows (), projData.getCols ());
  Matrix gradient (projData.getRows (), projData.getCols ());

  Matrix tmpmat (projData.getRows (), projData.getCols ());
  Matrix direction (projData.getRows (), projData.getCols ());


  /* Choose the negative gradient as the initial direction... */

  costFunc.getGradient (projData, gradient);
  double sqsum = 0.0;

  for (size_t i = 0; i < gradient.getRows (); ++i)
    for (size_t j = 0; j < gradient.getCols (); ++j)
      sqsum += gradient (i, j) * gradient (i, j);
  //  std::cerr<<"initial scaling:"<< sqsum<<std::endl;
  if (sqrt (sqsum) < EPS)
    return;
  previousStepSize = 1.0 / sqsum;
  gradient.scale (-1.0);


  direction = gradient;

  /* .. and minimize the function along the line that passes through
     projData in the direction of the negative gradient, setting projData
     equal the minimum. */

  previousStepSize = linesearch (costFunc, projData, gradient,
                                 previousStepSize, previousCost);

  //costFunc.updateDataRepresentation(projData);

  double oldGradientSqrd = 0.0;


  for (size_t i = 0; i < iterationsPerStep; i++)
  {
    if (fabs (previousCost - cost) < EPS)
      return;
    /* On subsequent iterations the direction is constructed by subtracting
       nonconjugate components from the gradient (Gram-Schmidt). */
    previousCost = cost;

    oldGradient = gradient;
    costFunc.getGradient (projData, gradient);
    gradient.scale (-1.0);

    tmpmat = gradient;
    tmpmat -= oldGradient;

    oldGradientSqrd = oldGradient.dotProduct (oldGradient);

    if (oldGradientSqrd < EPS)
      return;

    beta = gradient.dotProduct (tmpmat) / oldGradientSqrd;

    std::cout << "Beta: " << beta << std::endl;


    /* If beta < 0, we should set beta to 0 to guarantee convergence.
       Note that setting beta to 0 is equivalent with restarting the method,
       ie., minimizing along the negative gradient instead of a direction
       that is conjugate with the previous direction. */

    if (beta < 0)
      beta = 0;

    tmpmat = direction;
    tmpmat.scale (beta);

    direction = gradient;
    direction += tmpmat;

    previousStepSize = linesearch (costFunc, projData, direction,
                                   previousStepSize, cost);
    std::cerr << "ConjGrad step end cost: " << cost << std::endl;
    costFunc.updateDataRepresentation (projData);
  }

  feedback << "Conjugate gradient step finished, cost now "
  << costFunc.evaluate (projData) << std::endl;
}


/* The conjugate gradient method doesn't have any parameters that need to
   be updated every round. */

void
ConjugateGradientOpt::updateDynamicParameters (size_t, size_t)
{
}

ConjugateGradientOpt::ConjugateGradientOpt (CostFunction & costFunc, LineSearch & linesearch, std::ostream & feedback):iterationsPerStep (DEFAULT_ITERATIONS), costFunc (costFunc),
    linesearch (linesearch), feedback (feedback),
    previousStepSize (1.0)
{
}


void
ConjugateGradientOpt::setIterationsPerStep (size_t number)
{
  iterationsPerStep = number;
}
