/*Copyright (C) Konstantinos Georgatzis
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#define PI 3.1415926535897932384626433832795
#define cluster_tol (1e-3)

double dRs, dRc, dKx, dKy, grad;

/* Compute 1st grad term */

double compute_gradterm1(
  double **Y, double *sigmaSqrd, double **clusterYmeans, double ***clusterYcovs, int **exact, double **exactdists, double *Ky_values, double *Kx_values, double *Psimple_values, double *Pcross_values, double **gterm1, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim, double lambda
)
{
  double Ky1 = 0, det = 0;
  double grad1[2], grad1_1[2], grad1_2[2], mu_z[2];
  int i, k, m, n, exactj;
  double tempquadratic, tempquadratic2, det_S, det_Sz, z_p;  
  double invCov[2][2], Sigma_z[2][2];

  for (i = ndata-1; i >= 0; i--)
  {
   
   Ky1 = 0; grad1_1[0] = 0; grad1_1[1] = 0; 
   for (k = nexact-1; k >= 0; k--)
   {
     exactj = exact[i][k];          
     Ky1 += exp(-exactdists[i][k]/(2*sigmaSqrd[i]));
     grad1_1[0] += exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * ((Y[i][0] - Y[exactj][0]) / (Ky_values[i]));
     grad1_1[1] += exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * ((Y[i][1] - Y[exactj][1]) / (Ky_values[i]));
   
   }
     
   grad1_2[0] = 0; grad1_2[1] = 0;
   for (k = nclusters-1; k >= 0; k--)
   { 
     if(tempclustersizes[i][k] > cluster_tol) 
    {
    det_S = clusterYcovs[k][0][0] * clusterYcovs[k][1][1] - clusterYcovs[k][0][1] * clusterYcovs[k][1][0];
    det_Sz = ((clusterYcovs[k][1][1] / det_S) + (1 / sigmaSqrd[i])) * ((clusterYcovs[k][0][0] / det_S) + (1 / sigmaSqrd[i])) - ((clusterYcovs[k][1][0] / det_S) * (clusterYcovs[k][0][1] / det_S));
    Sigma_z[0][0] = (clusterYcovs[k][0][0] / det_S + 1 / sigmaSqrd[i]) / (det_Sz); 
    Sigma_z[0][1] = (clusterYcovs[k][0][1] / det_S) / (det_Sz);
    Sigma_z[1][0] = (clusterYcovs[k][1][0] / det_S) / (det_Sz);
    Sigma_z[1][1] = (clusterYcovs[k][1][1] / det_S + 1/sigmaSqrd[i]) / (det_Sz);
    
    mu_z[0] = Sigma_z[0][0]*(Y[i][0] / sigmaSqrd[i] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
            + Sigma_z[0][1]*(Y[i][1] / sigmaSqrd[i] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
    mu_z[1] = Sigma_z[1][0]*(Y[i][0] / sigmaSqrd[i] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
            + Sigma_z[1][1]*(Y[i][1] / sigmaSqrd[i] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
	    
    invCov[0][0] = 1 / sigmaSqrd[i];
    invCov[0][1] = 0;
    invCov[1][0] = 0;
    invCov[1][1] = 1 / sigmaSqrd[i];
    
    tempquadratic = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic = tempquadratic + (Y[i][m]-mu_z[m])*invCov[m][n]*(Y[i][n]-mu_z[n]);
      }       
    } 
    det = (clusterYcovs[k][0][0] + sigmaSqrd[i]) * (clusterYcovs[k][1][1] + sigmaSqrd[i]) - (clusterYcovs[k][0][1]) * (clusterYcovs[k][1][0]);
    invCov[0][0] = (clusterYcovs[k][1][1] + sigmaSqrd[i]) / det;
    invCov[0][1] = -clusterYcovs[k][0][1] / det;
    invCov[1][0] = -clusterYcovs[k][1][0] / det;
    invCov[1][1] = (clusterYcovs[k][0][0] + sigmaSqrd[i]) / det;

    tempquadratic2 = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic2 = tempquadratic2 + (Y[i][m]-clusterYmeans[k][m])*invCov[m][n]*(Y[i][n]-clusterYmeans[k][n]);
      }       
    } 

    z_p = ( 1 / sqrt ( pow(2 * PI, nYdim) *( (clusterYcovs[k][0][0] + sigmaSqrd[i]) * (clusterYcovs[k][1][1] + sigmaSqrd[i]) - clusterYcovs[k][0][1]*clusterYcovs[k][1][0] ) ) ) * exp( (-tempquadratic2) / 2);
    grad1_2[0] += sqrt ( pow(2 * PI, nYdim) * sigmaSqrd[i] * sigmaSqrd[i] ) * (tempclustersizes[i][k] * z_p / Ky_values[i]) * (Y[i][0] - mu_z[0]) ;
    grad1_2[1] += sqrt ( pow(2 * PI, nYdim) * sigmaSqrd[i] * sigmaSqrd[i] ) * (tempclustersizes[i][k] * z_p / Ky_values[i]) * (Y[i][1] - mu_z[1]) ;
    }
   }
   grad1[0] = (1 / (2*sigmaSqrd[i])) * ( (1 - 2 * lambda)   +  (1 - lambda) * (Psimple_values[i]-log(Ky_values[i]))  + (lambda - 1) * (Pcross_values[i]-log(Kx_values[i])) ) * (grad1_1[0] + grad1_2[0]);
   grad1[1] = (1 / (2*sigmaSqrd[i])) * ( (1 - 2 * lambda)   +  (1 - lambda) * (Psimple_values[i]-log(Ky_values[i]))  + (lambda - 1) * (Pcross_values[i]-log(Kx_values[i])) ) * (grad1_1[1] + grad1_2[1]);

   
   gterm1[i][0] = grad1[0];
   gterm1[i][1] = grad1[1];
  }

  return 0; // return the Ky'/Ky term
}

double compute_gradterm2(
  double **Y, double **X, double *sigmaSqrd, int *clusterIndices, double **denseMemberships, int *nonzeroMemberscounter, int **memb2clusterind, double **clusterYmeans, double ***clusterYcovs, double **clusterXmeans, double ***clusterXcovs, int **exact, double **exactdists, double **exactXdists, int *nexactqueries, int **exactqueries, int **exactqueries_neighbororder, double *Ky_values, double *Kx_values, double *Psimple_values, double *Pcross_values, double **gterm2, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim, int nXdim, double lambda
)

