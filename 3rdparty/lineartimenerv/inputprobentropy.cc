/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include <iostream>
#include <limits>
#include "inputprobentropy.hh"

double
InputProbEntropy::evaluate (const DataMatrix & sigma)
{
  if (sigma (0, 0) <= 0.0)
    return std::numeric_limits < double >::max ();

  prob.update (index, sigma (0, 0));

  double entropy = 0.0;

  for (size_t j = 0; j < prob.getCols (); j++)
  {
    if (j != index)
    {
      //if (j < 10) printf("prob(%d,%d)=%e\n", index,j,prob(index,j));
      entropy -= prob (index, j) * log (prob (index, j)) / log (2.0);
    }
  }

  //printf("Sigma: %e, Entropy[%d] now %e, desired %e\n", sigma(0,0),index,entropy, desiredEntropy);
  //std::cout << "Entropy error " << fabs(entropy - desiredEntropy) << "\n";

  return fabs (entropy - desiredEntropy);
}


InputProbEntropy::InputProbEntropy (size_t effectiveNeighborhoodSize, const size_t index, NeRVProbability & prob):desiredEntropy ((effectiveNeighborhoodSize <
        2) ? 1.0 : log (effectiveNeighborhoodSize) / log (2.0)),
    index (index), prob (prob)
{
}
