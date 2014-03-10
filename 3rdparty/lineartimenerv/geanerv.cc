/*Copyright (C) Kristian Nybo, Jarkko Venna
 * Modified for tcNeRV by Jaakko Peltonen
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <float.h>

//#include <newmat.h>
//#include <newmatap.h>
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

//#include "costapprox.cc"
//#include "gradapprox.cc"

#include "clustersandexacts.c"


int
main (int argc, char **argv)
{

  clock_t start = clock();  
  
  int nclusters, ndata, nexact, nYdim, nXdim;
  DataSet projData;
  DataSet origData;
  ConjugateGradientOpt *optStep = NULL;

  //DistanceMatrix origDataDist;
  DataSet provided_clusterIndices;
  DataSet provided_allclusterXmeans;
  DataSet provided_allclusterXcovs;
  int provided_nexact;
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

    /*
    if (args.hasArg ("--inputdist"))
    {
      args.getString ("--inputdist", inputfile);

      DistanceMatrixLoader dmloader;
      dmloader.loadMatrix(inputfile, origDataDist);
    }
    else
    {            
      args.getString ("--inputfile", inputfile);

      SOMPackLoader loader (inputfile);
      loader.loadData (origData);

      Euclidean metric;

      DistanceMatrix dm (origData, metric);
      origDataDist = dm;
    }
    */
    
    if (args.hasArg ("--inputfile"))
    {            
      args.getString ("--inputfile", inputfile);

      SOMPackLoader loader (inputfile);
      loader.loadData (origData);

      ndata = origData.getRows();
      printf("read ndata %d\n", ndata);
      nXdim = origData.getCols();
      printf("read nXdim %d\n", nXdim);

      /*
      Euclidean metric;

      DistanceMatrix dm (origData, metric);
      origDataDist = dm;
      */
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
    
    

    // Read in or create the initial projected data
    if (args.hasArg ("--init"))
    {
      std::string initfile;
      args.getString ("--init", initfile);

      SOMPackLoader initloader (initfile);
      initloader.loadData (projData);
    }
    /*
    else
    {
      size_t outputDimension = 2;       // The default dimension 

      if (args.hasArg ("--outputdim"))
      {
        outputDimension = args.getUInt ("--outputdim");
      }

      if (args.hasArg ("--seed"))
      {
        RandomDataGenerator randgen (origDataDist.getRows (), outputDimension, 1, 1, args.getUInt ("--seed"));
        randgen.loadData (projData);
      }
      else
      {
        RandomDataGenerator randgen (origDataDist.getRows (), outputDimension, 1, 0, 0);
        randgen.loadData (projData);
      }      
    }
    */
    else if (args.hasArg("--randinit") || args.hasArg("--inputdist"))
    {
      size_t outputDimension = 2;       // The default dimension 

      if (args.hasArg ("--outputdim"))
      {
        outputDimension = args.getUInt ("--outputdim");
      }
      
      RandomDataGenerator randgen (origData.getRows(), outputDimension, 1, 1, seed);
      // RandomDataGenerator randgen (origData.getRows(), outputDimension, 1, 0, 0);
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
      //RandomDataGenerator randgen (origData.getRows(), outputDimension, 1, 0, 0);
      RandomDataGenerator randgen (origData.getRows(), outputDimension, 1, 1, seed);      
      randgen.loadData (projData);      
      // projData = DataSet(origData.getRows(), outputDimension);
    printf("Trying PCA initialization\n");
      pca.getComponents(projData);
    printf("PCA initialization ready\n");
    }

    printf("After initialization\n");
    
    
    
    nYdim = projData.getCols();
    /*if (projData.getRows() != ndata)
    {
      printf("Error: number of rows %d in projected data does not match number of rows %d in original data!\n", projData.getRows(), ndata);
      exit(1);
    }*/      
    if (nYdim != 2)
    {
      printf("Error: the code does not currently support output dimensionalities other than 2 (projected data was set to dimensionality %d)!\n", nYdim);
      exit(1);
    }      
    
    
#ifdef MATLAB
    // Read in cluster indices
    if (args.hasArg ("--clusterindicesfile"))
    {
      std::string clusterindicesfile;
      args.getString ("--clusterindicesfile", clusterindicesfile);

      std::cerr << "Loading cluster indices from file [" << clusterindicesfile << "]" << std::endl;

      SOMPackLoader loader (clusterindicesfile);
      loader.loadData (provided_clusterIndices);
      
      if (provided_clusterIndices.getRows() != ndata)
      {
	printf("Error: number of rows %d in cluster indices does not match number of rows %d in original data!\n", provided_clusterIndices.getRows(), ndata);
	exit(1);
      }
    }
    else
    {
      printf("Warning: clusterindicesfile has not been provided!\n");
      //nclusters = 0;
      //exit(1);
    }
#endif

#ifdef MATLAB
    // Read in cluster means in original space
    if (args.hasArg ("--clusterxmeansfile"))
    {
      std::string clusterxmeansfile;
      args.getString ("--clusterxmeansfile", clusterxmeansfile);

      std::cerr << "Loading clusterXmeans from file [" << clusterxmeansfile << "]" << std::endl;
      SOMPackLoader loader (clusterxmeansfile);
      loader.loadData (provided_allclusterXmeans);
      nclusters = provided_allclusterXmeans.getRows();
      
      if (provided_allclusterXmeans.getCols() != nXdim)
      {
	printf("Error: number of dimensions %d in cluster means does not match number of dimensions %d in original data!\n", provided_allclusterXmeans.getCols(), nXdim);
	exit(1);
      }      
    }
    else
    {
      printf("Warning: clusterxmeansfile has not been provided!\n");
      nclusters = 0;
      //exit(1);
    }
#endif

#ifdef MATLAB
    // Read in cluster covariances in original space
    if (args.hasArg ("--clusterxcovsfile"))
    {
      std::string clusterxcovsfile;
      args.getString ("--clusterxcovsfile", clusterxcovsfile);

      std::cerr << "Loading clusterXcovs from file [" << clusterxcovsfile << "]" << std::endl;
      SOMPackLoader loader (clusterxcovsfile);
      loader.loadData (provided_allclusterXcovs);
      
      if (provided_allclusterXcovs.getRows() != (nclusters*nXdim))
      {
	printf("Error: number of rows %d in cluster covs does not match number of clusters %d times the number of dimensions %d in original data!\n", provided_allclusterXcovs.getRows(), nclusters, nXdim);
	exit(1);
      }            
      
      if (provided_allclusterXcovs.getCols() != nXdim)
      {
	printf("Error: number of columns %d in cluster covs does not match the number of dimensions %d in original data!\n", provided_allclusterXcovs.getCols(), nXdim);
	exit(1);
      }      
    }
    else
    {
      printf("Warning: clusterxcovsfile has not been provided!\n");
      //exit(1);
    }
#endif

#ifdef MATLAB
    // Read in indices of exact neighbors
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
    }
    else
    {
      printf("Warning: exactneighborindicesfile has not been provided!\n");
      nexact = 0;
      //exit(1);
    }
    