{
  double det = 0;
  double grad2_2[2], mu_z[2];
  int i, j, k, l, m, n, exacti, exacti_to_row;
  double tempquadratic2, det_S, det_Sz, z_p;  
  double invCov[2][2], Sigma_z[2][2];
  
  static double *W_values = NULL;
  static double **meanWdivK = NULL;  
  static double **meanSigmaSqrd = NULL;  
  
  double grad2ex[2];
  
  if (W_values == NULL)
  {
    W_values = (double *)malloc(ndata*sizeof(double));  
    meanWdivK = (double **)malloc(ndata*sizeof(double *));
    meanSigmaSqrd = (double **)malloc(ndata*sizeof(double *));
    for (i = ndata-1; i >= 0; i--)
    {
      meanWdivK[i] = (double *)malloc(nclusters*sizeof(double));
      meanSigmaSqrd[i] = (double *)malloc(nclusters*sizeof(double));
    }  
  }

  for (i = ndata-1; i >= 0; i--)
  {
    W_values[i] = (1 - lambda) / (2 * sigmaSqrd[i]) * (Psimple_values[i] - log(Ky_values[i]) - Pcross_values[i] + log(Kx_values[i])) + (1 - 2 * lambda) / (2 * sigmaSqrd[i]);
  }

  // Compute mean of sigmaSqrd and W/Ky for each data point over its non-nexact neighbors in each cluster
  if (nclusters > 0)
  {
    for (i = ndata-1; i >= 0; i--)
    {       
      for (j = nclusters-1; j >= 0; j--)
      {
        meanWdivK[i][j] = 0;
        meanSigmaSqrd[i][j] = 0;
      }
    } 
    // First compute a sum over all data into the first entries of meanWdivK and meanSigmaSqrd, and copy it for all other data points i
    for (i = ndata-1; i >= 0; i--)
    {
      for (j = nonzeroMemberscounter[i]-1; j >= 0; j--)
      {
	k = memb2clusterind[i][j];
        meanWdivK[0][k] += W_values[i]/Ky_values[i]*denseMemberships[i][j];
        meanSigmaSqrd[0][k] += sigmaSqrd[i]*denseMemberships[i][j];
      }
    }
    for (i = ndata-1; i >= 1; i--)
    {
      for (j = nclusters-1; j >= 0; j--)
      {
        meanWdivK[i][j] = meanWdivK[0][j];
        meanSigmaSqrd[i][j] = meanSigmaSqrd[0][j];
      }
    }
    // Adjust values for each data point to remove influence of the point itself and its exact neighbors
    for (i = ndata-1; i >= 0; i--)
    {    
      for (j = nonzeroMemberscounter[i]-1; j >= 0; j--)
      {
	k = memb2clusterind[i][j];
        // Remove influence of data point itself
        meanWdivK[i][k] -= W_values[i]/Ky_values[i]*denseMemberships[i][j];
        meanSigmaSqrd[i][k] -= sigmaSqrd[i]*denseMemberships[i][j];
      }
    
      // Remove influences of exact neighbors
      for (j = nexact-1; j >= 0; j--)
      {
	k = exact[i][j];
        for (l = nonzeroMemberscounter[k]-1; l >= 0; l--)
        {	
	  m = memb2clusterind[k][l];
          meanWdivK[i][m] -= W_values[k]/Ky_values[k]*denseMemberships[k][l];
	  //meanSigmaSqrd[i][clusterIndices[i]] += sigmaSqrd[i];
          meanSigmaSqrd[i][m] -= sigmaSqrd[k]*denseMemberships[k][l];
        }
      }

      // Normalize
      for (j = nclusters-1; j >= 0; j--)
      {
	if (tempclustersizes[i][j] > cluster_tol)
	{  
          meanWdivK[i][j] /= tempclustersizes[i][j];
          meanSigmaSqrd[i][j] /= tempclustersizes[i][j];
	}
      }      
    }      
  } 
  
  
  for (i = ndata-1; i >= 0; i--)
  { 
   grad2ex[0] = 0; grad2ex[1] = 0;
   for (k = nexactqueries[i]-1; k >= 0; k--)
   { 
     exacti = exactqueries[i][k];
     exacti_to_row = exactqueries_neighbororder[i][k];
     
     
     grad2ex[0] += W_values[exacti] * exp(-exactdists[exacti][exacti_to_row] / (2 * sigmaSqrd[exacti])) / Ky_values[exacti] * (Y[i][0] - Y[exacti][0]);
    
     grad2ex[1] += W_values[exacti] * exp(-exactdists[exacti][exacti_to_row] / (2 * sigmaSqrd[exacti])) / Ky_values[exacti] * (Y[i][1] - Y[exacti][1]);
  }
  
   grad2_2[0] = 0; grad2_2[1] = 0;     
   for (k = nclusters-1; k >= 0; k--)
   {
    if(tempclustersizes[i][k] > cluster_tol) 
    {
    det_S = clusterYcovs[k][0][0] * clusterYcovs[k][1][1] - clusterYcovs[k][0][1] * clusterYcovs[k][1][0];
    det_Sz = ((clusterYcovs[k][1][1] / det_S) + (1 / meanSigmaSqrd[i][k])) * ((clusterYcovs[k][0][0] / det_S) + (1 / meanSigmaSqrd[i][k])) - ((clusterYcovs[k][1][0] / det_S) * (clusterYcovs[k][0][1] / det_S));
    Sigma_z[0][0] = (clusterYcovs[k][0][0] / det_S + 1 / meanSigmaSqrd[i][k]) / (det_Sz); 
    Sigma_z[0][1] = (clusterYcovs[k][0][1] / det_S) / (det_Sz);
    Sigma_z[1][0] = (clusterYcovs[k][1][0] / det_S) / (det_Sz);
    Sigma_z[1][1] = (clusterYcovs[k][1][1] / det_S + 1 / meanSigmaSqrd[i][k]) / (det_Sz);
    
    mu_z[0] = Sigma_z[0][0]*(Y[i][0] / meanSigmaSqrd[i][k] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
            + Sigma_z[0][1]*(Y[i][1] / meanSigmaSqrd[i][k] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
    mu_z[1] = Sigma_z[1][0]*(Y[i][0] / meanSigmaSqrd[i][k] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
            + Sigma_z[1][1]*(Y[i][1] / meanSigmaSqrd[i][k] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
    
    det = (clusterYcovs[k][0][0] + meanSigmaSqrd[i][k]) * (clusterYcovs[k][1][1] + meanSigmaSqrd[i][k]) - (clusterYcovs[k][0][1]) * (clusterYcovs[k][1][0]);
    invCov[0][0] = (clusterYcovs[k][1][1] + meanSigmaSqrd[i][k]) / det;
    invCov[0][1] = -clusterYcovs[k][0][1] / det;
    invCov[1][0] = -clusterYcovs[k][1][0] / det;
    invCov[1][1] = (clusterYcovs[k][0][0] + meanSigmaSqrd[i][k]) / det;

    tempquadratic2 = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic2 = tempquadratic2 + (Y[i][m]-clusterYmeans[k][m])*invCov[m][n]*(Y[i][n]-clusterYmeans[k][n]);
      }       
    } 
    
     z_p = ( 1 / sqrt ( pow(2 * PI, nYdim) * det ) ) * exp( (-tempquadratic2) / 2 );

     grad2_2[0] +=  meanWdivK[i][k] * sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * (tempclustersizes[i][k] * z_p) * (Y[i][0] - mu_z[0]) ;
     grad2_2[1] +=  meanWdivK[i][k] * sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * (tempclustersizes[i][k] * z_p) * (Y[i][1] - mu_z[1]) ;
     
     } 
   }
   
   gterm2[i][0] = grad2ex[0] + grad2_2[0];
   gterm2[i][1] = grad2ex[1] + grad2_2[1];
  }
  
  return 0; // return the Ky'/Ky term
}

double compute_gradterm3(
  double **Y, double **X, double *sigmaSqrd, double **clusterYmeans, double ***clusterYcovs, double **clusterXmeans, double ***clusterXcovs, int **exact, double **exactdists, double **exactXdists, double *Ky_values, double *Kx_values, double *Psimple_values, double *Pcross_values, double **gterm3, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim, int nXdim, double lambda, double ***invCovType1, double *detType1
)
{
  double det = 0;
  double grad3[2], mu_z_p[2], grad3_1_1[2], grad3_1_2[2], grad3_2_1[2], grad3_2_2[2], grad3_3_1[2], grad3_3_2[2], grad3_4_1[2], grad3_4_2[2];
  int i, k, m, n, exactj;
  double tempquadratic, tempquadratic2, tempXquadratic, tempXquadratic2, det_S, det_Sz, z_p, Tr_Sz, Tr_sigmaMSp; // , det_X; //det; // take det_X from MATLAB  
  double invCov[2][2], Sigma_z[2][2];
  double ***invCovType1_i;
  double *detType1_i;

  for (i = ndata-1; i >= 0; i--)
  {    
   grad3_1_1[0] = 0; grad3_1_1[1] = 0; grad3_2_1[0] = 0; grad3_2_1[1] = 0; grad3_3_1[0] = 0; grad3_3_1[1] = 0; grad3_4_1[0] = 0; grad3_4_1[1] = 0;
   for (k = nexact-1; k >= 0; k--)
   {
     
     exactj = exact[i][k];
     grad3_1_1[0] +=    exp(-exactdists[i][k]/(2*sigmaSqrd[i])) *  (-exactdists[i][k]/(2*sigmaSqrd[i])) * Y[exactj][0] / (Ky_values[i])  
                    - (log(Ky_values[i]) / Ky_values[i]) * (exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][0])) ;
     grad3_1_1[1] +=    exp(-exactdists[i][k]/(2*sigmaSqrd[i])) *  (-exactdists[i][k]/(2*sigmaSqrd[i])) * Y[exactj][1] / (Ky_values[i])  
                    - (log(Ky_values[i]) / Ky_values[i]) * (exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][1])) ;

     grad3_2_1[0] += (((exp(-exactdists[i][k]/(2*sigmaSqrd[i]))) * (-exactXdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][0])) / (Ky_values[i])) 
                   - ((log(Kx_values[i]) / Ky_values[i])) * (exp(-exactdists[i][k]/(2*sigmaSqrd[i]))) * (Y[exactj][0]);
     grad3_2_1[1] += (((exp(-exactdists[i][k]/(2*sigmaSqrd[i]))) * (-exactXdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][1])) / (Ky_values[i])) 
                   - ((log(Kx_values[i]) / Ky_values[i])) * (exp(-exactdists[i][k]/(2*sigmaSqrd[i]))) * (Y[exactj][1]);
		      
     grad3_3_1[0] += (exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][0])) / (Ky_values[i]);
     grad3_3_1[1] += (exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][1])) / (Ky_values[i]);
     
     grad3_4_1[0] += (exp(-exactXdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][0])) / (Kx_values[i]);
     grad3_4_1[1] += (exp(-exactXdists[i][k]/(2*sigmaSqrd[i])) * (Y[exactj][1])) / (Kx_values[i]);     
   }

   invCovType1_i = invCovType1;
   detType1_i = detType1;


   grad3_1_2[0] = 0; grad3_1_2[1] = 0; grad3_2_2[0] = 0; grad3_2_2[1] = 0; grad3_3_2[0] = 0; grad3_3_2[1] = 0; grad3_4_2[0] = 0; grad3_4_2[1] = 0; //grad3_2_5[0] = 0; grad3_2_5[1] = 0;
   for (k = nclusters-1; k >= 0; k--)
   { 
     
    if(tempclustersizes[i][k] > cluster_tol)
    {
    det_S = clusterYcovs[k][0][0] * clusterYcovs[k][1][1] - clusterYcovs[k][0][1] * clusterYcovs[k][1][0];
    det_Sz = ((clusterYcovs[k][1][1] / det_S) + (1 / sigmaSqrd[i])) * ((clusterYcovs[k][0][0] / det_S) + (1 / sigmaSqrd[i])) - ((clusterYcovs[k][1][0] / det_S) * (clusterYcovs[k][0][1] / det_S));
    Sigma_z[0][0] = (clusterYcovs[k][0][0] / det_S + 1 / sigmaSqrd[i]) / (det_Sz); 
    Sigma_z[0][1] = (clusterYcovs[k][0][1] / det_S) / (det_Sz);
    Sigma_z[1][0] = (clusterYcovs[k][1][0] / det_S) / (det_Sz);
    Sigma_z[1][1] = (clusterYcovs[k][1][1] / det_S + 1/sigmaSqrd[i]) / (det_Sz);
    Tr_Sz = Sigma_z[0][0] + Sigma_z[1][1];
    
    mu_z_p[0] = Sigma_z[0][0]*(Y[i][0] / sigmaSqrd[i] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
              + Sigma_z[0][1]*(Y[i][1] / sigmaSqrd[i] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
    mu_z_p[1] = Sigma_z[1][0]*(Y[i][0] / sigmaSqrd[i] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
              + Sigma_z[1][1]*(Y[i][1] / sigmaSqrd[i] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
	    
     invCov[0][0] = 1 / sigmaSqrd[i];
     invCov[0][1] = 0;
     invCov[1][0] = 0;
     invCov[1][1] = 1 / sigmaSqrd[i];
    
    tempquadratic = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic = tempquadratic + (Y[i][m]-mu_z_p[m]) * invCov[m][n] * (Y[i][n]-mu_z_p[n]);
      }       
    } 

    det = (clusterYcovs[k][0][0] + sigmaSqrd[i]) * (clusterYcovs[k][1][1] + sigmaSqrd[i]) - (clusterYcovs[k][0][1]) * (clusterYcovs[k][1][0]);
    invCov[0][0] = (clusterYcovs[k][1][1] + sigmaSqrd[i]) / det;
    invCov[0][1] = -clusterYcovs[k][0][1] / det;
    invCov[1][0] = -clusterYcovs[k][1][0] / det;
    invCov[1][1] = (clusterYcovs[k][0][0] + sigmaSqrd[i]) / det;

    tempquadratic2 = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic2 = tempquadratic2 + (Y[i][m]-clusterYmeans[k][m])*invCov[m][n]*(Y[i][n]-clusterYmeans[k][n]);
      }       
    } 

    tempXquadratic = 0;
    for (m = nXdim-1; m >= 0; m--)
    {  
      tempXquadratic = tempXquadratic + (X[i][m]-clusterXmeans[k][m])*(1/sigmaSqrd[i])*(X[i][m]-clusterXmeans[k][m]);
    } 
     
     tempXquadratic2 = 0;
     for (m = nXdim-1; m >= 0; m--)
     {  
       for (n = nXdim-1; n >= 0; n--)
       {	 
	 tempXquadratic2 += (X[i][m]-clusterXmeans[k][m])*invCovType1_i[k][m][n]*(X[i][n]-clusterXmeans[k][n]);
       }       
     }
     
    det = sigmaSqrd[i] * sigmaSqrd[i];
    
     
    Tr_sigmaMSp = 0;
    for (m = nXdim-1; m >= 0; m--) 
      {
	Tr_sigmaMSp += (clusterXcovs[k][m][m] / sigmaSqrd[i]);
      }

    z_p = ( 1 / sqrt ( pow(2 * PI, nYdim) *( (clusterYcovs[k][0][0] + sigmaSqrd[i]) * (clusterYcovs[k][1][1] + sigmaSqrd[i]) - clusterYcovs[k][0][1]*clusterYcovs[k][1][0] ) ) ) * exp( (-tempquadratic2) / 2);

    grad3_1_2[0] += 
                    (1 / Ky_values[i]) * tempclustersizes[i][k]  * ( sqrt(pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i])) * z_p )  / (2 * sigmaSqrd[i]) 
                  * (-1)*(  Tr_Sz * (mu_z_p[0]) 
		     + 2 * (Sigma_z[0][0] * (mu_z_p[0]) + Sigma_z[0][1] * (mu_z_p[1])) 
		     + ( (mu_z_p[0]- Y[i][0]) * (mu_z_p[0] - Y[i][0]) + (mu_z_p[1] - Y[i][1]) * (mu_z_p[1] - Y[i][1])) * mu_z_p[0]  ) 
                  - (log(Ky_values[i]) / Ky_values[i]) * (sqrt ( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i]) ) * (tempclustersizes[i][k] * z_p * mu_z_p[0])) ;     
    grad3_1_2[1] += 
                    (1 / Ky_values[i]) * tempclustersizes[i][k]  * (sqrt ( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i])) * z_p)  / (2 * sigmaSqrd[i]) 
                  * (-1)*(  Tr_Sz * (mu_z_p[1]) 
		     + 2 * (Sigma_z[1][0] * (mu_z_p[0]) + Sigma_z[1][1] * (mu_z_p[1])) 
		     + ( (mu_z_p[0]- Y[i][0]) * (mu_z_p[0] - Y[i][0]) + (mu_z_p[1] - Y[i][1]) * (mu_z_p[1] - Y[i][1])) * mu_z_p[1]   )  
                  - (log(Ky_values[i]) / Ky_values[i]) * (sqrt ( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i]) ) * (tempclustersizes[i][k] * z_p * mu_z_p[1])) ;
		  
		  
    grad3_2_2[0] +=  ((1 / Ky_values[i]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i]) ) * z_p) * (-0.5) * (tempXquadratic + Tr_sigmaMSp) * mu_z_p[0]) 
    - (log(Kx_values[i]) / Ky_values[i]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * det  ) * z_p * mu_z_p[0]);
    grad3_2_2[1] +=  ((1 / Ky_values[i]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i]) ) * z_p) * (-0.5) * (tempXquadratic + Tr_sigmaMSp) * mu_z_p[1]) 
    - (log(Kx_values[i]) / Ky_values[i]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * det ) * z_p * mu_z_p[1]);
   
    grad3_3_2[0] += (1 / Ky_values[i]) * tempclustersizes[i][k] * sqrt( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i]) ) * z_p * mu_z_p[0];
    grad3_3_2[1] += (1 / Ky_values[i]) * tempclustersizes[i][k] * sqrt( pow(2 * PI, nYdim) * (sigmaSqrd[i]) * (sigmaSqrd[i]) ) * z_p * mu_z_p[1];
    
    grad3_4_2[0] += (1 / Kx_values[i]) * tempclustersizes[i][k] * (1 / sqrt(pow(sigmaSqrd[i],-nXdim)*detType1_i[k]) ) * exp((-tempXquadratic2) / 2) * clusterYmeans[k][0];
    grad3_4_2[1] += (1 / Kx_values[i]) * tempclustersizes[i][k] * (1 / sqrt(pow(sigmaSqrd[i],-nXdim)*detType1_i[k]) ) * exp((-tempXquadratic2) / 2) * clusterYmeans[k][1];
    }
   }
   
   grad3[0] = (((lambda -1) * (Y[i][0])) / (2*sigmaSqrd[i]) * (Psimple_values[i] - log(Ky_values[i]))) 
            + ((1 - lambda) / (2*sigmaSqrd[i]) * (grad3_1_1[0] + grad3_1_2[0])) 
	    + (((1 - lambda) * (Y[i][0])) / (2*sigmaSqrd[i]) * (Pcross_values[i] - log(Kx_values[i]))) 
	    - ((1 - lambda) / (2*sigmaSqrd[i]) * (grad3_2_1[0] + grad3_2_2[0])) 	    
	    - ((1 - lambda) * Y[i][0] / (2*sigmaSqrd[i]))
	    + ((1 - lambda) / (2*sigmaSqrd[i]) * (grad3_3_1[0] + grad3_3_2[0]))
	    + ((lambda / (2*sigmaSqrd[i])) * Y[i][0])
	    - ((lambda / (2*sigmaSqrd[i])) * (grad3_4_1[0] + grad3_4_2[0]));
   grad3[1] = (((lambda -1) * (Y[i][1])) / (2*sigmaSqrd[i]) * (Psimple_values[i] - log(Ky_values[i]))) 
            + ((1 - lambda) / (2*sigmaSqrd[i]) * (grad3_1_1[1] + grad3_1_2[1]))
	    + (((1 - lambda) * (Y[i][1])) / (2*sigmaSqrd[i]) * (Pcross_values[i] - log(Kx_values[i]))) 
	    - ((1 - lambda) / (2*sigmaSqrd[i]) * (grad3_2_1[1] + grad3_2_2[1])) 	    
	    - ((1 - lambda) * Y[i][1] / (2*sigmaSqrd[i]))
	    + ((1 - lambda) / (2*sigmaSqrd[i]) * (grad3_3_1[1] + grad3_3_2[1]))
	    + ((lambda / (2*sigmaSqrd[i])) * Y[i][1])
	    - ((lambda / (2*sigmaSqrd[i])) * (grad3_4_1[1] + grad3_4_2[1]));
   
   gterm3[i][0] = grad3[0];
   gterm3[i][1] = grad3[1];
  }
  return 0; // return the Ky'/Ky term
}

