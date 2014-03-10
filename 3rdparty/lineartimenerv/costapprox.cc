/*Copyright (C) Konstantinos Georgatzis
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#define PI 3.1415926535897932384626433832795
#define maxcost (1e+12)
#define cluster_tol (1e-3)


double **tempXquadratic_precompute1 = NULL;
double **tempXquadratic_precompute2 = NULL;

void compute_exactdistsOrig
(
  double **X, int **exact, int nexact, int ndata, int nXdim, double **exactXdists
)
{
  int i, k, l;
  double *exactXdists_i, *X_i, *X_exactj;
  double tempdist;
  int *exact_i;
  
  for (i = ndata-1; i >= 0; i--)
  {
    exact_i = exact[i];
    exactXdists_i = exactXdists[i];
    X_i = X[i];
    
   for (k = nexact-1; k >= 0; k--)
   {
     X_exactj = X[exact_i[k]];

     tempdist = 0;
     for (l = nXdim-1; l >= 0; l--)
     {
       tempdist += ( X_i[l] - X_exactj[l] ) * ( X_i[l] - X_exactj[l] );
     }     
     exactXdists_i[k] = tempdist;
    }
  }
}


void compute_exactdistsDisplay
(
  double **Y, int **exact, int nexact, int ndata, int nYdim, double **exactdists  
)
{
  int i, k, l;
  double *exactdists_i, *Y_i, *Y_exactj;
  double tempdist;
  int *exact_i;
  
  for (i = ndata-1; i >= 0; i--)
  {
    exact_i = exact[i];
    exactdists_i = exactdists[i];
    Y_i = Y[i];   

    for (k = nexact-1; k >= 0; k--)
   {

     Y_exactj = Y[exact_i[k]];
          
     tempdist = 0;
     for (l = nYdim-1; l >= 0; l--)
     {
       tempdist += ( Y_i[l] - Y_exactj[l] ) * ( Y_i[l] - Y_exactj[l] );
     }
     exactdists_i[k] = tempdist;
    }
  }    
}


 /* Compute logKy cost term */

double compute_sumlogKy(
  double **Y, double *sigmaSqrd, double **clusterYmeans, double ***clusterYcovs, int **exact, double **exactdists, double *Ky_values, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim
)
{ 
  double sumlogKy = 0;
  double Kytemp, Ky1 = 0, Ky2 = 0, det = 0, detQ = 0;
  int i, k, m, n;
  double tempquadratic;  
  double invCov[2][2];

  sumlogKy = 0;
  for (i = ndata-1; i >= 0; i--)
  {   
    Ky1 = 0; Ky2 = 0; Kytemp = 0;
    for (k = nexact-1; k >= 0; k--)
    {
     Kytemp = exp(-exactdists[i][k]/(2*sigmaSqrd[i]));
     Ky1 += Kytemp;
    }

    for (k = nclusters-1; k >= 0; k--)
    {      
      if(tempclustersizes[i][k] > cluster_tol)
      {
      det = (clusterYcovs[k][0][0] + sigmaSqrd[i]) * (clusterYcovs[k][1][1] + sigmaSqrd[i]) - (clusterYcovs[k][0][1]) * (clusterYcovs[k][1][0]);
      invCov[0][0] = (clusterYcovs[k][1][1] + sigmaSqrd[i]) / det;
      invCov[0][1] = -clusterYcovs[k][0][1] / det;
      invCov[1][0] = -clusterYcovs[k][1][0] / det;
      invCov[1][1] = (clusterYcovs[k][0][0] + sigmaSqrd[i]) / det;	
      
      tempquadratic = 0;
        for (m = nYdim-1; m >= 0; m--)
        {  
          for (n = nYdim-1; n >= 0; n--)
          {	 
         tempquadratic += (Y[i][m]-clusterYmeans[k][m])*invCov[m][n]*(Y[i][n]-clusterYmeans[k][n]);
          }       
        }    	
        
       detQ = ( 1 / sqrt( (clusterYcovs[k][0][0] / sigmaSqrd[i] + 1) * (clusterYcovs[k][1][1] / sigmaSqrd[i] + 1) - (clusterYcovs[k][0][1] / sigmaSqrd[i]) * (clusterYcovs[k][1][0] / sigmaSqrd[i]) ) );
       Kytemp = tempclustersizes[i][k] * detQ * exp( (-tempquadratic) / 2);
       Ky2 += Kytemp;	       
      }
    }    

#define KYTOLERANCE (1e-6)
   if ( (Ky1 + Ky2) > KYTOLERANCE)
   {
     Ky_values[i] = Ky1 + Ky2;   
   }
   else
   {
     Ky_values[i] = KYTOLERANCE;
   }
   sumlogKy = sumlogKy + log(Ky_values[i]);
  }
  return(sumlogKy);
}

