/*Copyright (C) Kristian Nybo, Jarkko Venna
 *Modified for tcNeRV by Jaakko Peltonen
 *Modified for lineartime nerv by Konstantinos Georgatzis
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <float.h>

#include "sompackloader.hh"
#include "dataset.hh"
#include "distancematrix.hh"
#include "sompackexporter.hh"
#include "metric.hh"
#include "euclidean.hh"
#include "randomdatagenerator.hh"
#include "commandlineargs.hh"
#include "geanervcostfunction.hh"
#include "geanervoptstrat.hh"
#include "goldensectionsearch.hh"
#include "conjugategradientopt.hh"
#include "distancematrixloader.hh"
#include "pca.hh"
#include "nervcostfunction.hh"
#include "nervoptstrat.hh"
#include "clustersandexacts.c"

#define membership_tol (1e-6)
int iteration = 0;

int
main (int argc, char **argv)
{
  clock_t start = clock();  
  
  int nclusters, ndata, nexact, nYdim, nXdim;
  DataSet projData;
  DataSet origData;
  ConjugateGradientOpt *optStep = NULL;
  ConjugateGradientOpt *nerv_optStep = NULL;

  //DistanceMatrix origDataDist;
  DataSet provided_clusterIndices;
  DataSet provided_allclusterXmeans;
  DataSet provided_allclusterXcovs;
  DataSet provided_exactIndices;

  
  
  std::vector<std::string> switches;
  switches.push_back("--inputfile");
  switches.push_back("--outputfile");
  switches.push_back("--inputdist");
  switches.push_back("--lambda");
  switches.push_back("--neighbors");
  switches.push_back("--init");
  switches.push_back("--outputdim");
  switches.push_back("--initrad");
  switches.push_back("--iterations");
  switches.push_back("--seed");  
  switches.push_back("--cg_steps");
  switches.push_back("--cg_steps_final");  
  switches.push_back("--record");
  switches.push_back("--randinit");

  switches.push_back("--clusterindicesfile");  
  switches.push_back("--clusterxmeansfile");  
  switches.push_back("--clusterxcovsfile");  
  switches.push_back("--exactneighborindicesfile");

  switches.push_back("--nkmeansiterations");
  switches.push_back("--nclusters");
  switches.push_back("--nexactpercentage");
  
  
  
  std::set<std::string> recognizedSwitches(switches.begin(), switches.end());

  std::string usage =
    "Synopsis: tcnerv [OPTION] [--inputfile | --inputdist] [INPUT_FILE] --outputfile [OUTPUT_FILE]"
    "\n"
    "Required parameters:"
    "\n"
    "--inputfile INPUT_FILE - The name of the file that contains the data points to be visualized in SOM_PAK format. If a distance matrix is provided via --inputdist (see below), this switch will be ignored. "
    "\n"
    "--outputfile OUTPUT_FILE - The name of the file to which the output will be written, also in SOM_PAK format."
    "\n\n"
    "Optional parameters:"
    "\n"
    "--inputdist INPUT_DIST - The name of the SOM_PAK file that contains a distance matrix of the data to be visualized."
    "\n"
    "--lambda L - Control the trustworthiness-continuity tradeoff. A value of zero emphasizes trustworthiness only; increasing L shifts the emphasis towards continuity. L must be a floating point number between 0 and 1, but in practice values higher than 0.5 are not recommended. The default value for L is 0."
    "\n"
    "--neighbors K - Set the number of neighbors that each point should have: each point will consider the K points closest to it its neighbors. K should be a positive integer. The default value for K is 20."
    "\n"
    "--init INIT_FILE - Initialize the visualization with the data in INIT_FILE instead of using a random initialization as per default. The data in INIT_FILE should be in SOM_PAK format, and naturally it must contain the same number of points as the data in INPUT_FILE (see --inputfile)."
    "\n"
    "--seed SEED - Random seed number when using a random initialization; by default chooses seed from system time."
    "\n"
    "--outputdim DIM - The desired dimension of the output data. DIM should be a positive integer. The default value for DIM is 2. Note that --outputdim is ignored if initialization data is passed via --init, because the dimension of the output data must have the same dimension as the initialization."
    "\n"
    "--initrad RAD - The initial value for the global radius parameter. The default is 1.5 * the maximum distance in the original data."
    "\n"
    "--iterations ITERATIONS - The number of iterations to perform. If OPTMETHOD='cca', the default is 200 * the number of data points in INPUT_FILE. If OPTMETHOD='nerv', the default is 10."
    "\n"
    "--cg_steps CG_STEPS -  The number of conjugate gradient steps to perform per iteration in NeRV's optimization scheme. The default value is 2."
    "\n"
    "--cg_steps_final CG_STEPS_FINAL -  The number of conjugate gradient steps to perform on the final iteration in NeRV's optimization scheme. The default value is 40."
    "\n"
    "--record FILENAME_STEM - Outputs the current projection to a separate file after each optimization step. The filenames will be of the form ${FILENAME_STEM}_${STEP_NUMBER}.dat."
    "\n";


  try
  {
    CommandLineArgs args (argc, argv, recognizedSwitches);
    std::string inputfile;
    std::string outputfile;
    std::string tmpoutputfile;
    double lambda;
    size_t lastNeighbor;

    if (argc == 1 || args.hasArg ("--help"))
    {
      std::cerr << usage;
    }

    if (args.hasArg("--lambda"))
    {
      lambda = args.getDouble ("--lambda");
    }
    else
    {
      lambda = 0;
    }

    if (args.hasArg("--neighbors"))
    {
      lastNeighbor = args.getUInt ("--neighbors");
    }
    else
    {
      lastNeighbor = 20;
    }

    args.getString ("--outputfile", outputfile);
    SOMPackExporter exporter (outputfile);
    
    if (args.hasArg ("--inputfile"))
    {            
      args.getString ("--inputfile", inputfile);

      SOMPackLoader loader (inputfile);
      loader.loadData (origData);

      ndata = origData.getRows();
      printf("read ndata %d\n", ndata);
      nXdim = origData.getCols();
      printf("read nXdim %d\n", nXdim);
    }
    else
    {
      printf("Error: original data matrix not given! This version of NeRV requires the original data feature matrix, distances alone are not enough.\n");
      exit(1);
    }
    
    unsigned int seed;
    if (args.hasArg ("--seed"))
    {
      seed = args.getUInt("--seed");
      srand(seed);
      printf("Using provided random seed %u\n", seed);
    }
    else
    { 
      seed = time(NULL);
      srand(seed);	
      printf("Using generated random seed %u\n", seed);
    }

    size_t outputDimension;
    if (args.hasArg ("--outputdim"))
    {
      outputDimension = args.getUInt ("--outputdim");
    }
    else
    {
      outputDimension = 2;
    }
     
    nYdim = outputDimension; // projData.getCols();
    
    if (nYdim != 2)
    {
      printf("Error: the code does not currently support output dimensionalities other than 2 (projected data was set to dimensionality %d)!\n", nYdim);
      exit(1);
    }      
       
    int nkmeansiterations;
    if (args.hasArg ("--nkmeansiterations"))
    {
      nkmeansiterations = args.getUInt ("--nkmeansiterations");
    }
    else
    {
      nkmeansiterations = 20;
    }
    
    if (args.hasArg ("--nclusters"))
    {
      nclusters = args.getUInt ("--nclusters");
    }
    else
    {
      nclusters = 20;
    }

    if (args.hasArg ("--nexactpercentage"))
    {
      double temppercentage;
      temppercentage = args.getDouble ("--nexactpercentage");
      nexact = ((ndata-1)*temppercentage)/100.0;
    }
    else
    {
      nexact = 0;
    }

if (nclusters == 0)
{
    // Read in or create the initial projected data
    if (args.hasArg ("--init"))
    {
      std::string initfile;
      args.getString ("--init", initfile);

      SOMPackLoader initloader (initfile);
      initloader.loadData (projData);
    }
    else if (args.hasArg("--randinit") || args.hasArg("--inputdist"))
    {
      size_t outputDimension = 2;       // The default dimension 

      if (args.hasArg ("--outputdim"))
      {
        outputDimension = args.getUInt ("--outputdim");
      }
      printf("seed for initialisation of 1st phase of approx NeRV = %u\n", seed);
      RandomDataGenerator randgen (origData.getRows(), outputDimension, 1, 1, seed);
      randgen.loadData (projData);      
    }
    //Initialize with PCA.
    else
    {
      size_t outputDimension = 2;       // The default dimension 

      if (args.hasArg ("--outputdim"))
      {
        outputDimension = args.getUInt ("--outputdim");
      }
      
      PCA<DataSet> pca(origData);
      RandomDataGenerator randgen (origData.getRows(), outputDimension, 1, 1, seed);      
      randgen.loadData (projData);      
      printf("Trying PCA initialization\n");
      pca.getComponents(projData);
      printf("PCA initialization ready\n");
    }

    printf("After initialization\n");
}

  // Create arrays etc. to hold cluster statistics   
  int i, j, k ,l;
  int *clusterIndices;         // ndata * nclusters
  double *clustersizes;   	     // nclusters * 1
  double  **clusterXmeans;   // nclusters * nXdim
  double **clusterMemberships;
  double **X;
  double ***clusterXcovs;   // nclusters * nXdim * nXdim
  int **exact;
  double kmeansfinalcost;
  
  // Read in cluster statistics from files  
  
  
  clusterIndices = (int *)malloc(ndata*sizeof(int));
  clusterXmeans = (double **) malloc(nclusters*sizeof(double *));
  
  clustersizes = (double *) malloc(nclusters*sizeof(double));  
  clusterXcovs = (double ***) malloc(nclusters*sizeof(double **));
  
   
  X = (double **)malloc(ndata*sizeof(double *));
  for (i = 0; i < ndata; i++)
  {
    X[i] = (double *)malloc(nXdim*sizeof(double));
    for (j = 0; j < nXdim; j++)
      X[i][j] = origData(i,j);
  }  

   printf("Running k-means\n");
   weightedkmeans(ndata,nXdim,X,nclusters,nkmeansiterations,&clusterIndices,&clusterXmeans,&clustersizes,&clusterXcovs,&kmeansfinalcost);

   // Read in indices of exact neighbors
   myprintf("Finding exact neighbors\n\n");
   if (args.hasArg ("--exactneighborindicesfile"))
   {
     std::string exactneighborindicesfile;
     args.getString ("--exactneighborindicesfile", exactneighborindicesfile);

     std::cerr << "Loading exact neighbor indices from file [" << exactneighborindicesfile << "]" << std::endl;
     SOMPackLoader loader (exactneighborindicesfile);
     loader.loadData (provided_exactIndices);
     nexact = provided_exactIndices.getCols();
     printf("Entered exactneighborindices section-------------------------------------------------------------------------------------------------------");
     if (provided_exactIndices.getRows() != ndata)
     {
       printf("Error: number of rows %d in exact indices does not match number of data %d!\n", provided_exactIndices.getRows(), ndata);
       exit(1);
     }      
     exact = (int **)malloc(ndata*sizeof(int *));
     for (i = 0; i < ndata; i++)
     {
       exact[i] = (int *)malloc(nexact*sizeof(int));
       for (j = 0; j < nexact; j++)
       { 
         if (nexact > 0)
         {    
          exact[i][j] = provided_exactIndices(i,j);
         }
         if ((exact[i][j] < 0) || (exact[i][j] >= ndata))
         {
           printf("Error: index of the %d:th exact neighbor of data point %d is %d, which is outside the allowed range [0,...,%d] for %d data points!\n", j, i, exact[i][j], ndata-1, ndata);
           exit(1);
         }
       }
     }     
   }
   else
   {
     printf("Warning: exactneighborindicesfile has not been provided!\n");

     initializeExactNeighbors
     (
       ndata, 
       nXdim, 
       nexact,
       nclusters,
       clusterIndices,
       clustersizes,
       &exact
     );     
   }

  

  // Compute overall data scale (largest pairwise distance)
    double initrad = 0.0;
    double tempcompare = 0.0;
    int exactj;
    double tempTrace;
    // Calculate intitial Radius
    if (args.hasArg ("--initrad"))
    {
      initrad = args.getDouble ("--initrad");
    }
    else
    {
       if ( nexact > 0)
       {
         for (i = ndata-1; i >= 0; i--)
         {
           for (k = nexact-1; k >= 0; k--)
           {
             //exactj = exact[i][k];
	     exactj = exact[i][k];
             tempcompare = 0;
             for (l = nXdim-1; l >= 0; l--)
             {
              tempcompare = tempcompare + (origData(i, l) - origData(exactj, l)) * (origData(i, l) - origData(exactj, l));
             }
             if (tempcompare > initrad)
             {
	      initrad = tempcompare;
             }
           } 
         }
       }

       if (nclusters > 0)
       {
         for (i = ndata-1; i >= 0; i--)
         {
           for (k = nclusters-1; k >= 0; k--)
           {
             tempTrace = 0;
             tempcompare = 0;
             for (l = nXdim-1; l >= 0; l--)
             {
	      tempTrace += clusterXcovs[k][l][l];
              tempcompare = tempcompare + (X[i][l] - clusterXmeans[k][l]) * (X[i][l] - clusterXmeans[k][l]);        
             }
             tempcompare += tempTrace / nXdim;
             if (tempcompare > initrad)
             {
	      initrad = tempcompare;
             }
           } 
         }
       }      

      initrad = sqrt(initrad) / 2.0;
      std::cerr << "Initial radius " << initrad << std::endl;
      std::cerr << "Maximum distance " << initrad * 2 << std::endl;
    }
  
  // Compute memberships of data points in clusters  
  computeClusterMemberships
  (
    ndata,
    nXdim,
    X,
    nclusters,
    clusterIndices,
    clusterXmeans,
    clustersizes,
    clusterXcovs,
    &clusterMemberships
  );
  printf("CheckSeg\n");
 
  double *membershipSum;      // the per-point sum of the memberships that are > cluster_tol, will be used for renormalisation  
  int *nonzeroMemberscounter; // the per-point number of memberships that are > cluster_tol, will be used to malloc our denseMemberships array
  
  membershipSum = (double *)malloc(ndata*sizeof(double));   
  nonzeroMemberscounter = (int *)malloc(ndata*sizeof(int)); 
  
  for (i = 0; i < ndata; i++)
  {
    membershipSum[i] = 0;
    nonzeroMemberscounter[i] = 0;
    for (j = 0; j < nclusters; j++)
    {
      if (clusterMemberships[i][j] > membership_tol)
      {
	nonzeroMemberscounter[i]++;
	membershipSum[i] += clusterMemberships[i][j];
      }	
    }  
  }
  
  double **denseMemberships; 
  int **memb2clusterind;   
  
  denseMemberships = (double **)malloc(ndata*sizeof(double*));
  memb2clusterind = (int **)malloc(ndata*sizeof(int*));
  
  for (i = 0; i < ndata; i++)
  {
    memb2clusterind[i] = (int *)malloc(nonzeroMemberscounter[i]*sizeof(int));
    denseMemberships[i] = (double *)malloc(nonzeroMemberscounter[i]*sizeof(double));
  }
  
  for (i = 0; i < ndata; i++)
  {
    k = 0;
    for (j = 0; j < nclusters; j++)
    {
      if (clusterMemberships[i][j] > membership_tol)
      {
	denseMemberships[i][k] = clusterMemberships[i][j] / membershipSum[i];
	memb2clusterind[i][k] = j;
	k++;
      }	
    }  
  }
  
  
    size_t iterations, stepsPerRound, stepsOnLastRound;

    if (args.hasArg ("--iterations"))
    {
      iterations = args.getUInt ("--iterations");
    }
    else
    {
      iterations = 10;
    }

    if (args.hasArg ("--cg_steps"))
    {
      stepsPerRound = args.getUInt ("--cg_steps");
    }
    else
    {
      stepsPerRound = 2;
    }


    if (args.hasArg ("--cg_steps_final"))
    {
      stepsOnLastRound = args.getUInt ("--cg_steps_final");
    }
    else
    {
      stepsOnLastRound = 40;
    }
  
 //--------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
 if (nclusters > 0)
 {  
  
  DistanceMatrix clusterdists(nclusters);
  double maxclusterdist = 0;
  for (i = nclusters-1; i >=0; i--)
  {
    for (j = nclusters-1; j > i; j--)
    {
      double tempdist = 0;
      for (k = nXdim-1; k >=0; k--)
      {
	double tempdiff = clusterXmeans[i][k] - clusterXmeans[j][k];
	tempdist += tempdiff*tempdiff;
      }
      tempdist = sqrt(tempdist);
      clusterdists(i,j) = tempdist;
      clusterdists(j,i) = tempdist;
      if (tempdist > maxclusterdist)
	maxclusterdist = tempdist;
    }
    clusterdists(i,i) = 0;
  }
  DataSet projData_clusters;
  printf("seed = %u\n", seed);
  RandomDataGenerator randgen (nclusters, nYdim, 1, 1, seed);
  randgen.loadData (projData_clusters);
  DynamicDouble nerv_initialRadius (maxclusterdist/2, 0);
  GoldenSectionSearch nerv_linesearch;
  int lastNeighbor_clusters = nclusters * 0.25;   // Currently just set as a fixed proportion, 25% of all the clusters
  int stepsPerRound_clusters = stepsPerRound;
  int stepsOnLastRound_clusters = stepsOnLastRound;
  int iterations_clusters = iterations;
  std::string tmpoutputfile_clusters;
  std::vector<double> nerv_weights(nclusters, 1);  
  NeRVCostFunction nerv_costFunc (clusterdists, projData_clusters, nerv_linesearch, nerv_initialRadius, lambda, lastNeighbor_clusters, nerv_weights, std::cout);
  nerv_optStep = new ConjugateGradientOpt(nerv_costFunc, nerv_linesearch, std::cerr);    
  NeRVOptStrat nerv_optStrategy(iterations_clusters, stepsPerRound_clusters, stepsOnLastRound_clusters, tmpoutputfile_clusters);
  nerv_optStrategy.optimize (clusterdists, projData_clusters, *nerv_optStep, nerv_costFunc, std::cerr);
  
  
  RandomDataGenerator randgentemp (origData.getRows(), outputDimension, 1, 1, seed);
  randgentemp.loadData (projData);        
  //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  
    
   
  //Interpolate between cluster centroids to create an initialisation for approximated NeRV
  double tempsum;
  double jitter;
  
  for (i = 0; i < ndata; i++)
  {
    for (j = 0; j < nYdim; j++)
    { 
      tempsum = 0;
      for (k = 0; k < nonzeroMemberscounter[i] /*nclusters*/; k++)
      {
	//tempsum += clusterMemberships[i][k]* projData_clusters(k, j);
	
	tempsum += denseMemberships[i][k]* projData_clusters(memb2clusterind[i][k], j);

      }
     jitter = ((float)rand()/RAND_MAX); 
     projData(i, j) = tempsum + jitter*(0.025*tempsum);
    }
  }
  
  if (projData.getRows() != ndata)
  {
    printf("Error: number of rows %d in projected data does not match number of rows %d in original data!\n", projData.getRows(), ndata);
    exit(1);
  } 
  
 } 

 

    DynamicDouble initialRadius (initrad, 0);

    GoldenSectionSearch linesearch;
    
    geaNeRVCostFunction costFunc (projData, linesearch,  // costFunc is provided with projData and lambda --- I also need origData
                               initialRadius, lambda, lastNeighbor,
                               std::cerr);
    
  
  printf("Converting provided neighbor and cluster information\n");
  

  // Provide them to the cost function computation code
  printf("ndata: %d nXdim: %d nYdim: %d nclusters :%d", ndata, nXdim, nYdim, nclusters);
  
  costFunc.provideClusterStatistics
  (
    ndata, nXdim, nYdim, nclusters,
    X,
    clusterIndices, 
    denseMemberships,
    nonzeroMemberscounter,
    memb2clusterind,
    clusterXmeans,
    clusterXcovs,
    nexact,
    exact
  );
  
  printf("Checkpoint 2 \n");
    
  
  
    if (args.hasArg("--record"))
    {
      std::string filename_stem;
      args.getString("--record", filename_stem);

      optStep = new ConjugateGradientOpt(costFunc, linesearch, std::cerr,
                                     filename_stem);
    }
    else
    {
      optStep = new ConjugateGradientOpt(costFunc, linesearch, std::cerr);
    }

    geaNeRVOptStrat optStrategy (iterations, stepsPerRound, stepsOnLastRound,
        tmpoutputfile);

    optStrategy.geanervoptimize (projData, *optStep, costFunc,
                          std::cerr);

    printf("Exporting data\n");
    exporter.exportData (projData);
    printf("Exporting data done\n");

  }


  catch (Exception & exc)
  {
    std::cerr << "Error: " << exc.getErrMsg () << std::endl;
  }

  printf("After try-catch block\n");

  printf("Deleting optimization object\n");
  delete optStep;
  printf("Deleting optimization object done, exiting\n");
  
  clock_t ends = clock();
  printf("Running Time : %e \n", (double) (ends - start) / CLOCKS_PER_SEC);
  
  return 0;
}


    
    
    
    
    
    
    