double compute_gradterm4(
  double **Y, double **X, double *sigmaSqrd, int *clusterIndices, double **denseMemberships, int *nonzeroMemberscounter, int **memb2clusterind, double **clusterYmeans, double ***clusterYcovs, double **clusterXmeans, double ***clusterXcovs, int **exact, double **exactdists, double **exactXdists,  int *nexactqueries, int **exactqueries, int **exactqueries_neighbororder, double *Ky_values, double *Kx_values, double *Psimple_values, double *Pcross_values, double **gterm4, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim, int nXdim, double lambda, void **invcovTempPointer, double ***invCovType1, double *detType1
)

{
  double det = 0;
  double grad4[2], mu_z_p[2], grad4_1_1a[2], grad4_1_1b[2], grad4_1_2a[2], grad4_1_2b[2], grad4_2_1a[2], grad4_2_1b[2], grad4_2_2a[2], grad4_2_2b[2], grad4_3_1a[2], grad4_3_1b[2], grad4_3_2a[2], grad4_3_2b[2], grad4_4_1a[2], grad4_4_1b[2], grad4_4_2a[2], grad4_4_2b[2];
  int i, j, k, l, m, n, exacti, exacti_to_row;
  double tempquadratic, tempquadratic2, tempXquadratic, tempXquadratic2, det_S, det_Sz, det_prop, z_p, Tr_Sz, Tr_sigmaMSp;
  double invCov[2][2], Sigma_z[2][2];
  static double **meanKy = NULL, **meanKx = NULL;  
  static double **meanSigmaSqrd = NULL;    
  
  nm::Matrix *invcovTempLocal;
  double ***invCovType1mean_i;
  double *detType1mean_i;
  
  if ((*invcovTempPointer) == NULL)
  {
    printf("Allocating new matrix in compute_gradterm4\n");    
    invcovTempLocal = new nm::Matrix(nXdim,nXdim);
    *invcovTempPointer = invcovTempLocal;
  }
  invcovTempLocal = (nm::Matrix *)(*invcovTempPointer);

  if (meanKy == NULL)
  {
    meanKy = (double **)malloc(ndata*sizeof(double *));
    meanKx = (double **)malloc(ndata*sizeof(double *));
    meanSigmaSqrd = (double **)malloc(ndata*sizeof(double *));
    for (i = ndata-1; i >= 0; i--)
    {
      meanKy[i] = (double *)malloc(nclusters*sizeof(double));
      meanKx[i] = (double *)malloc(nclusters*sizeof(double));
      meanSigmaSqrd[i] = (double *)malloc(nclusters*sizeof(double));
    }
  }
  
  // Compute mean of sigmaSqrd, Ky, and Kx for each data point over its non-nexact neighbors in each cluster
  if (nclusters > 0)
  {
    for (i = ndata-1; i >= 0; i--)
    {       
      for (j = nclusters-1; j >= 0; j--)
      {
        meanSigmaSqrd[i][j] = 0;
        meanKy[i][j] = 0;
        meanKx[i][j] = 0;
      }
    } 
    // First compute a sum over all data into the first entries of meanKy, meanKx, and meanSigmaSqrd, and copy it for all other data points i
    for (i = ndata-1; i >= 0; i--)
    {
      for (j = nonzeroMemberscounter[i]-1; j >= 0; j--)
      {      
	k = memb2clusterind[i][j];
        meanKy[0][k] += Ky_values[i]*denseMemberships[i][j];	
        meanKx[0][k] += Kx_values[i]*denseMemberships[i][j];
        meanSigmaSqrd[0][k] += sigmaSqrd[i]*denseMemberships[i][j];      
      }
    }

    for (i = ndata-1; i >= 1; i--)
    {
      for (j = nclusters-1; j >= 0; j--)
      {
        meanKy[i][j] = meanKy[0][j];
        meanKx[i][j] = meanKx[0][j];
        meanSigmaSqrd[i][j] = meanSigmaSqrd[0][j];
      }
    }
    
    // Adjust values for each data point to remove influence of the point itself and its exact neighbors
    for (i = ndata-1; i >= 0; i--)
    {
      // Remove influence of data point itself
    
      for (j = nonzeroMemberscounter[i]-1; j >= 0; j--)
      { 
	k = memb2clusterind[i][j];
        meanKy[i][k] -= Ky_values[i]*denseMemberships[i][j];
        meanKx[i][k] -= Kx_values[i]*denseMemberships[i][j];
        meanSigmaSqrd[i][k] -= sigmaSqrd[i]*denseMemberships[i][j];
      }
      // Remove influences of exact neighbors    
      for (j = nexact-1; j >= 0; j--)
      {
	k = exact[i][j];
        for (l = nonzeroMemberscounter[k]-1; l >= 0; l--)
        { 
	  m = memb2clusterind[k][l];
          meanKy[i][m] -= Ky_values[k]*denseMemberships[k][l];
          meanKx[i][m] -= Kx_values[k]*denseMemberships[k][l];
          meanSigmaSqrd[i][m] -= sigmaSqrd[k]*denseMemberships[k][l];
	}
      }
      for (j = nclusters-1; j >= 0; j--)
      {
	if (tempclustersizes[i][j] > cluster_tol)
	{  
          meanKy[i][j] /= tempclustersizes[i][j];
          meanKx[i][j] /= tempclustersizes[i][j];
          meanSigmaSqrd[i][j] /= tempclustersizes[i][j];
	}
      }      
    }      
  }
  
  
  
  
  
  
  for (i = ndata-1; i >= 0; i--)
  {  
    
   grad4_1_1b[0] = 0; grad4_1_1b[1] = 0; grad4_2_1b[0] = 0; grad4_2_1b[1] = 0; grad4_3_1b[0] = 0; grad4_3_1b[1] = 0; grad4_4_1b[0] = 0; grad4_4_1b[1] = 0; grad4_1_1a[0] = 0; grad4_1_1a[1] = 0; grad4_2_1a[0] = 0; grad4_2_1a[1] = 0; grad4_3_1a[0] = 0; grad4_3_1a[1] = 0; grad4_4_1a[0] = 0; grad4_4_1a[1] = 0;
   for (k = nexactqueries[i]-1; k >= 0; k--)
   {
     exacti = exactqueries[i][k];
     exacti_to_row = exactqueries_neighbororder[i][k];
     
     grad4_1_1a[0] +=  (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * Y[i][0] / (Ky_values[exacti])  
                    - (log(Ky_values[exacti]) / Ky_values[exacti]) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][0]))) / (2 * sigmaSqrd[exacti]) ;
     grad4_1_1a[1] +=  (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * Y[i][1] / (Ky_values[exacti])  
                    - (log(Ky_values[exacti]) / Ky_values[exacti]) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][1]))) / (2 * sigmaSqrd[exacti]) ;
     
     grad4_1_1b[0] +=    (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) *  (-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * Y[exacti][0] / (Ky_values[exacti])  
                    - (log(Ky_values[exacti]) / Ky_values[exacti]) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][0]))) / (2 * sigmaSqrd[exacti]) ;
     grad4_1_1b[1] +=    (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) *  (-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * Y[exacti][1] / (Ky_values[exacti])  
                    - (log(Ky_values[exacti]) / Ky_values[exacti]) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][1]))) / (2 * sigmaSqrd[exacti]) ;
		    
     grad4_2_1a[0] += ((((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][0])) / (Ky_values[exacti])) 
                   - ((log(Kx_values[exacti]) / Ky_values[exacti])) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (Y[i][0])) / (2 * sigmaSqrd[exacti]);
     grad4_2_1a[1] += ((((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][1])) / (Ky_values[exacti])) 
                   - ((log(Kx_values[exacti]) / Ky_values[exacti])) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (Y[i][1])) / (2 * sigmaSqrd[exacti]);
		    
     grad4_2_1b[0] += ((((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][0])) / (Ky_values[exacti])) 
                   - ((log(Kx_values[exacti]) / Ky_values[exacti])) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (Y[exacti][0])) / (2 * sigmaSqrd[exacti]);
     grad4_2_1b[1] += ((((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][1])) / (Ky_values[exacti])) 
                   - ((log(Kx_values[exacti]) / Ky_values[exacti])) * (exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti]))) * (Y[exacti][1])) / (2 * sigmaSqrd[exacti]);
     
     grad4_3_1a[0] += ((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][0])) / (Ky_values[exacti])) / (2 * sigmaSqrd[exacti]);
     grad4_3_1a[1] += ((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][1])) / (Ky_values[exacti])) / (2 * sigmaSqrd[exacti]);
		   
     grad4_3_1b[0] += ((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][0])) / (Ky_values[exacti])) / (2 * sigmaSqrd[exacti]);
     grad4_3_1b[1] += ((exp(-exactdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][1])) / (Ky_values[exacti])) / (2 * sigmaSqrd[exacti]);
     
     grad4_4_1a[0] += ((exp(-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][0])) / (Kx_values[exacti])) / (2 * sigmaSqrd[exacti]);
     grad4_4_1a[1] += ((exp(-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[i][1])) / (Kx_values[exacti])) / (2 * sigmaSqrd[exacti]); 
     
     grad4_4_1b[0] += ((exp(-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][0])) / (Kx_values[exacti])) / (2 * sigmaSqrd[exacti]);
     grad4_4_1b[1] += ((exp(-exactXdists[exacti][exacti_to_row]/(2*sigmaSqrd[exacti])) * (Y[exacti][1])) / (Kx_values[exacti]))  / (2 * sigmaSqrd[exacti]) ;
     
   }
 
    invCovType1mean_i = invCovType1;
    detType1mean_i = detType1;  
    
   grad4_1_2b[0] = 0; grad4_1_2b[1] = 0; grad4_2_2b[0] = 0; grad4_2_2b[1] = 0; grad4_3_2b[0] = 0; grad4_3_2b[1] = 0; grad4_4_2b[0] = 0; grad4_4_2b[1] = 0; grad4_1_2a[0] = 0; grad4_1_2a[1] = 0; grad4_2_2a[0] = 0; grad4_2_2a[1] = 0; grad4_3_2a[0] = 0; grad4_3_2a[1] = 0; grad4_4_2a[0] = 0; grad4_4_2a[1] = 0;
   for (k = nclusters-1; k >= 0; k--)
   { 
     
    if(tempclustersizes[i][k] > cluster_tol) 
    {
    det_S = clusterYcovs[k][0][0] * clusterYcovs[k][1][1] - clusterYcovs[k][0][1] * clusterYcovs[k][1][0];
    det_Sz = ((clusterYcovs[k][1][1] / det_S) + (1 / meanSigmaSqrd[i][k])) * ((clusterYcovs[k][0][0] / det_S) + (1 / meanSigmaSqrd[i][k])) - ((clusterYcovs[k][1][0] / det_S) * (clusterYcovs[k][0][1] / det_S));
    Sigma_z[0][0] = (clusterYcovs[k][0][0] / det_S + 1 / meanSigmaSqrd[i][k]) / (det_Sz); 
    Sigma_z[0][1] = (clusterYcovs[k][0][1] / det_S) / (det_Sz);
    Sigma_z[1][0] = (clusterYcovs[k][1][0] / det_S) / (det_Sz);
    Sigma_z[1][1] = (clusterYcovs[k][1][1] / det_S + 1/meanSigmaSqrd[i][k]) / (det_Sz);
    Tr_Sz = Sigma_z[0][0] + Sigma_z[1][1];
    
    mu_z_p[0] = Sigma_z[0][0]*(Y[i][0] / meanSigmaSqrd[i][k] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
              + Sigma_z[0][1]*(Y[i][1] / meanSigmaSqrd[i][k] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
    mu_z_p[1] = Sigma_z[1][0]*(Y[i][0] / meanSigmaSqrd[i][k] + (clusterYcovs[k][1][1] / det_S) * clusterYmeans[k][0] - (clusterYcovs[k][0][1] / det_S) * clusterYmeans[k][1]) 
              + Sigma_z[1][1]*(Y[i][1] / meanSigmaSqrd[i][k] + (clusterYcovs[k][0][0] / det_S) * clusterYmeans[k][1] - (clusterYcovs[k][1][0] / det_S) * clusterYmeans[k][0]);
	    
     invCov[0][0] = 1 / meanSigmaSqrd[i][k];
     invCov[0][1] = 0;
     invCov[1][0] = 0;
     invCov[1][1] = 1 / meanSigmaSqrd[i][k];
    
    tempquadratic = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic = tempquadratic + (Y[i][m]-mu_z_p[m]) * invCov[m][n] * (Y[i][n]-mu_z_p[n]); 
      }       
    }
    
    det = (clusterYcovs[k][0][0] + meanSigmaSqrd[i][k]) * (clusterYcovs[k][1][1] + meanSigmaSqrd[i][k]) - (clusterYcovs[k][0][1]) * (clusterYcovs[k][1][0]);
    invCov[0][0] = (clusterYcovs[k][1][1] + meanSigmaSqrd[i][k]) / det;
    invCov[0][1] = -clusterYcovs[k][0][1] / det;
    invCov[1][0] = -clusterYcovs[k][1][0] / det;
    invCov[1][1] = (clusterYcovs[k][0][0] + meanSigmaSqrd[i][k]) / det;

    tempquadratic2 = 0;
    for (m = nYdim-1; m >= 0; m--)
    {  
      for (n = nYdim-1; n >= 0; n--)
      {	 
        tempquadratic2 = tempquadratic2 + (Y[i][m]-clusterYmeans[k][m])*invCov[m][n]*(Y[i][n]-clusterYmeans[k][n]); 
      }       
    } 

    tempXquadratic = 0;
    for (m = nXdim-1; m >= 0; m--)
    {  
      tempXquadratic = tempXquadratic + (X[i][m]-clusterXmeans[k][m])*(1/meanSigmaSqrd[i][k])*(X[i][m]-clusterXmeans[k][m]);
    }

     tempXquadratic2 = 0;
     for (m = nXdim-1; m >= 0; m--)
     {  
       for (n = nXdim-1; n >= 0; n--)
       {	 
	 tempXquadratic2 += (X[i][m]-clusterXmeans[k][m])*invCovType1mean_i[k][m][n]*(X[i][n]-clusterXmeans[k][n]);
       }       
     }
    det = meanSigmaSqrd[i][k] * meanSigmaSqrd[i][k];
    Tr_sigmaMSp = 0;
    for (m = nXdim-1; m >= 0; m--) 
      {
	Tr_sigmaMSp += (clusterXcovs[k][m][m] / meanSigmaSqrd[i][k]);
      }
    

    det_prop = (clusterYcovs[k][0][0] / meanSigmaSqrd[i][k] + 1) * (clusterYcovs[k][1][1] / meanSigmaSqrd[i][k] + 1) - (clusterYcovs[k][0][1]/meanSigmaSqrd[i][k]) * (clusterYcovs[k][1][0]/meanSigmaSqrd[i][k]);

    z_p = ( 1 / sqrt ( pow(2 * PI, nYdim) *( (clusterYcovs[k][0][0] + meanSigmaSqrd[i][k]) * (clusterYcovs[k][1][1] + meanSigmaSqrd[i][k]) - clusterYcovs[k][0][1]*clusterYcovs[k][1][0] ) ) ) * exp( (-tempquadratic2) / 2);
    
    grad4_1_2a[0] += ((1 / meanKy[i][k]) * tempclustersizes[i][k] * ( sqrt(pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k])) * z_p ) * (-0.5) * (tempquadratic + (Tr_Sz / meanSigmaSqrd[i][k])) - (log(meanKy[i][k]) / meanKy[i][k]) * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2)) * Y[i][0] / (2 * meanSigmaSqrd[i][k]);    
    grad4_1_2a[1] += ((1 / meanKy[i][k]) * tempclustersizes[i][k] * ( sqrt(pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k])) * z_p ) * (-0.5) * (tempquadratic + (Tr_Sz / meanSigmaSqrd[i][k])) - (log(meanKy[i][k]) / meanKy[i][k]) * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2)) * Y[i][1] / (2 * meanSigmaSqrd[i][k]);

    grad4_1_2b[0] += ((1 / meanKy[i][k]) * tempclustersizes[i][k]  * ( sqrt(pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k])) * z_p )  / (2 * meanSigmaSqrd[i][k]) * (-1)*(  Tr_Sz * (mu_z_p[0]) + 2 * (Sigma_z[0][0] * (mu_z_p[0]) + Sigma_z[0][1] * (mu_z_p[1])) + ( (mu_z_p[0]- Y[i][0]) * (mu_z_p[0] - Y[i][0]) + (mu_z_p[1] - Y[i][1]) * (mu_z_p[1] - Y[i][1])) * mu_z_p[0]  ) - (log(meanKy[i][k]) / meanKy[i][k]) * (sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * (tempclustersizes[i][k] * z_p * mu_z_p[0]))) / (2 * meanSigmaSqrd[i][k]) ;  
    grad4_1_2b[1] += ((1 / meanKy[i][k]) * tempclustersizes[i][k]  * (sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k])) * z_p)  / (2 * meanSigmaSqrd[i][k]) * (-1)*(  Tr_Sz * (mu_z_p[1]) + 2 * (Sigma_z[1][0] * (mu_z_p[0]) + Sigma_z[1][1] * (mu_z_p[1])) + ( (mu_z_p[0]- Y[i][0]) * (mu_z_p[0] - Y[i][0]) + (mu_z_p[1] - Y[i][1]) * (mu_z_p[1] - Y[i][1])) * mu_z_p[1]   )  - (log(meanKy[i][k]) / meanKy[i][k]) * (sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * (tempclustersizes[i][k] * z_p * mu_z_p[1]))) / (2 * meanSigmaSqrd[i][k]) ;
		  
    
    grad4_2_2a[0] += ((1 / meanKy[i][k]) * tempclustersizes[i][k] * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2) * (-0.5) * (tempXquadratic + Tr_sigmaMSp) - (log(meanKx[i][k]) / meanKy[i][k]) * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2)) * Y[i][0] / (2 * meanSigmaSqrd[i][k]);
    grad4_2_2a[1] += ((1 / meanKy[i][k]) * tempclustersizes[i][k] * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2) * (-0.5) * (tempXquadratic + Tr_sigmaMSp) - (log(meanKx[i][k]) / meanKy[i][k]) * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2)) * Y[i][1] / (2 * meanSigmaSqrd[i][k]);
		  
    
    grad4_2_2b[0] +=  (((1 / meanKy[i][k]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * z_p) * (-0.5) * (tempXquadratic + Tr_sigmaMSp) * mu_z_p[0]) 
    - (log(meanKx[i][k]) / meanKy[i][k]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * det  ) * z_p * mu_z_p[0])) / (2 * meanSigmaSqrd[i][k]);  
    grad4_2_2b[1] +=  (((1 / meanKy[i][k]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * z_p) * (-0.5) * (tempXquadratic + Tr_sigmaMSp) * mu_z_p[1]) 
    - (log(meanKx[i][k]) / meanKy[i][k]) * tempclustersizes[i][k] * (sqrt ( pow(2 * PI, nYdim) * det ) * z_p * mu_z_p[1])) / (2 * meanSigmaSqrd[i][k]);    
    
    grad4_3_2a[0] += (1 / meanKy[i][k]) * tempclustersizes[i][k] * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2) * Y[i][0] / (2 * meanSigmaSqrd[i][k]);
    grad4_3_2a[1] += (1 / meanKy[i][k]) * tempclustersizes[i][k] * (1 / sqrt (det_prop)) * exp(-tempquadratic2 / 2) * Y[i][1] / (2 * meanSigmaSqrd[i][k]);
    
    grad4_3_2b[0] += (1 / meanKy[i][k]) * tempclustersizes[i][k] * sqrt( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * z_p * mu_z_p[0] / (2 * meanSigmaSqrd[i][k]);
    grad4_3_2b[1] += (1 / meanKy[i][k]) * tempclustersizes[i][k] * sqrt( pow(2 * PI, nYdim) * (meanSigmaSqrd[i][k]) * (meanSigmaSqrd[i][k]) ) * z_p * mu_z_p[1] / (2 * meanSigmaSqrd[i][k]);
    
    grad4_4_2a[0] += (1 / meanKx[i][k]) * tempclustersizes[i][k] * (1 / sqrt (pow(meanSigmaSqrd[i][k],-nXdim)*detType1mean_i[k])) * exp(-tempXquadratic2 / 2) * Y[i][0] / (2 * meanSigmaSqrd[i][k]);
    grad4_4_2a[1] += (1 / meanKx[i][k]) * tempclustersizes[i][k] * (1 / sqrt (pow(meanSigmaSqrd[i][k],-nXdim)*detType1mean_i[k])) * exp(-tempXquadratic2 / 2) * Y[i][1] / (2 * meanSigmaSqrd[i][k]);
    
    grad4_4_2b[0] += (1 / meanKx[i][k]) * tempclustersizes[i][k] * (1 / sqrt(pow(meanSigmaSqrd[i][k],-nXdim)*detType1mean_i[k]) ) * exp((-tempXquadratic2) / 2) * clusterYmeans[k][0] / (2 * meanSigmaSqrd[i][k]);
    grad4_4_2b[1] += (1 / meanKx[i][k]) * tempclustersizes[i][k] * (1 / sqrt(pow(meanSigmaSqrd[i][k],-nXdim)*detType1mean_i[k]) ) * exp((-tempXquadratic2) / 2) * clusterYmeans[k][1] / (2 * meanSigmaSqrd[i][k]);
    }
   }
   
   grad4[0] = ((lambda - 1) * (grad4_1_1a[0] + grad4_1_2a[0])) 
            + ((1 - lambda) * (grad4_1_1b[0] + grad4_1_2b[0]))
	    + ((1 - lambda) * (grad4_2_1a[0] + grad4_2_2a[0]))
	    - ((1 - lambda) * (grad4_2_1b[0] + grad4_2_2b[0])) 	    
	    - ((1 - lambda) * (grad4_3_1a[0] + grad4_3_2a[0]))
	    + ((1 - lambda) * (grad4_3_1b[0] + grad4_3_2b[0]))
	    + ((lambda)     * (grad4_4_1a[0] + grad4_4_2a[0]))
	    - ((lambda)     * (grad4_4_1b[0] + grad4_4_2b[0])); 
	    
   grad4[1] = ((lambda - 1) * (grad4_1_1a[1] + grad4_1_2a[1])) 
            + ((1 - lambda) * (grad4_1_1b[1] + grad4_1_2b[1]))
	    + ((1 - lambda) * (grad4_2_1a[1] + grad4_2_2a[1]))
	    - ((1 - lambda) * (grad4_2_1b[1] + grad4_2_2b[1])) 	    
	    - ((1 - lambda) * (grad4_3_1a[1] + grad4_3_2a[1]))
	    + ((1 - lambda) * (grad4_3_1b[1] + grad4_3_2b[1]))
	    + ((lambda)     * (grad4_4_1a[1] + grad4_4_2a[1]))
	    - ((lambda)     * (grad4_4_1b[1] + grad4_4_2b[1]));
   
   gterm4[i][0] = grad4[0];
   gterm4[i][1] = grad4[1];
   }
   
  invcovTempLocal = NULL;

  return 0;
}
