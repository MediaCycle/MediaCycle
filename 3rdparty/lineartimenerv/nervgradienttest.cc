/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "numericalgradientcomputer.hh"
#include "nervcostfunction.hh"
#include "commandlineargs.hh"
#include "dynamicdouble.hh"
#include "sompackloader.hh"
#include "goldensectionsearch.hh"
#include "datamatrix.hh"
#include "distancematrix.hh"
#include "dataset.hh"
#include "euclidean.hh"

#include <iostream>
#include <string>
#include <math.h>

int
main (int argc, char **argv)
{
  CommandLineArgs args (argc, argv);

  DataSet projData;
  DataSet origData;

  DataMatrix numGradient;
  DataMatrix gradientDiff;

  NumericalGradientComputer numgrad;


  std::string inputDataFile;
  std::string initDataFile;
  size_t knn = 0;
  double lambda = 0;
  double initialRadius = 1.5;
  double delta = 0.0;

  try
  {

    args.getString ("--inputdata", inputDataFile);
    args.getString ("--initdata", initDataFile);
    knn = args.getUInt ("--k");
    lambda = args.getDouble ("--lambda");
    initialRadius = args.getDouble ("--radius");
    delta = args.getDouble ("--delta");

    DynamicDouble radius (initialRadius, 0);

    SOMPackLoader origloader (inputDataFile);
    SOMPackLoader initloader (initDataFile);

    origloader.loadData (origData);

    Euclidean metric;
    DistanceMatrix distMatrix (origData, metric);
    distMatrix.scale (1.0 / distMatrix.getAverage ());

    initloader.loadData (projData);

    GoldenSectionSearch lineSearch;


    NeRVCostFunction costfunc (distMatrix, projData,
                               lineSearch, radius, lambda, knn, std::cerr);

    DataMatrix nervGradient (projData.getRows (), projData.getCols ());

    costfunc.getGradient (projData, nervGradient);

    numgrad.compute (costfunc, projData, delta, numGradient);


    gradientDiff = nervGradient;
    gradientDiff -= numGradient;

    for (size_t i = 0; i < gradientDiff.getRows (); i++)
    {
      for (size_t j = 0; j < gradientDiff.getCols (); j++)
      {
        gradientDiff (i, j) = fabs (gradientDiff (i, j));
      }
    }

    std::cout <<
    "gradientDiff.getAverage() / numGradient.getAverage(): "
    << gradientDiff.getAverage () / numGradient.getAverage () << std::endl;

    if (args.hasArg ("--print"))
    {
      std::cout << std::endl
      << nervGradient << std::endl << numGradient << std::endl;
    }

  }

  catch (Exception & foo)
  {
    std::cout << "Error: " << foo.getErrMsg () << std::endl;
  }

  return 0;
}