#else

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
#endif



  // Create arrays etc. to hold cluster statistics   
  //int i, j, k;  
  int i, j, k ,l, m;
  int *clusterIndices;         // ndata * nclusters
  double *clustersizes;   	     // nclusters * 1
  double  **clusterXmeans;   // nclusters * nXdim
  double  **clusterYmeans;   // nclusters * nYdim
  double **clusterMemberships;
  // double ***invCovX;
  double **X;
  double ***clusterXcovs, ***clusterYcovs;   // nclusters * nYdim * nYdim, nclusters * nXdim * nXdim
  int **exact;
  double kmeansfinalcost;
  
  // Read in cluster statistics from files  
  
  
   clusterIndices = (int *)malloc(ndata*sizeof(int));
   clusterXmeans = (double **) malloc(nclusters*sizeof(double *));
   //invCovX = (double ***) malloc(nclusters*sizeof(double **));
  
  
  clustersizes = (double *) malloc(nclusters*sizeof(double));  
  clusterYmeans = (double **) malloc(nclusters*sizeof(double *));
  clusterXcovs = (double ***) malloc(nclusters*sizeof(double **));
  clusterYcovs = (double ***) malloc(nclusters*sizeof(double **));
   
 

  X = (double **)malloc(ndata*sizeof(double *));
  for (i = 0; i < ndata; i++)
  {
    X[i] = (double *)malloc(nXdim*sizeof(double));
    for (j = 0; j < nXdim; j++)
      X[i][j] = origData(i,j);
  }  

   //Uncomment for local k-means algorithm
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

  // Print exact neighbors to a file
  FILE *fileforclust;
  printf("exactindices opened\n");
  fileforclust = fopen("exactindices", "w");
  fprintf(fileforclust,"%d\n", nexact);
  for (k=0;k<ndata;k++)
  {
    for (l=0;l<nexact;l++)
    fprintf(fileforclust,"%d ", exact[k][l]);
  fprintf(fileforclust,"\n");
  }
  fclose(fileforclust);
  printf("exactindices closed\n");
   

  
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

