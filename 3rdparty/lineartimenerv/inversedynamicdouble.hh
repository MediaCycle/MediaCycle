/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef INVERSEDYNAMICDOUBLE_HH
#define INVERSEDYNAMICDOUBLE_HH

#include <cstddef>
#include "dynamicdouble.hh"

class InverseDynamicDouble: public DynamicDouble
{
private:
public:
  ~InverseDynamicDouble() {}
  InverseDynamicDouble(double initialValue, double finalValue);

  virtual void update(size_t currentRound, size_t totalRounds);
};

#endif
