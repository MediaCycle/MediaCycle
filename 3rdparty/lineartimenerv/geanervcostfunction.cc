/*Copyright (C) Kristian Nybo, Jarkko Venna, 
 * Modified for conditional t-nerv by Jaakko Peltonen
 * Modified for lineartime nerv by Konstantinos Georgatzis
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "geanervcostfunction.hh"

#include "euclidean.hh"
#include "euclideansquared.hh"

#include "sompackexporter.hh"
#include <string>

#include <cmath>
#include <limits>

#include <iostream>
#include <sstream>
#include <float.h>
#include <curses.h>
#include <stdlib.h>

#include <newmat.h>
#include <newmatap.h>

namespace nm = NEWMAT;
using namespace NEWMAT;

#define PI 3.1415926535897932384626433832795


#define mincov (1e-12)

#include "costapprox.cc"
#include "gradapprox.cc"

#define TEMPEPSILON (1e-9)

#define USE_TDISTRIBUTION 1

#define min_covdiag (1e-6)

int geanerv_printcostfunctionterms = 0;

void
geaNeRVCostFunction::calculateFinalNeighborhoods (int effectiveNeighbors)
{
  double tolerance, stepsize;
  int stepdirection, goodenough;
  double sigma2_all;
  double cost4, cost6, meanentropy;
  double maxsigma2;
  double minsigma2;
  int i;
  
  
  tolerance=1e-6;
  
  stepsize=1;      
  stepdirection=1;

  minsigma2 = 0;
  maxsigma2 = DBL_MAX;
  
  sigma2_all = 1;
  goodenough=0;

  while(goodenough==0)
  {
    for  (i = ndata-1; i >= 0; i--)
    {
      sigmaSqrdArray[i] = sigma2_all*0.5; // multiplication by 0.5 is for compatibility with old NeRV code which had only 1/sigma^2 inside exponents whereas we have 1/(2*sigma^2)
    }  
    
    cost4 = compute_sumlogKx(X, sigmaSqrdArray, clusterIndices, clusterXmeans, clusterXcovs, invCovX, exact, exactXdists, Kx_values, tempclustersizes, nclusters, nexact, ndata, nXdim, &(invcovTempPointer), invCovType1, detType1);  
    
    cost6 = compute_sumRsimple(X, sigmaSqrdArray, clusterXmeans, clusterXcovs, invCovX, exact, exactXdists, Kx_values, tempclustersizes, nclusters, nexact, ndata, nXdim, &(invcovTempPointer), invCovType1, invCovType2, detType1);
    
    meanentropy = (-1)*(cost6 - cost4)/ndata;
#ifdef DEBUGMODE  
    printf("Trying value sigma2_all = %e, stepsize %e, meanentropy %e, %d effective neighbors, log(neffectiveneighbors) %e, cost4 %e, cost6 %e\n", sigma2_all, stepsize, meanentropy, effectiveNeighbors, log(effectiveNeighbors), cost4, cost6);
#endif    
    
    if (fabs(meanentropy-log(effectiveNeighbors)) < tolerance)
      goodenough=1;
       
    if (goodenough==0)
    {
      if ((isnan(meanentropy)) || (meanentropy<log(effectiveNeighbors)))
      {
	if (sigma2_all > minsigma2)
	  minsigma2 = sigma2_all;

        if (sigma2_all+stepsize >= maxsigma2)
	  stepsize = 0.5*(sigma2_all + maxsigma2) - sigma2_all;
	
        sigma2_all=sigma2_all+stepsize;
		
        if (stepdirection==1) 
	  stepsize=stepsize*1.5; 
        else 
	  stepsize=stepsize/1.5;
        stepdirection=1;
      }
      else
      {
	if (sigma2_all < maxsigma2)
	  maxsigma2 = sigma2_all;	
	
        if (sigma2_all-stepsize <= minsigma2)
	  stepsize = sigma2_all - 0.5*(sigma2_all + minsigma2);

        sigma2_all=sigma2_all-stepsize;	
	
        if (stepdirection==-1) 
	  stepsize=stepsize*1.5; 
        else 
	  stepsize=stepsize/1.5;
        stepdirection=-1;
      }   
    }
  }

  for  (i = ndata-1; i >= 0; i--)
  {
    finalNeighborhoods[i] = sigma2_all;
  }  
   
  for (int i = (sigmaSqrd.size())-1; i >= 0; i--)
  {
    if (2.0 * radius.value() * radius.value() > finalNeighborhoods[i])
    {
      sigmaSqrd[i] = 2.0 * radius.value() * radius.value();
    }
    else
    {
      sigmaSqrd[i] = finalNeighborhoods[i];
    }
  }
  printf("final SigmaSqrd value : %.15e\n", finalNeighborhoods[0]);
}

void
geaNeRVCostFunction::updateDynamicParameters(size_t currentRound,
    size_t totalRounds,
    const DataMatrix & projData)
{ 
  radius.update(currentRound, totalRounds);

  for (int i = (sigmaSqrd.size())-1; i >= 0; i--)
  {
    if (2.0 * radius.value() * radius.value() > finalNeighborhoods[i])
    {
      sigmaSqrd[i] = 2.0 * radius.value() * radius.value();
    }
    else
    {
      sigmaSqrd[i] = finalNeighborhoods[i];
    }
  }

  // Copy contents of provided sigmaSqrd structure into array sigmaSqrdArray for convenience
  for  (int i = ndata-1; i >= 0; i--)
  {
    sigmaSqrdArray[i] = sigmaSqrd[i]*0.5; // multiplication by 0.5 is for compatibility with old NeRV code which had only 1/sigma^2 inside exponents whereas we have 1/(2*sigma^2)
  }  
  
   for (int i = ndata-1; i >= 0; i--)
   {
     Kx_values[i] = 0;
   }

  // This call also computes "Kx_values", which are then used by the later functions 
  stored_cost4 = compute_sumlogKx(X, sigmaSqrdArray, clusterIndices, clusterXmeans, clusterXcovs, invCovX, exact, exactXdists, Kx_values, tempclustersizes, nclusters, nexact, ndata, nXdim, &(invcovTempPointer), invCovType1, detType1);  
  
  stored_cost6 = compute_sumRsimple(X, sigmaSqrdArray, clusterXmeans, clusterXcovs, invCovX, exact, exactXdists, Kx_values, tempclustersizes, nclusters, nexact, ndata, nXdim, &(invcovTempPointer), invCovType1, invCovType2, detType1);
}


void
geaNeRVCostFunction::updateLowDimensionalClusters()
{
  int i, j, k, l, m; 
 
  if (nclusters > 0)
  {
   for (i = nclusters-1; i >= 0; i--)
   {     
     for (j=nYdim-1;j>=0;j--)
     {
       clusterYmeans[i][j] = 0;
       for (k=nYdim-1;k>=0;k--)
	 clusterYcovs[i][j][k] = 0;
     }
   }    

   for (i = ndata-1; i >= 0; i--)
   {     
     for (k = nonzeroMemberscounter[i]-1; k >= 0; k--)
     {
       l = memb2clusterind[i][k];
       for (j=nYdim-1;j>=0;j--)
       { 
         clusterYmeans[l][j] += Y[i][j]*denseMemberships[i][k];
       } 
     }
   }

   // Normalize cluster means  in low-dimensional space
   for (i = nclusters-1; i >= 0; i--)
   {
     for (j=nYdim-1;j>=0;j--)
     {
       if (clustersizes[i] > 0)
       {
         clusterYmeans[i][j] /= clustersizes[i];
       }
     }  
   } 

   for (i = ndata-1; i >= 0; i--)
   {
     for (l = nonzeroMemberscounter[i]-1; l >= 0; l--)
     { 
       m = memb2clusterind[i][l];
       for (j=nYdim-1;j>=0;j--)
         for (k=nYdim-1;k>=0;k--)
	 {
	   if (j == k)
             clusterYcovs[m][j][k] += (Y[i][j] - clusterYmeans[clusterIndices[i]][j])*(Y[i][k] - clusterYmeans[clusterIndices[i]][k])*denseMemberships[i][l] + min_covdiag;
	   else
	     clusterYcovs[m][j][k] += (Y[i][j] - clusterYmeans[clusterIndices[i]][j])*(Y[i][k] - clusterYmeans[clusterIndices[i]][k])*denseMemberships[i][l];
	 }  
     }
    
   }

   // Normalize cluster covariances  in low-dimensional space
   for (i = nclusters-1; i >= 0; i--)
   {
     if (clustersizes[i] > 0)
     {
       for (j=nYdim-1;j>=0;j--)
         for (k=nYdim-1;k>=0;k--)
           clusterYcovs[i][j][k] /= clustersizes[i];
     }
     else
     {
       // If cluster has only one or zero occupants, set covariance to a small diagonal
       for (j=nYdim-1;j>=0;j--)
         for (k=nYdim-1;k>=0;k--)
         {
  	   if (j == k)
	     clusterYcovs[i][j][k] = mincov;
	   else
	     clusterYcovs[i][j][k] = 0;
         }
     }
   }
 } 
}


double
geaNeRVCostFunction::evaluate(const DataMatrix & projData)
{
  double cost1, cost2, cost3, cost4, cost5, cost6, cost;
  int i, j;
  extern int iteration;

  
  // Copy contents of provided projData structure into matrix Y for convenience
  for  (i = ndata-1; i >= 0; i--)
  {
    for  (j = nYdim-1; j >= 0; j--)
    {
      Y[i][j] = projData(i,j);
    }
  }
  
  updateLowDimensionalClusters();

  compute_exactdistsDisplay
  (
    Y, exact, nexact, ndata, nYdim, exactdists  
  );
  
  for (i = ndata-1; i >= 0; i--)
  {
     Ky_values[i] = 0;
     Psimple_values[i] = 0;
     Pcross_values[i] = 0;
   }  

  //-------------------------------------
  // Compute cost function into "cost"
  //-------------------------------------
  // This call also computes "Ky_values", which are then used by the later functions 
  cost1 = compute_sumlogKy(Y, sigmaSqrdArray, clusterYmeans, clusterYcovs, exact, exactdists, Ky_values, tempclustersizes, nclusters, nexact, ndata, nYdim);
  
  // This call also computes "Psimple_values", which is here not needed further (it would be needed if we were computing the gradient)
  cost2 = compute_sumPsimple(Y, sigmaSqrdArray, clusterYmeans, clusterYcovs, exact, exactdists, Ky_values, Psimple_values, tempclustersizes, nclusters, nexact, ndata, nYdim);
  
  // This call also computes "Pcross_values", which is here not needed further (it would be needed if we were computing the gradient)
  cost3 = compute_sumPcross(Y, X, sigmaSqrdArray, clusterYmeans, clusterYcovs, clusterXmeans, clusterXcovs, exact, exactdists, exactXdists, Ky_values, Pcross_values, tempclustersizes, nclusters, nexact, ndata, nYdim, nXdim);

  cost4 = stored_cost4;
 
  cost5 = compute_sumRcross(Y, X, sigmaSqrdArray, clusterYmeans, clusterYcovs, clusterXmeans, clusterXcovs, invCovX, exact, exactdists, exactXdists, Kx_values, tempclustersizes, nclusters, nexact, ndata, nYdim, nXdim, invCovType1, detType1);

  cost6 = stored_cost6;

  cost = lambda * (cost6 - cost4 - cost5 + cost1) + (1 - lambda) * (cost2 - cost1 - cost3 + cost4);

  feedback << "Current cost: " << cost << std::endl;
 
  printf("Iteration No. : %d ", iteration);

  iteration++;

  if (geanerv_printcostfunctionterms == 1)
    printf("\n");

  return cost;
  
}

void
geaNeRVCostFunction::updateDataRepresentation(const DataMatrix & newData)
{
  throw std::runtime_error("LinearNeRV Error: updateDataRepresentation is obsolete and should not be called!\n");
  //exit(1);
}

double
geaNeRVCostFunction::getGradient(const DataMatrix & projData,
                              DataMatrix & gradient)
{
  int i, j;
  static double **gterm1 = NULL, **gterm2 = NULL, **gterm3 = NULL, **gterm4 = NULL;
  
  if (gterm1 == NULL)
  {
    gterm1 = (double **) malloc(ndata*sizeof(double *));
    gterm2 = (double **) malloc(ndata*sizeof(double *));
    gterm3 = (double **) malloc(ndata*sizeof(double *));
    gterm4 = (double **) malloc(ndata*sizeof(double *));  
    for (i = 0; i < ndata; i++)
    {
     gterm1[i] = (double *) malloc(nYdim*sizeof(double));
     gterm3[i] = (double *) malloc(nYdim*sizeof(double));
     gterm2[i] = (double *) malloc(nYdim*sizeof(double));
     gterm4[i] = (double *) malloc(nYdim*sizeof(double));
    }
  }
  
  
  // Copy contents of provided projData structure into matrix Y for convenience
  for  (i = ndata-1; i >= 0; i--)
  {
    for  (j = nYdim-1; j >= 0; j--)
    {
      Y[i][j] = projData(i,j);
    }
  }

  // Update clusters in low-dimensional space based on projData
  updateLowDimensionalClusters();

  compute_exactdistsDisplay
  (
    Y, exact, nexact, ndata, nYdim, exactdists  
  );
  
  // Compute gradient into "gradient"

  compute_sumlogKy(Y, sigmaSqrdArray, clusterYmeans, clusterYcovs, exact, exactdists, Ky_values, tempclustersizes, nclusters, nexact, ndata, nYdim);

  compute_sumPsimple(Y, sigmaSqrdArray, clusterYmeans, clusterYcovs, exact, exactdists, Ky_values, Psimple_values, tempclustersizes, nclusters, nexact, ndata, nYdim);

  compute_sumPcross(Y, X, sigmaSqrdArray, clusterYmeans, clusterYcovs, clusterXmeans, clusterXcovs, exact, exactdists, exactXdists, Ky_values, Pcross_values, tempclustersizes, nclusters, nexact, ndata, nYdim, nXdim);

  compute_gradterm1(Y, sigmaSqrdArray, clusterYmeans, clusterYcovs, exact, exactdists, Ky_values, Kx_values, Psimple_values, Pcross_values, gterm1, tempclustersizes, nclusters, nexact, ndata, nYdim, lambda);

  compute_gradterm2(Y, X, sigmaSqrdArray, clusterIndices, denseMemberships, nonzeroMemberscounter, memb2clusterind, clusterYmeans, clusterYcovs, clusterXmeans, clusterXcovs, exact, exactdists, exactXdists, nexactqueries, exactqueries, exactqueries_neighbororder, Ky_values, Kx_values, Psimple_values, Pcross_values, gterm2, tempclustersizes, nclusters, nexact, ndata, nYdim, nXdim, lambda);

  compute_gradterm3(Y, X, sigmaSqrdArray, clusterYmeans, clusterYcovs, clusterXmeans, clusterXcovs, exact, exactdists, exactXdists, Ky_values, Kx_values, Psimple_values, Pcross_values, gterm3, tempclustersizes, nclusters, nexact, ndata, nYdim, nXdim, lambda, invCovType1, detType1);

  compute_gradterm4(Y, X, sigmaSqrdArray, clusterIndices, denseMemberships, nonzeroMemberscounter, memb2clusterind, clusterYmeans, clusterYcovs, clusterXmeans, clusterXcovs, exact, exactdists, exactXdists, nexactqueries, exactqueries, exactqueries_neighbororder, Ky_values, Kx_values, Psimple_values, Pcross_values, gterm4, tempclustersizes, nclusters, nexact, ndata, nYdim, nXdim, lambda, &(invcovTempPointer), invCovType1, detType1);

  for(i = ndata-1; i >= 0; i--)
  {
    for(j = nYdim-1; j >= 0; j--)
    {
      gradient(i, j) = gterm1[i][j] + gterm2[i][j] + gterm3[i][j] + gterm4[i][j];  
    }
  }

  double sqsum=0.0;
  int n=gradient.getRows();
  int dim=gradient.getCols();

  for (int t = n-1; t >= 0; t--)
  {
    for (int d = dim-1; d >= 0; d--)
    {
      sqsum += (gradient (t, d) * gradient(t,d)); 
    }
  }

  std::cout << "gradient squared norm " << sqsum << std::endl;


  // Constrain very large gradient norms
  double normalizertemp = sqrt(sqsum/(n*dim));
  for (int t = n-1; t >= 0; t--)
  {
    for (int d = dim-1; d >= 0; d--)
    {
      gradient (t, d) = gradient(t,d) / normalizertemp; 
    }
  }
  
  return sqsum;
}

void
geaNeRVCostFunction::provideClusterStatistics
(
  int provided_ndata, int provided_nXdim, int provided_nYdim, int provided_nclusters,
  double **provided_X,
  int *provided_clusterIndices,
  double **provided_denseMemberships,
  int *provided_nonzeroMemberscounter,
  int **provided_memb2clusterind,
  double **provided_clusterXmeans,
  double ***provided_clusterXcovs,
  int provided_nexact,
  int **provided_exactIndices
)
{
  int i,j,k,l;

  
  ndata = provided_ndata;
  nXdim = provided_nXdim;
  nYdim = provided_nYdim;  
  nclusters = provided_nclusters;

  X = (double **)malloc(ndata*sizeof(double *));  
  clusterIndices = (int *)malloc(ndata*sizeof(int));
  
  for (i = ndata-1; i >= 0; i--)
  {  
    X[i] = (double *)malloc(nXdim*sizeof(double));
    for (j = nXdim-1; j >= 0; j--)
    { 
      X[i][j] = provided_X[i][j];
    }
    clusterIndices[i] = provided_clusterIndices[i];
  }
   
  nonzeroMemberscounter = (int *)malloc(ndata*sizeof(int));
  for (i = ndata-1; i >= 0; i--)
  {
    nonzeroMemberscounter[i] = provided_nonzeroMemberscounter[i];
  }

  memb2clusterind = (int **)malloc(ndata*sizeof(int *));
  for (i = ndata-1; i >= 0; i--)
  {
    memb2clusterind[i] = (int *)malloc(nonzeroMemberscounter[i]*sizeof(int));
    for (j = nonzeroMemberscounter[i]-1; j >= 0; j--)
      memb2clusterind[i][j] = provided_memb2clusterind[i][j];
  }

  denseMemberships = (double **)malloc(ndata*sizeof(double *));
  for (i = ndata-1; i >= 0; i--)
  {
    denseMemberships[i] = (double *)malloc(nonzeroMemberscounter[i]*sizeof(double));
    for (j = nonzeroMemberscounter[i]-1; j >= 0; j--)
      denseMemberships[i][j] = provided_denseMemberships[i][j];
  }

  nexact = provided_nexact;
  exact = (int **)malloc(ndata*sizeof(int *));
  for (i = ndata-1; i >= 0; i--)
  {
    exact[i] = (int *)malloc(nexact*sizeof(int));
    for (j = nexact-1; j >= 0; j--)
      exact[i][j] = provided_exactIndices[i][j];
  }

  exactdists = (double **) malloc(ndata*sizeof(double *)); 
  for (i = ndata-1; i >= 0; i--)
  {
   exactdists[i] = (double *) malloc(nexact*sizeof(double));
  }
  exactXdists = (double **) malloc(ndata*sizeof(double *)); 
  for (i = ndata-1; i >= 0; i--)
  {
   exactXdists[i] = (double *) malloc(nexact*sizeof(double));
  }

  compute_exactdistsOrig
  (
    X, exact, nexact, ndata, nXdim, exactXdists
  );

  Y = (double **)malloc(ndata*sizeof(double *));  
  for (i = ndata-1; i >= 0; i--)
  {  
    Y[i] = (double *)malloc(nYdim*sizeof(double));
    for (j = nYdim-1; j >= 0; j--)
    { 
      Y[i][j] = 0; // Will be provided later in the code for each cost function or gradient evaluation
    }
  }

  sigmaSqrdArray = (double *)malloc(ndata*sizeof(double));
  for (i = ndata-1; i >= 0; i--)
  {
    sigmaSqrdArray[i] = 0;
  }

  clustersizes = (double *)malloc(nclusters*sizeof(double));
  for (i = 0; i < nclusters; i++)
  {
    clustersizes[i] = 0;
  }
  if (nclusters > 0)
  {
    for (i = 0; i < ndata; i++)
    {
      for (j = 0; j < nonzeroMemberscounter[i]; j++)
      {
	l = memb2clusterind[i][j];
        clustersizes[l] += denseMemberships[i][j];
      }
    }
  }

  tempclustersizes = (double **)malloc(ndata*sizeof(double *));
  for (i = ndata-1; i >= 0; i--)
  {
    tempclustersizes[i] = (double *)malloc(nclusters*sizeof(double));
    for (k = nclusters-1; k >= 0; k--)
    {    
      tempclustersizes[i][k] = clustersizes[k];
    }
    if (nclusters > 0)
    {
      // remove membership of query point from all clusters
      for (k = nonzeroMemberscounter[i]-1; k >= 0; k--)
      { 
	l = memb2clusterind[i][k];
        tempclustersizes[i][l] -= denseMemberships[i][k];
      }	
      // remove membership of every exact neighbour of each query point from every cluster 
      for (j = nexact-1; j >= 0; j--)
      {  
        int n = exact[i][j];
	for (k = nonzeroMemberscounter[n]-1; k >= 0; k--)
	{ 
	  l = memb2clusterind[n][k];
          tempclustersizes[i][l] -= denseMemberships[n][k];
	}  
      }
     }	
   }

  // Initialise the rest of the cluster statistics: the original-space statistics are provided, the low-dimensional statistics are recomputed when needed
  clusterXmeans = (double **)malloc(nclusters*sizeof(double *));
  clusterYmeans = (double **)malloc(nclusters*sizeof(double *));
  clusterXcovs = (double ***)malloc(nclusters*sizeof(double **));
  clusterYcovs = (double ***)malloc(nclusters*sizeof(double **));
  invCovX = (double ***)malloc(nclusters*sizeof(double **));
  for (i = nclusters-1; i >= 0; i--)
  {  
    clusterXmeans[i] = (double *)malloc(nXdim*sizeof(double));
    clusterXcovs[i] = (double **)malloc(nXdim*sizeof(double *));
    invCovX[i] = (double **)malloc(nXdim*sizeof(double *));
    for (j = nXdim-1; j >= 0; j--)
    { 
      clusterXmeans[i][j] = provided_clusterXmeans[i][j]; 
      clusterXcovs[i][j] = (double *)malloc(nXdim*sizeof(double));
      invCovX[i][j] = (double *)malloc(nXdim*sizeof(double));
      for (k = nXdim-1; k >= 0; k--)
      {
 	clusterXcovs[i][j][k] = provided_clusterXcovs[i][j][k];
 	invCovX[i][j][k] = 0; // Will be computed properly later in the code
      }
    }    
    clusterYmeans[i] = (double *)malloc(nYdim*sizeof(double));
    clusterYcovs[i] = (double **)malloc(nYdim*sizeof(double *));
    for (j = nYdim-1; j >= 0; j--)
    { 
      clusterYmeans[i][j] = 0; // Will be computed properly later in the code
      clusterYcovs[i][j] = (double *)malloc(nYdim*sizeof(double));           
      for (k = nYdim-1; k >= 0; k--)
      {
 	clusterYcovs[i][j][k] = 0; // Will be computed properly later in the code
      }
    }
  }

  // Initialise the inverse covariance matrices for each cluster
  invCovType1 = (double ***)malloc(nclusters*sizeof(double **));
  invCovType2 = (double ***)malloc(nclusters*sizeof(double **));  
  for (j = nclusters-1; j >= 0; j--)
  {
    invCovType1[j] = (double **)malloc(nXdim*sizeof(double *));
    invCovType2[j] = (double **)malloc(nXdim*sizeof(double *));  
    for (k = nXdim-1; k >= 0; k--)
    {
      invCovType1[j][k] = (double *)malloc(nXdim*sizeof(double));
      invCovType2[j][k] = (double *)malloc(nXdim*sizeof(double));  
      for (l = nXdim-1; l >= 0; l--)
      {
        invCovType1[j][k][l] = 0; // Will be computed later in the code
        invCovType2[j][k][l] = 0; // Will be computed later in the code	
      }      
    }
  }

  // Initialize the determinants for each cluster
  detType1 = (double *)malloc(nclusters*sizeof(double));
  for (j = nclusters-1; j >= 0; j--)
  {
    detType1[j] = 0; // Will be computed later in the code
  }  


  printf("Computing exact queries\n");
  nexactqueries = (int *)malloc(ndata*sizeof(int));
  exactqueries = (int **)malloc(ndata*sizeof(int *));
  exactqueries_neighbororder = (int **)malloc(ndata*sizeof(int *));
  for (i = ndata-1; i >= 0; i--)
  {
    nexactqueries[i] = 0;
    exactqueries[i] = NULL;
    exactqueries_neighbororder[i] = NULL;
  }

  for (i = ndata-1; i >= 0; i--)  
  {
    for (j = nexact-1; j >= 0; j--)
    {
      k = exact[i][j];
      nexactqueries[k] = nexactqueries[k] + 1;
    }
  }
  for (i = ndata-1; i >= 0; i--)  
  {
    exactqueries[i] = (int *)malloc(nexactqueries[i]*sizeof(int));
    exactqueries_neighbororder[i] = (int *)malloc(nexactqueries[i]*sizeof(int));
  }
 
  for (i = ndata-1; i >= 0; i--)  
  {
    nexactqueries[i] = 0;
  } 
  
  for (i = ndata-1; i >= 0; i--)  
  {
    for (j = nexact-1; j >= 0; j--)
    {
      k = exact[i][j];
      nexactqueries[k] = nexactqueries[k] + 1;    
      exactqueries[k][nexactqueries[k]-1] = i;
      exactqueries_neighbororder[k][nexactqueries[k]-1] = j;
    }
  }
 
  computeCovMatrixInverses
  (
    clusterXcovs, invCovX, nclusters, nXdim,
    &(invcovTempPointer)  
  );

  Kx_values = (double *)malloc(ndata*sizeof(double));
  Ky_values = (double *)malloc(ndata*sizeof(double));
  Psimple_values = (double *)malloc(ndata*sizeof(double));
  Pcross_values = (double *)malloc(ndata*sizeof(double));

  printf("Calculating final neighborhoods for %d desired effective neighbors\n", desiredEffectiveNeighbors);    
  
  calculateFinalNeighborhoods(desiredEffectiveNeighbors);  
  printf("provideClusterStatistics done\n");    
}



geaNeRVCostFunction::geaNeRVCostFunction(
                                   DataMatrix & projData,
                                   LineSearch & linesearch,
                                   DynamicDouble radius,
                                   double lambda, int neighborhoodSize,
                                   std::ostream & feedback):
    radius(radius),
    lambda(lambda),
    gamma(1-lambda),
    linesearch(linesearch),   
    ndata(-1), 
    nXdim(-1), 
    nYdim(-1), 
    nclusters(-1), 
    nexact(-1),
    X(NULL),
    Y(NULL),
    clusterIndices(NULL), 
    denseMemberships(NULL),
    nonzeroMemberscounter(NULL),
    memb2clusterind(NULL),   
    clustersizes(NULL),
    clusterXmeans(NULL),
    clusterXcovs(NULL),
    clusterYmeans(NULL),
    clusterYcovs(NULL),
    exact(NULL),
    invCovX(NULL),
    sigmaSqrdArray(NULL),
    exactqueries(NULL),
    exactqueries_neighbororder(NULL),
    nexactqueries(NULL),
    Kx_values(NULL),
    Ky_values(NULL),
    Psimple_values(NULL), 
    Pcross_values(NULL),
    exactdists(NULL),
    exactXdists(NULL),
    stored_cost4(-1),
    stored_cost6(-1),
    invCovType1(NULL),  
    invCovType2(NULL),
    detType1(NULL),  
    invcovTempPointer(NULL),
    tempclustersizes(NULL),
    desiredEffectiveNeighbors(neighborhoodSize),
    finalNeighborhoods(projData.getRows(),0),
    sigmaSqrd(projData.getRows()),
    feedback(feedback)
{    
  printf("Initialized effective neighbors to %d (%d)\n", desiredEffectiveNeighbors, neighborhoodSize);
}

void geaNeRVCostFunction::reportParameters(std::string& target)
{
  std::ostringstream params;
  params << "Lambda: " << lambda << "\nCurrent radius: " << radius.value()
    << "\n";

  target = params.str();
}