/* Compute Psimple cost term */
double compute_sumPsimple(
  double **Y, double *sigmaSqrd, double **clusterYmeans, double ***clusterYcovs, int **exact, double **exactdists, double *Ky_values, double *Psimple_values, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim
)
{
  double sumPsimple = 0;
  double Ps1 = 0, Ps1n = 0, Ps2 = 0, z_p = 0;
  int i, j, k, m, n;
  double tempquadratic, tempquadratic2;  
  double Sigma_z[2][2], invCov[2][2];
  double mu_z[2];
  double det_Sz, det_S, det = 0;
  double Tr_Sz = 0;

  sumPsimple = 0;
  for (i = ndata-1; i >= 0; i--)
  {         
  Ps1n = 0; 
  for (j = nexact-1; j >= 0; j--)
  {
    Ps1n += exp(-exactdists[i][j]/(2*sigmaSqrd[i]))*(-exactdists[i][j]/(2*sigmaSqrd[i]));    
  }
  
  Ps1 = Ps1n / Ky_values[i];

  Ps2 = 0;
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

    Ps2 +=  (- z_p * tempclustersizes[i][k] * (tempquadratic + (Tr_Sz / sigmaSqrd[i]) )) / 2;
    }
  }
    
   Ps2 = ( sqrt(pow(2 * PI, nYdim) * sigmaSqrd[i] * sigmaSqrd[i]) * Ps2) / (Ky_values[i]);
   Psimple_values[i] = Ps1 + Ps2;
   sumPsimple += Psimple_values[i];
  }

  return(sumPsimple);
}

/* Compute Pcross term */

