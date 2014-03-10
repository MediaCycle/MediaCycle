/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include <stdlib.h>
#include <locale.h>
#include <cmath>
#include <stdio.h>
#include "randomdatagenerator.hh"

RandomDataGenerator::RandomDataGenerator (size_t numberOfPoints,
					  size_t dimension, double range, 
					  int useSpecifiedSeed, 
					  unsigned int theSpecifiedSeed):
    range (range),
    matrix (numberOfPoints, dimension)
{
  unsigned int mytime;
  if (useSpecifiedSeed)
    mytime = theSpecifiedSeed;
  else mytime = time(0);
  printf("Using random seed %u\n", mytime);
  srand(mytime);
  /* srand (time (0)); */  
}


void
RandomDataGenerator::loadData (DataSet & target)
{
  for (size_t i = 0; i < matrix.getRows (); i++)
    for (size_t j = 0; j < matrix.getCols (); j++)
      matrix (i, j) = rand () * (range / RAND_MAX);

  DataSet data (matrix);

  target = data;
}
