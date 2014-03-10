/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "conjugategradientopt.hh"
#include "sompackloader.hh"
#include "goldensectionsearch.hh"
#include "simplecostfunction.hh"
#include "commandlineargs.hh"
#include "datamatrix.hh"

#include <string>
#include <iostream>


int
main (int argc, char **argv)
{
  DataSet initialPoint;
  CommandLineArgs args (argc, argv);


  std::string dataFile;
  args.getString ("--data", dataFile);
  SOMPackLoader loader (dataFile);
  loader.loadData (initialPoint);


  GoldenSectionSearch linesearch;
  SimpleCostFunction costFunc;

  std::cout << "Initial data:" << std::endl << initialPoint;

  ConjugateGradientOpt optimizer (costFunc, linesearch, std::cerr);
  optimizer.perform (initialPoint);

  std::cout << std::endl << "Optimized data:" << initialPoint << std::endl;

  return 0;
}