double compute_sumPcross(
    double **Y, double **X, double *sigmaSqrd, double **clusterYmeans, double ***clusterYcovs, double **clusterXmeans, double ***clusterXcovs, int **exact, double **exactdists, double **exactXdists, double *Ky_values, double *Pcross_values, 
    double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim, int nXdim
)
{
  double sumPcross = 0;
  double Pcross1 = 0, Pcross2 = 0;
  int i, k, m, n;
  double tempquadratic, tempXquadratic;  
  double det;
  double invCovsigmaQ[2][2];

  sumPcross = 0;
  for (i = ndata-1; i >= 0; i--)
  {
   Pcross1 = 0;    
   for (k = nexact-1; k >= 0; k--)
   {
     Pcross1 += (exp(-exactdists[i][k]/(2*sigmaSqrd[i])) * ( -exactXdists[i][k]/(2*sigmaSqrd[i]) ) ) / Ky_values[i];     
   }
   
   Pcross2 = 0; 
   for (k = nclusters-1; k >= 0; k--)
   {
     if(tempclustersizes[i][k] > cluster_tol)
     {
     det = (clusterYcovs[k][0][0] + sigmaSqrd[i]) * (clusterYcovs[k][1][1] + sigmaSqrd[i]) - (clusterYcovs[k][0][1]) * (clusterYcovs[k][1][0]);

     invCovsigmaQ[0][0] = (clusterYcovs[k][1][1] + sigmaSqrd[i]) / det;    invCovsigmaQ[0][1] = (-clusterYcovs[k][0][1]) / det;
     invCovsigmaQ[1][0] = (-clusterYcovs[k][1][0]) / det;                  invCovsigmaQ[1][1] = (clusterYcovs[k][0][0] + sigmaSqrd[i]) / det;

     tempquadratic = 0;
     for (m = nYdim-1; m >= 0; m--)
     {  
       for (n = nYdim-1; n >= 0; n--)
       {
         tempquadratic += (Y[i][m]-clusterYmeans[k][m])*invCovsigmaQ[m][n]*(Y[i][n]-clusterYmeans[k][n]);
       }       
     }

     tempXquadratic = tempXquadratic_precompute1[i][k];
     
     Pcross2 += (tempclustersizes[i][k] * ( 1 / sqrt( (clusterYcovs[k][0][0] / sigmaSqrd[i] + 1 ) * ( clusterYcovs[k][1][1] / sigmaSqrd[i] + 1 ) - ( clusterYcovs[k][0][1] / sigmaSqrd[i] ) * (clusterYcovs[k][1][0] / sigmaSqrd[i]) ) ) * exp( (-tempquadratic) / 2) * tempXquadratic) / ( -2 * Ky_values[i]); 
     } 
    }

   Pcross_values[i] = Pcross1 + Pcross2;
   sumPcross += Pcross_values[i];
  }

  return(sumPcross);
}


void computeCovMatrixInverses
(
  double ***clusterXcovs, double ***invCovX, int nclusters, int nXdim,
  void **invcovTempPointer  
)
{
  int k, l, m;
  nm::Matrix *invcovTempLocal;
  double precisionfix;  
  
  if ((*invcovTempPointer) == NULL)
  {
    printf("Allocating new matrix\n");
    invcovTempLocal = new nm::Matrix(nXdim,nXdim);
    *invcovTempPointer = invcovTempLocal;
  }
  invcovTempLocal = (nm::Matrix *)(*invcovTempPointer);

  for (k = nclusters-1; k >= 0; k--)
  {
    precisionfix = (1e-8);
    int inverse_ok = 0;
    while (inverse_ok == 0)
    {
      for (l = nXdim-1; l >= 0; l--)
      {
        for (m = nXdim-1; m >= 0; m--)
        {
          (*invcovTempLocal)(l+1, m+1) = clusterXcovs[k][l][m]; 
          if (l == m) (*invcovTempLocal)(l+1, m+1) += precisionfix;
        }
      }
      (*invcovTempLocal) = (*invcovTempLocal).i(); 
      
      inverse_ok = 1;
      for (l = nXdim-1; l >= 0; l--)
      {
        for (m = nXdim-1; m > l; m--)
        {
	  double tempdifference = (*invcovTempLocal)(l+1,m+1) - (*invcovTempLocal)(m+1,l+1);
	  if ((tempdifference < -1) || (tempdifference > 1))
	    inverse_ok = 0;
	}
      }
      precisionfix = precisionfix*10;
    }
    
    for (l = nXdim-1; l >= 0; l--)
    {
      for (m = nXdim-1; m >= 0; m--)
      {
        invCovX[k][l][m] = (*invcovTempLocal)(l+1,m+1);
      }
    }     
  }
  invcovTempLocal = NULL;  
}

