/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "costfunction.hh"

class SimpleCostFunction:public CostFunction
{
public:
  double getGradient (const DataMatrix & data, DataMatrix & target);

  double evaluate (const DataMatrix & data);


  void updateDynamicParameters (size_t currentRound,
                                size_t totalRounds,
                                const DataMatrix & projData)
  {
  }

  virtual void updateDataRepresentation (const DataMatrix & newData)
  {
  }
};
