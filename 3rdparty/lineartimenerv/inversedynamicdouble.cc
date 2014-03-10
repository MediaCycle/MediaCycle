/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "inversedynamicdouble.hh"

InverseDynamicDouble::InverseDynamicDouble(double initialValue, double finalValue)
    : DynamicDouble(initialValue,finalValue)
{}


void InverseDynamicDouble::update(size_t currentRound, size_t totalRounds)
{
  currentValue = originalValue -
                 ((finalValue - originalValue)/(double)(totalRounds)) * (double)currentRound;
}