double compute_sumlogKx(
  double **X, double *sigmaSqrd, int *clusterIndices, double **clusterXmeans, double ***clusterXcovs, double ***invCovX, int **exact, double **exactXdists, double *Kx_values, double **tempclustersizes, int nclusters, int nexact, int ndata, int nXdim, void **invcovTempPointer, double ***invCovType1, double *detType1
)
{
  double sumlogKx = 0;  
  double Kx1 = 0, Kx2 = 0;
  int i, k, l, m, n;
  double tempquadratic, detX;
  double ***invCovType1_i;
  double *detType1_i;
  
  nm::Matrix *invcovTempLocal;

  if ((*invcovTempPointer) == NULL)
  {
    printf("Allocating new matrix in compute_sumlogKx\n");    
    invcovTempLocal = new nm::Matrix(nXdim,nXdim);
    *invcovTempPointer = invcovTempLocal;
  }
  invcovTempLocal = (nm::Matrix *)(*invcovTempPointer);  

 for (k = nclusters-1; k >= 0; k--)
 {
   for (l = nXdim-1; l >= 0; l--)
   {
     for (m = nXdim-1; m >= 0; m--)
     {
       if (l == m)
       {
         (*invcovTempLocal)(l+1, m+1) = sigmaSqrd[0] + clusterXcovs[k][l][m]; // We haven't stored the overall sigma^2, so we just use the value for the first point since they all have the same value
       }
       else
       {
         (*invcovTempLocal)(l+1, m+1) = clusterXcovs[k][l][m];
       }
     }
   }
   detX = (*invcovTempLocal).Determinant();
   (*invcovTempLocal) = (*invcovTempLocal).i(); 

   for (l = nXdim-1; l >= 0; l--)
   {
     for (m = nXdim-1; m >= 0; m--)
     {
       invCovType1[k][l][m] = (*invcovTempLocal)(l+1,m+1);
     }
   }
   detType1[k] = detX;
 }

  if (tempXquadratic_precompute1 == NULL)
  {
    tempXquadratic_precompute1 = (double **)malloc(ndata*sizeof(double *));
    for (i = ndata-1; i >= 0; i--)
      tempXquadratic_precompute1[i] = (double *)malloc(nclusters*sizeof(double));
  }
  if (tempXquadratic_precompute2 == NULL)
  {
    tempXquadratic_precompute2 = (double **)malloc(ndata*sizeof(double *));
    for (i = ndata-1; i >= 0; i--)
      tempXquadratic_precompute2[i] = (double *)malloc(nclusters*sizeof(double));
  }

  sumlogKx = 0;
  for (i = ndata-1; i >= 0; i--)
  {  
    invCovType1_i = invCovType1;
    detType1_i = detType1;   

   Kx1 = 0;     
   for (k = nexact-1; k >= 0; k--)
   {
     Kx1 += exp(-exactXdists[i][k]/(2*sigmaSqrd[i]));     
   }

   Kx2 = 0;
   for (k = nclusters-1; k >= 0; k--)
   {
     if (tempclustersizes[i][k] > cluster_tol) 
     {  

     // Note: this quadratic is not needed for sumlogKx, it is needed later for Pcross but we compute it here since it does not change unless sigmaSqrd changes
     double tempXquadratic = 0;
     for (m = nXdim-1; m >= 0; m--)
     {  
       tempXquadratic += ( X[i][m] - clusterXmeans[k][m] ) * (1/sigmaSqrd[i]) * ( X[i][m] - clusterXmeans[k][m] );
     }
     double Tr_Sp = 0;
     for(m = nXdim-1; m >= 0; m--)
       Tr_Sp += clusterXcovs[k][m][m];     
     tempXquadratic += (Tr_Sp / sigmaSqrd[i]);
     tempXquadratic_precompute1[i][k] = tempXquadratic;
       
     // Note: this quadratic is not needed for sumlogKx, it is needed later for Rcross but we compute it here since it does not change unless sigmaSqrd changes
     tempXquadratic = 0;
     for (m = nXdim-1; m >= 0; m--)
     {  
       for (n = nXdim-1; n >= 0; n--)
       {	 
         tempXquadratic = tempXquadratic + (X[i][m]-clusterXmeans[k][m])*invCovType1_i[k][m][n]*(X[i][n]-clusterXmeans[k][n]);
       }       
     }
     tempXquadratic_precompute2[i][k] = tempXquadratic;

   
     tempquadratic = 0;
     for (m = nXdim-1; m >= 0; m--)
     {  
       for (n = nXdim-1; n >= 0; n--)
       {	 
         tempquadratic = tempquadratic + (X[i][m]-clusterXmeans[k][m]) * invCovType1_i[k][m][n] * (X[i][n]-clusterXmeans[k][n]); // invCov requires k index in this case (cf. Ky)
       }       
     }
     Kx2 += tempclustersizes[i][k] * ( 1 / sqrt(pow(sigmaSqrd[i],-nXdim)*detType1_i[k]) )  * exp( (-tempquadratic) / 2 );
     }
   }
    
   Kx_values[i] = Kx1+Kx2;
   sumlogKx = sumlogKx + log(Kx_values[i]);   
  }

  invcovTempLocal = NULL;
  
  return(sumlogKx);
}

