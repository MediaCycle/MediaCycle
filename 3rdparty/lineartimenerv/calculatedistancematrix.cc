/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "calculatedistancematrix.hh"

void
CalculateDistanceMatrix::operator () (const DataMatrix & data,
                                      Metric & metric, DataMatrix & target)
{
  //  Matrix distmat(data.getRows(), data.getRows());


  std::cout << "Calculating distance matrix" << std::endl;

  if (metric.isSymmetric ())
  {
    for (size_t i = 0; i < data.getRows (); i++)
      for (size_t j = i; j < data.getRows (); j++)
        if (i == j)
          target (i, j) = 0;
        else
          target (i, j) = target (j, i) = metric (data, i, j);
  }
  else
  {
    for (size_t i = 0; i < data.getRows (); i++)
      for (size_t j = 0; j < data.getRows (); j++)
        if (i == j)
          target (i, j) = 0;
        else
          target (i, j) = target (j, i) = metric (data, i, j);
  }

    for (size_t i = 0; i < data.getRows (); i++)
      for (size_t j = 0; j < data.getRows (); j++)
      {
	//std::cout << "d[" << i << "," << j << "]=" << target(i,j) << std::endl;
      }

  //  target = distmat;
}
