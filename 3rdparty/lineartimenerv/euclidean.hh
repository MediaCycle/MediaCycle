/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef EUCLIDEAN_HH
#define EUCLIDEAN_HH

#include "metric.hh"
#include "dataset.hh"


class Euclidean:public Metric
{
public:
  double operator () (const DataMatrix & data, size_t row1, size_t row2);

  bool isSymmetric () const
  {
    return true;
  }
};

#endif
