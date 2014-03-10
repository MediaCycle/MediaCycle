/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "numericalgradientcomputer.hh"

void
NumericalGradientComputer::compute (CostFunction & function,
                                    DataMatrix & data, double delta,
                                    DataMatrix & gradient)
{
  gradient = DataMatrix (data.getRows (), data.getCols ());

  double originalCost = function.evaluate (data);
  double shiftedCost = 0.0;
  double originalComponent;

  for (size_t i = 0; i < data.getRows (); i++)
  {
    for (size_t j = 0; j < data.getCols (); j++)
    {
      originalComponent = data (i, j);

      data (i, j) += delta;
      shiftedCost = function.evaluate (data);
      gradient (i, j) = (shiftedCost - originalCost) / delta;

      data (i, j) = originalComponent;
    }
  }
}
