/*Copyright (C) Kristian Nybo, Jarkko Venna
 *Modified for lineartime nerv by Konstantinos Georgatzis
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef GEANERVCOSTFUNCTION_HH
#define GEANERVCOSTFUNCTION_HH

#include "linesearch.hh"
#include "distancematrix.hh"
#include "dataset.hh"
#include "costfunction.hh"
#include "dynamicdouble.hh"
#include "nervprobability.hh"

#define MIN_PROB 1.0e-200       /* The minimimum probability allowed. */

#include <iostream>
#include <vector>

/* NeRVCostFunction implements the cost function used in NeRV, as described
   in "Nonlinear dimensionality reduction as information retrieval" by
   Jarkko Venna and Samuel Kaski. */

class geaNeRVCostFunction:public CostFunction
{
private:
  /* radius is used as the gaussian when calculating input and output
     probabilities... */

  DynamicDouble radius;

  double lambda;
  double gamma;

  
  
  /* The line search used in calculateFinalNeighborhoods() */

  LineSearch & linesearch;


  /* inputProb(i,j), a double precision value between 0 and 1, represents
     the probability that point i would pick point j as its neighbor in
     the input data. Analogously, outputProb(i,j) represents the probability
     that point i would pick point j as its neighbor in the output data. */
  
  int ndata, nXdim, nYdim, nclusters, nexact;
  double **X;
  double **Y;
  int *clusterIndices;
  double **clusterMemberships;
  double **denseMemberships;
  int * nonzeroMemberscounter;
  int ** memb2clusterind;
  double *clustersizes;
  double **clusterXmeans;
  double ***clusterXcovs;
  double **clusterYmeans;
  double ***clusterYcovs;
  int **exact;
  double ***invCovX;  // Inverse covariance in each cluster
  double *sigmaSqrdArray;
  int **exactqueries;
  int **exactqueries_neighbororder;
  int *nexactqueries;
  double *Kx_values, *Ky_values;
  double *Psimple_values, *Pcross_values;
  double **exactdists, **exactXdists;
  double stored_cost4, stored_cost6;
  int iteration;
   
  double ***invCovType1; // Inverse covariances (sigma_all^2 I + Sigma_p)^{-1} for each cluster
  double ***invCovType2; // Inverse covariances (Sigma_p^{-1} + (sigma_all^2 I)^{-1})^{-1} for each cluster
  double *detType1; // Determinants of (sigma_all^2 I + Sigma_p) for each cluster
  
  void *invcovTempPointer;
  double **tempclustersizes;
  int desiredEffectiveNeighbors;
  
  /* ...except if radius is smaller than finalNeighborhoods[i], in which case
     the point i will use finalNeighborhoods[i] instead of radius.
     finalNeighborhoods[i] will contain a value such that when it is used
     as the gaussian for the neighborhood probability distribution of point i,
     the entropy of the distribution will be log(effectiveNeighborhoodSize),
     where effectiveNeighborhoodSize is the neighborhoodSize given to
     NeRVCostFunction's constructor. */

  vector < double >finalNeighborhoods;

  /* For convenience, the squares of the current gaussians are always stored
     in this vector, ie., sigma[i] equals
     max(radius.value(), finalNeighborhoods[i]). */

  vector < double >sigmaSqrd;

  std::ostream & feedback;

  /* Initializes finalNeighborhoods. */

  void calculateFinalNeighborhoods (int effectiveNeighborhoodSize);


public:
  geaNeRVCostFunction (
                    DataMatrix & projData, LineSearch & lineSearch,
                    DynamicDouble radius, double lambda,
                    int neighborhoodSize, std::ostream & feedback);

  /* See CostFunction for the rest. */

  double getGradient (const DataMatrix & projData, DataMatrix & target);

  double evaluate (const DataMatrix & data);

  void reportParameters(std::string& target);
  void updateDynamicParameters (size_t currentRound, size_t totalRounds,
                                const DataMatrix & projData);

  void updateDataRepresentation (const DataMatrix & newData);

  void updateLowDimensionalClusters();
  
  void provideClusterStatistics
  (
    int provided_ndata, int provided_nXdim, int provided_nYdim, int provided_nclusters,
    double **provided_X,
    int *provided_clusterIndices,   
    double **denseMemberships,
    int *nonzeroMemberscounter,
    int **memb2clusterind,   
    double **provided_clusterXmeans,
    double ***provided_clusterXcovs,
    int provided_nexact,
    int **provided_exactIndices
  );
};


#endif
