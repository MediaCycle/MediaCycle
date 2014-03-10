/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include <iostream>
#include <string>

#include "sompackloader.hh"
#include "dataset.hh"
#include "distancematrix.hh"
#include "sompackexporter.hh"
#include "metric.hh"
#include "euclidean.hh"
#include "randomdatagenerator.hh"
#include "commandlineargs.hh"
#include "nervcostfunction.hh"
#include "nervoptstrat.hh"
#include "goldensectionsearch.hh"
#include "conjugategradientopt.hh"
#include "coloroptcostfunction.hh"
#include "inversedynamicdouble.hh"

void shiftinit(DataMatrix & data)
{
  for (size_t i=0;i<data.getRows();++i)
    data(i,0)=data(i,0)+50.0;
}

int main(int argc, char** argv)
{
  CommandLineArgs args(argc, argv);
  DataSet projData;
  DataSet origData;
  double minC,minL,maxL;
  double betaC,betaL,betaG;
  double colorScaling;

  std::string usage = "No usage string yet; look at nerv.cc to find out what parameters I expect.\n";

  try
  {
    std::string inputfile;
    std::string outputfile;
    double lambda;
    size_t lastNeighbor;

    if (argc == 1 || args.hasArg("--help"))
    {
      std::cerr << usage;
    }

    args.getString("--inputfile", inputfile);
    args.getString("--outputfile", outputfile);
    lambda = args.getDouble("--lambda");
    lastNeighbor = args.getUInt("--k");


    SOMPackExporter exporter(outputfile);

    SOMPackLoader loader(inputfile);
    loader.loadData(origData);

    if (args.hasArg("--init"))
    {
      std::string initfile;
      args.getString("--init", initfile);

      SOMPackLoader initloader(initfile);
      initloader.loadData(projData);
      if (projData.getCols()!=3)
        std::cerr << "Error! Wrong dimension in initialization file. Colors must be 3 dimensional.";
    }
    else
    {
      size_t outputDimension = 3; /* The default dimension */

      RandomDataGenerator randgen(origData.getRows(),
                                  outputDimension, 1);
      randgen.loadData(projData);
      shiftinit(projData);
    }


    Euclidean metric;
    DistanceMatrix origDataDist(origData, metric);
    if (args.hasArg("--color_scaling"))
    {
      colorScaling = args.getDouble("--color_scaling");
      if (colorScaling<0.0)
        colorScaling=1.0;
    }
    else
      colorScaling=1.0;

    origDataDist.scale(colorScaling/origDataDist.getAverage());


    double initrad = 0.0;
    if (args.hasArg("--initial_radius"))
    {
      initrad = args.getDouble("--initial_radius");
    }
    else
    {
      initrad = origDataDist.getMax() / 2.0;
      std::cerr << "Initial radius " << initrad << std::endl;
      std::cerr << "Maximum distance " << origDataDist.getMax()
      << std::endl;
    }


    DynamicDouble initialRadius(initrad, 0);

    GoldenSectionSearch linesearch;

    NeRVCostFunction costFunc(origDataDist, projData, linesearch,
                              initialRadius, lambda, lastNeighbor,
                              std::cerr);

    if (args.hasArg("--minC"))
      minC=args.getDouble("--minC");
    else
      minC=0.0;


    if (args.hasArg("--minL"))
    {
      minL=args.getDouble("--minL");
      if (minL<10.0)
        minL=10.0;
    }
    else
      minL=10.0;


    if (args.hasArg("--maxL"))
    {
      maxL=args.getDouble("--maxL");
      if (maxL>95.0)
        maxL=95.0;
    }
    else
      maxL=95.0;

    if (maxL<minL)
    {
      double tmp=maxL;
      maxL=minL;
      minL=tmp;
    }


    if (args.hasArg("--betaL"))
    {
      betaL=args.getDouble("--betaL");
      if (betaL<0.0)
        betaL=0.0;
    }
    else
      betaL=100.0;


    InverseDynamicDouble dBetaL(0, betaL);


    if (args.hasArg("--betaC"))
    {
      betaC=args.getDouble("--betaC");
      if (betaC<0.0)
        betaC=0.0;
    }
    else
      betaC=100.0;

    InverseDynamicDouble dBetaC(0, betaC);

    if (args.hasArg("--betaG"))
    {
      betaG=args.getDouble("--betaG");
      if (betaL<0.0)
        betaG=0.0;
    }
    else
      betaG=1.0e6;

    InverseDynamicDouble dBetaG(0, betaG);

    ColorOptCostFunction colCostFunc(costFunc, dBetaL, dBetaC,
                                     dBetaG, minL, maxL, minC,
                                     std::cerr);


    ConjugateGradientOpt optStep(colCostFunc, linesearch, std::cerr);
    NeRVOptStrat optStrategy(10,2,40, "");

    optStrategy.optimize(origDataDist, projData, optStep, colCostFunc,
                         std::cerr);
    exporter.exportData(projData);
    long numLminViol;
    long numLmaxViol;
    long numCminViol;
    long numGamutViol;
    double projCost;
    double colCost;

    colCostFunc.violations(projData, projCost, colCost, numLminViol,
                           numLmaxViol, numCminViol, numGamutViol);

    std::cout <<projData.getRows()<<" data points, cost: "<<projCost+colCost<<" ("<<projCost<<", "<<colCost<<")"<<std::endl<<"Lmin violations: "<<numLminViol<<", Lmax violations: "<<numLmaxViol<<", Cmin violations: "<<numCminViol<<", gamut violations: "<<numGamutViol<<std::endl;

    if (args.hasArg("--sRGB_out"))
    {
      SOMPackExporter rgbexporter(outputfile.append("-rgb"));
      double me=colCostFunc.convertToGamut(projData, projData);
      std::cout <<"Mean truncation in sRGB conversion: "<<me<<std::endl;
      //colCostFunc.convert(projData, projData);
      rgbexporter.exportData(projData);
    }
  }

  catch (Exception& exc)
  {
    std::cerr << "Error: " << exc.getErrMsg() << std::endl;
  }

  return 0;
}