#ifdef INTERPOLATE  
  //Interpolate between cluster centroids to create a decent initialisation for approximated NeRV
  double tempsum;
  double **tempInterpolateMatrix;
  
  tempInterpolateMatrix = (double **)malloc(ndata*sizeof(double *));
  for (i = 0; i < ndata; i++)
  {
    tempInterpolateMatrix[i] = (double *)malloc(nYdim*sizeof(double));
  }  
  
  for (i = 0; i < ndata; i++)
  {
    for (j = 0; j < nYdim; j++)
    { 
      tempsum = 0;
      for (k = 0; k < nclusters; k++)
      {
	tempsum += clusterMemberships[i][k]* projData(k, j); 
      }
     tempInterpolateMatrix[i][j] = tempsum; 
    }
  }
  
  for (i = 0; i < ndata; i++)
  {
    for (j = 0; j < nYdim; j++)
    {
     projData(i, j) = tempInterpolateMatrix[i][j]; 
    } 
  }
  
  if (projData.getRows() != ndata)
  {
    printf("Error: number of rows %d in projected data does not match number of rows %d in original data!\n", projData.getRows(), ndata);
    exit(1);
  } 
#endif

  fileforclust = fopen("temp_clustersizes", "w");
  fprintf(fileforclust,"%d\n", 1);
  for (k=0;k<nclusters;k++)
  {
    fprintf(fileforclust,"%e\n", clustersizes[k]);
  }  
  fclose(fileforclust);
  
  fileforclust = fopen("temp_clusterindices", "w");
  fprintf(fileforclust,"%d\n", 1);
  for (k=0;k<ndata;k++)
  {
    fprintf(fileforclust,"%e\n", clusterIndices[k]);
  }  
  fclose(fileforclust);
  
  fileforclust = fopen("temp_clustermeans", "w");
  fprintf(fileforclust,"%d\n", nXdim);
  for (k=0;k<nclusters;k++)
  {
    for (l=0;l<nXdim;l++)
      fprintf(fileforclust,"%e ", clusterXmeans[k][l]);
    fprintf(fileforclust,"\n");
  }
  fclose(fileforclust);
  
  fileforclust = fopen("temp_clustercovs", "w");
  fprintf(fileforclust,"%d\n", nXdim);
  for (k=0;k<nclusters;k++)
  {
    for (l=0;l<nXdim;l++)
    {
      for (m=0;m<nXdim;m++)
        fprintf(fileforclust,"%e ", clusterXcovs[k][l][m]);
      fprintf(fileforclust,"\n");
    }
  }
  fclose(fileforclust);
  
  fileforclust = fopen("temp_clustermemberships", "w");
  fprintf(fileforclust,"%d\n", nclusters);
  for (k=0;k<ndata;k++)
  {
    for (l=0;l<nclusters;l++)
      fprintf(fileforclust,"%e ", clusterMemberships[k][l]);
    fprintf(fileforclust,"\n");
  }
  fclose(fileforclust);
  
  
  
  
  
  // We do not scale data distances
  //origDataDist.scale (1.0 / origDataDist.getAverage ()); // what is this ?
  

  // Compute overall data scale (largest pairwise distance)
    double initrad = 0.0;
    double tempcompare = 0.0;
    int exactj;
    double tempTrace;
    int **tempexact;
    // Calculate intitial Radius
    if (args.hasArg ("--initrad"))
    {
      initrad = args.getDouble ("--initrad");
    }
    else
    {
      /*
      tempexact = (int **)malloc(ndata*sizeof(int *));
      for (i = ndata-1; i >= 0; i--)
      {
        tempexact[i] = (int *)malloc(nexact*sizeof(int));
        for (j = nexact-1; j >= 0; j--)
          {
          if (nexact > 0)
            {    
              tempexact[i][j] = provided_exactIndices(i,j);
            }
            if ((tempexact[i][j] < 0) || (tempexact[i][j] >= ndata))
            {
              printf("Error: index of the %d:th exact neighbor of data point %d is %d, which is outside the allowed range [0,...,%d] for %d data points!\n", j, i, tempexact[i][j], ndata-1, ndata);
              exit(1);
            }
          }
       }
       */
 
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
              //tempcompare = tempcompare + (X[i][l] - X[exactj][l]) * (X[i][l] - X[exactj][l]); 
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
             //exactj = tempexact[i][k];
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
       
#ifdef MATLAB       
       if (nclusters > 0)
       {
         for (i = ndata-1; i >= 0; i--)
         {
           for (k = nclusters-1; k >= 0; k--)
           {
             //exactj = tempexact[i][k];
             tempTrace = 0;
             tempcompare = 0;
             for (l = nXdim-1; l >= 0; l--)
             {
              tempTrace += provided_allclusterXcovs(k * nXdim + l, l);
              tempcompare = tempcompare + (origData(i, l) - provided_allclusterXmeans(k, l)) * (origData(i, l) - provided_allclusterXmeans(k, l));        
             }
             tempcompare += tempTrace / nXdim;
             if (tempcompare > initrad)
             {
              initrad = tempcompare;
             }
           } 
         }
       }
#endif       

      //initrad = origDataDist.getMax () / 2.0;
      initrad = sqrt(initrad/nXdim) / 2.0;
      std::cerr << "Initial radius " << initrad << std::endl;
      std::cerr << "Maximum distance " << initrad * 2 << std::endl;
      //printf("Initial radius has not been provided and will be calculated\n");
    }
    