double compute_sumRcross(
  double **Y, double **X, double *sigmaSqrd, double **clusterYmeans, double ***clusterYcovs, double **clusterXmeans, double ***clusterXcovs, double ***invCovX, int **exact, double **exactdists, double **exactXdists, double *Kx_values, double **tempclustersizes, int nclusters, int nexact, int ndata, int nYdim, int nXdim, double ***invCovType1, double *detType1
)

{
  double sumRcross = 0;
  double Rcross1 = 0, Rcross2 = 0;
  int i, k, m;
  double tempquadratic, tempXquadratic;  
  double Tr_Sq;
  double *detType1_i;

  sumRcross = 0;
  for (i = ndata-1; i >= 0; i--)
  {
    detType1_i = detType1;

    
   Rcross1 = 0;     
   for (k = nexact-1; k >= 0; k--)
   { 
     Rcross1 += (exp(-exactXdists[i][k]/(2*sigmaSqrd[i])) * (-exactdists[i][k]/(2*sigmaSqrd[i]))) / Kx_values[i];     
   }
   
   Rcross2 = 0; 
   for (k = nclusters-1; k >= 0; k--)
   { 
     if(tempclustersizes[i][k] > cluster_tol)
     {
     
     Tr_Sq = clusterYcovs[k][0][0] + clusterYcovs[k][1][1];
     
     tempquadratic = 0;
     for (m = nYdim-1; m >= 0; m--)
     {  
       tempquadratic = tempquadratic + (Y[i][m]-clusterYmeans[k][m])*(1/sigmaSqrd[i])*(Y[i][m]-clusterYmeans[k][m]);      
     }
     tempquadratic += Tr_Sq / sigmaSqrd[i]; 
     tempXquadratic = tempXquadratic_precompute2[i][k];

     Rcross2 += (tempclustersizes[i][k] * ( 1 / sqrt(pow(sigmaSqrd[i],-nXdim) * detType1_i[k]) ) * exp((-tempXquadratic) / 2) * tempquadratic) / (-2 * Kx_values[i]);
     } 
   }       
          
   
   sumRcross += Rcross1 + Rcross2; 
  }

  return(sumRcross);
 }

 double compute_sumRsimple(
  double **X, double *sigmaSqrd, double **clusterXmeans, double ***clusterXcovs, double ***invCovX, int **exact, double **exactXdists, double *Kx_values, double **tempclustersizes, int nclusters, int nexact, int ndata, int nXdim,
  void **invcovTempPointer, double ***invCovType1, double ***invCovType2, double *detType1
)
{
  double sumRsimple = 0;
  double Rs1 = 0, Rs1n = 0, Rs2 = 0, z_r = 0;
  int i, j, k, l, m, n;
  double tempquadratic, tempquadratic2;  

  static double *tempmult = NULL, *mu_z = NULL;
  
  double det = 0, detX2 = 0;
  double Tr_Sz = 0;
  nm::Matrix *invcovTempLocal;
  double ***invCovType2_i;
  double *detType1_i;
  
  if ((*invcovTempPointer) == NULL)
  { 
    printf("Allocating new matrix in compute_sumRsimple\n");
    invcovTempLocal = new nm::Matrix(nXdim,nXdim);
    *invcovTempPointer = invcovTempLocal;
  }
  invcovTempLocal = (nm::Matrix *)(*invcovTempPointer);
  
  if (tempmult == NULL)
  {
    tempmult = (double *) malloc(nXdim*sizeof(double));
    mu_z = (double *) malloc(nXdim*sizeof(double));
  }

  sumRsimple = 0;

  for (k = nclusters-1; k >= 0; k--)
  {
    for (l = nXdim-1; l >= 0; l--)
    {
      for (m = nXdim-1; m >= 0; m--)
      {
        if (l == m)
        {
          (*invcovTempLocal)(l+1, m+1) = 1/sigmaSqrd[0] + invCovX[k][l][m]; // Since we haven't stored the overall sigma^2, we use the value of the first point since they are all equal
        }
        else
        {
  	  (*invcovTempLocal)(l+1, m+1) = invCovX[k][l][m];
        }
      }
    }
    (*invcovTempLocal) = (*invcovTempLocal).i(); 
    for (l = nXdim-1; l >= 0; l--)
    {
      for (m = nXdim-1; m >= 0; m--)
      {
        invCovType2[k][l][m] = (*invcovTempLocal)(l+1,m+1);
      }
    }    
  }





  for (i = ndata-1; i >= 0; i--)
  {       
    invCovType2_i = invCovType2;
    detType1_i = detType1;

   /* compute numerator of first term of Rsimple */
  
  Rs1n = 0; 
  for (j = nexact-1; j >= 0; j--)
  {
    Rs1n += exp(-exactXdists[i][j]/(2*sigmaSqrd[i]))*(-exactXdists[i][j]/(2*sigmaSqrd[i]));    
  }
  
  /* Compute first term of Rsimple */  
  Rs1 = Rs1n / Kx_values[i];

  Rs2 = 0;
  for (k = nclusters-1; k >= 0; k--)
  { 
    if(tempclustersizes[i][k] > cluster_tol) 
    {

    Tr_Sz = 0;
    for(j = nXdim-1; j >= 0; j--)
    Tr_Sz += invCovType2_i[k][j][j]; 
    
     for (m = nXdim-1; m >= 0; m--)
     {  
       tempmult[m] = 0;
       for (n = nXdim-1; n >= 0; n--)
       {	 
	 tempmult[m] += invCovX[k][m][n] * clusterXmeans[k][n]; 
       }
       tempmult[m] += X[i][m] / sigmaSqrd[i];
     }
     
     for (m = nXdim-1; m >= 0; m--)
     {  
       mu_z[m] = 0;
       for (n = nXdim-1; n >= 0; n--)
       {	 
	 mu_z[m] += invCovType2_i[k][m][n] * tempmult[n]; 
       }
     }

    tempquadratic = 0;
    for (m = nXdim-1; m >= 0; m--)
    {  
      tempquadratic = tempquadratic + (X[i][m]-mu_z[m])*(1/sigmaSqrd[i])*(X[i][m]-mu_z[m]);
    } 

    tempquadratic2 = tempXquadratic_precompute2[i][k];
    
    
    det = detType1_i[k] ; // MATLAB
    
    z_r = (1 / sqrt( pow(2 * PI, nXdim) * det)) * exp((-tempquadratic2) / 2);

    Rs2 +=  (-z_r / 2) * tempclustersizes[i][k] * (tempquadratic + (Tr_Sz / sigmaSqrd[i]) );  
    }
  }
   detX2 = 1;
   for(m = nXdim-1; m >= 0; m--)
     detX2 = detX2 * sigmaSqrd[i]; 
   
   Rs2 = (sqrt(pow(2 * PI, nXdim) * detX2) * Rs2) / (Kx_values[i]); 
  
   sumRsimple += Rs1 + Rs2;
  }

  invcovTempLocal = NULL;
  
  return(sumRsimple);
}




