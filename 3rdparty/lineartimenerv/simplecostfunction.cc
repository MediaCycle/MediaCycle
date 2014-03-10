/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "simplecostfunction.hh"

double
SimpleCostFunction::getGradient (const DataMatrix & data, DataMatrix & target)
{
  double sqsum = 0.0;
  for (size_t i = 0; i < data.getRows (); i++)
  {
    for (size_t j = 0; j < data.getCols (); j++)
    {
      target (i, j) = 4.0 * data (i, j) * data (i, j) * data (i, j);
      sqsum += target (i, j) * target (i, j);
    }


  }
  return sqsum;

}

double
SimpleCostFunction::evaluate (const DataMatrix & data)
{
  double cost = 0.0;

  for (size_t i = 0; i < data.getRows (); i++)
  {
    for (size_t j = 0; j < data.getCols (); j++)
    {
      cost += data (i, j) * data (i, j) * data (i, j) * data (i, j);
    }
  }

  return cost;
}