#ifdef MATLAB 
     for (i = ndata-1; i >= 0; i--)
     {
       free(tempexact[i]); tempexact[i] = NULL;
     }  
     free(tempexact); tempexact = NULL;
#endif
              
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

    
      

    DynamicDouble initialRadius (initrad, 0);

    GoldenSectionSearch linesearch;
    
    /*
    geaNeRVCostFunction costFunc (origDataDist, projData, linesearch,  // costFunc is provided with projData and lambda --- I also need origData
                               initialRadius, lambda, lastNeighbor,
                               std::cerr);
    */
    geaNeRVCostFunction costFunc (projData, linesearch,  // costFunc is provided with projData and lambda --- I also need origData
                               initialRadius, lambda, lastNeighbor,
                               std::cerr);
    
  
  printf("Converting provided neighbor and cluster information\n");
  
  
#ifdef MATLAB
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
  
  
  
  if(nclusters > 0)
  {
    for (i = 0; i < ndata; i++)
    {
      clusterIndices[i] = provided_clusterIndices(i,0);
      if ((clusterIndices[i] < 0) || (clusterIndices[i] >= nclusters))
      {
        printf("Error: cluster index of data point %d is %d which is outside the allowed range [0,...,%d] for %d clusters!\n", i, clusterIndices[i], nclusters-1, nclusters);
        exit(1);
      }
    }
  }
  
  
  
  for (i = 0; i < nclusters; i++)
  {     
    // Means in original space, these should be given from Matlab and not computed here
    clusterXmeans[i] = (double *) malloc(nXdim*sizeof(double));
    for (j=0;j<nXdim;j++)
      clusterXmeans[i][j] = provided_allclusterXmeans(i,j);

    // Covariances in original space, these should be given from Matlab and not computed here
    clusterXcovs[i] = (double **) malloc(nXdim*sizeof(double *));
    for (j=0;j<nXdim;j++)
    {
      clusterXcovs[i][j] = (double *) malloc(nXdim*sizeof(double));
      for (k=0;k<nXdim;k++)
        clusterXcovs[i][j][k] = provided_allclusterXcovs(i*nXdim+j,k);
    }
  }
  
#endif 

  // Provide them to the cost function computation code
  printf("ndata: %d nXdim: %d nYdim: %d nclusters :%d", ndata, nXdim, nYdim, nclusters);
  
  costFunc.provideClusterStatistics
  (
    ndata, nXdim, nYdim, nclusters,
    X,
    clusterIndices,
    clusterMemberships,
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

    //DistanceMatrix origDataDist;
    optStrategy.geanervoptimize (/*origDataDist,*/ projData, *optStep, costFunc,
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

// ROSKAT

/*double tempcompare = 0;
 
 
 tempexact = (int **)malloc(ndata*sizeof(int *));
  for (i = 0; i < ndata; i++)
  {
    tempexact[i] = (int *)malloc(nexact*sizeof(int));
    for (j = 0; j < nexact; j++)
    {
      if (nexact > 0)
      {    
       tempexact[i][j] = provided_exactIndices(i,j);
      }
      if ((tempexact[i][j] < 0) || (tempexact[i][j] >= ndata))
      {
        printf("Error: index of the %d:th exact neighbor of data point %d is %d, which is outside the allowed range [0,...,%d] for %d data points!\n", j, i, exact[i][j], ndata-1, ndata);
        exit(1);
      }
    }
  }
 
 if ((nclusters && nexact) > 0)
 {
   for (i = 0; i < ndata; i++)
   {
     for (k = 0; k < nexact; k++)
     {
       exactj = tempexact[i][k];
       tempcompare = 0;
       for (l = 0; l < nXdim; l++)
       {
         tempcompare = tempcompare + fabs(X(i, l) - X(exactj, l));        
       }
       if (tempcompare > initrad)
       {
	 initrad = tempcompare;
       }
     } 
   }
 }    
 else
 {
  initrad = 10; 
 }
    
    
  exactdists = (double **) malloc(ndata*sizeof(double *)); 
  for (i = 0; i < ndata; i++)
  {
   exactdists[i] = (double *) malloc(nexact*sizeof(double));
   for (j = 0; j < nexact; j++)
   {
     exactdists[i][j] = 0;
   }
  }
  
  for (i = 0; i < ndata; i++)
  {       
   for (k = 0; k < nexact; k++)
   {
     exactj = exact[i][k];
     for (l = 0; l < nYdim; l++)
     {
       exactdists[i][k] = exactdists[i][k] + (Y[i][l] - Y[exactj][l])*(Y[i][l] - Y[exactj][l]);        
     }  
    }
  }*/ 
    
    
    
    
    
    
    