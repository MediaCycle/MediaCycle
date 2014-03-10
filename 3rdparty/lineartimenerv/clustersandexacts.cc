/*Copyright (C) Jaakko Peltonen
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#define mymalloc malloc
#define myrealloc realloc
#define myfree free
#define myprintf printf

#define MAX_LINELENGTH 1024

#define MYPI 3.14159265358979310862446895044

#include<cstdlib>
#include<math.h>
#include<float.h>
#include<stdexcept>
#include<stdio.h>

#include <newmat.h>

namespace nm = NEWMAT;

double ** S_temp = NULL;
double ** C_temp = NULL;


/* Box-Muller method for generating normally distributed random variables */
int myrandn_hasy = 0;
double myrandn_part1;
double myrandn_v;
double myrandn()
{
    double u, x;

    if (myrandn_hasy == 0)
    {
        myrandn_hasy = 1;
        u = rand()/(double)RAND_MAX;
        myrandn_part1 = sqrt(-2*log(u));
        myrandn_v = rand()/(double)RAND_MAX;
        x = myrandn_part1*cos(2*MYPI*myrandn_v);
        if (isnan(x))
        {
            //throw std::runtime_error("LinearNeRV Error: x=%e, myrandn_part1=%e,myrandn_v=%e,u=%e\n",x,myrandn_part1,myrandn_v,u);
            throw std::runtime_error("LinearNeRV Error: NaN when generating normally distributed random variables");
            //exit(1);
        }
    }
    else
    {
        myrandn_hasy = 0;
        x = myrandn_part1*sin(2*MYPI*myrandn_v);
        if (isnan(x))
        {
            //throw std::runtime_error("LinearNeRV Error: x=%e, myrandn_part1=%e,myrandn_v=%e\n",x,myrandn_part1,myrandn_v);
            throw std::runtime_error("LinearNeRV Error: NaN when generating normally distributed random variables");
            //exit(1);
        }
    }
    return(x);
}


void weightedkmeans
(
        int ndata,
        int ndim,
        double **x,
        int nclust,
        int niter,
        int **result_clustind,
        double ***result_clustmeans,
        double **result_clustsizes,
        double ****result_clustcovs,
        double *result_cost
        )
{
    int i, j, k, l, m;
    int bestclust;
    double bestdist, tempdist;
    double sum_mindist;
    int *clustind;
    double **clustmeans;
    double *clustsizes;
    double ***clustcovs;

    /* Initialize cluster means at random data points */
    clustmeans = (double **)malloc(nclust*sizeof(double *));
    for (k=nclust-1;k>=0;k--)
    {
        clustmeans[k] = (double *)malloc(ndim*sizeof(double));
        l=rand()%ndata;
        for (m=ndim-1;m>=0;m--)
            clustmeans[k][m] = x[l][m];
    }

    clustsizes = (double *)malloc(nclust*sizeof(double));

    clustind = (int *)malloc(ndata*sizeof(int));



    if (nclust>0)
        for (i=niter-1;i>=0;i--)
        {
            for (k=nclust-1;k>=0;k--)
                clustsizes[k] = 0;

            sum_mindist = 0;
            /* Compute distances to clusters, assign points to closest cluster */
            for (j=ndata-1;j>=0;j--)
            {
                bestclust=-1;
                bestdist=DBL_MAX;
                for (k=nclust-1;k>=0;k--)
                {
                    tempdist=0;
                    for (l=ndim-1;l>=0;l--)
                        tempdist += (x[j][l]-clustmeans[k][l])*(x[j][l]-clustmeans[k][l]);
                    if (tempdist<bestdist)
                    {
                        bestclust=k;
                        bestdist=tempdist;
                    }
                }

                sum_mindist += bestdist*1; /*xweights[j];*/
                clustsizes[bestclust] += 1; /*xweights[j];*/
                clustind[j] = bestclust;
            }


            /* Update cluster means */
            for (k=nclust-1;k>=0;k--)
                for (l=ndim-1;l>=0;l--)
                    clustmeans[k][l] = 0;
            for (j=ndata-1;j>=0;j--)
            {
                bestclust=clustind[j];
                for (l=ndim-1;l>=0;l--)
                    clustmeans[bestclust][l] += /* xweights[j]* */ x[j][l];
            }

            /* Normalize non-empty clusters, reinitialize empty ones at random data points. */
            for (k=nclust-1;k>=0;k--)
            {
                if (clustsizes[k] == 0)
                {
                    l=rand()%ndata;
                    for (m=ndim-1;m>=0;m--)
                        clustmeans[k][m] = x[l][m];
                }
                else
                {
                    for (m=ndim-1;m>=0;m--)
                        clustmeans[k][m] /= clustsizes[k];
                }
            }

        } /* End of loop over kmeans iterations */

    /* Initialize covariances */
    clustcovs=(double ***)malloc(nclust*sizeof(double **));
    for (k=nclust-1;k>=0;k--)
    {
        clustcovs[k] = (double **)malloc(ndim*sizeof(double *));
        for (l=ndim-1;l>=0;l--)
        {
            clustcovs[k][l] = (double *)malloc(ndim*sizeof(double));
            for (m=ndim-1;m>=0;m--)
                clustcovs[k][l][m] = 0;
        }
    }

    /* Calculate covariances around clusters */
    if (nclust>0)
        for (j=ndata-1;j>=0;j--)
        {
            bestclust=clustind[j];
            for (l=ndim-1;l>=0;l--)
                for (m=ndim-1;m>=0;m--)
                    clustcovs[bestclust][l][m] += (x[j][l]-clustmeans[bestclust][l])*(x[j][m]-clustmeans[bestclust][m])*1; /* *xweights[j]; */
        }

    /* Normalize covariances */
    for (k=nclust-1;k>=0;k--)
    {
        /* Reset empty clusters to nonzero covariance */
        if (clustsizes[k] == 0)
        {
            for (m=ndim-1;m>=0;m--)
                clustcovs[k][m][m] = 1;
        }
        else
        {
            for (l=ndim-1;l>=0;l--)
                for (m=ndim-1;m>=0;m--)
                    clustcovs[k][l][m] /= clustsizes[k];
        }
    }



    /* Compute final cost */
    sum_mindist = 0;
    for (j=ndata-1;j>=0;j--)
    {
        bestclust=-1;
        bestdist=DBL_MAX;
        for (k=nclust-1;k>=0;k--)
        {
            tempdist=0;
            for (l=ndim-1;l>=0;l--)
                tempdist += (x[j][l]-clustmeans[k][l])*(x[j][l]-clustmeans[k][l]);
            if (tempdist<bestdist)
            {
                bestclust=k;
                bestdist=tempdist;
            }
        }
        sum_mindist += bestdist; /* *xweights[j]; */
    }
    myprintf("K-means final cost: %e\n", sum_mindist);


    *result_clustind = clustind;
    *result_clustmeans = clustmeans;
    *result_clustsizes = clustsizes;
    *result_clustcovs = clustcovs;
    *result_cost = sum_mindist;
}


void computeClusterMemberships
(
        int ndata,
        int ndim,
        double **x,
        /* double *xweights, */
        int nclust,
        int *clustind,
        double **clustmeans,
        double *clustsizes,
        double ***clustcovs,
        double ***result_clustmemberships
        )
{
    static nm::Matrix *invcovTempLocal = NULL;
    double precisionfix;
    static double ***clustinvcovs;
    static double *clustdets = NULL;
    double **clustmemberships;
    int i, j, k, l, m;

    //printf("Computing inverses of cluster covariances, step1\n");
    if (invcovTempLocal == NULL)
    {
        printf("Allocating new matrix\n");
        invcovTempLocal = new nm::Matrix(ndim,ndim);
    }


    clustmemberships = (double **)malloc(ndata*sizeof(double *));
    for (k=ndata-1;k>=0;k--)
    {
        clustmemberships[k] = (double *)malloc(nclust*sizeof(double));
    }

    clustdets = (double *)malloc(nclust*sizeof(double));
    printf("Computing inverses of cluster covariances, step2\n");
    clustinvcovs = (double ***)malloc(nclust*sizeof(double **));
    for (i=nclust-1;i>=0;i--)
    {
        clustinvcovs[i] = (double **)malloc(ndim*sizeof(double *));
        for (j = ndim-1; j>=0; j--)
            clustinvcovs[i][j] = (double *)malloc(ndim*sizeof(double));
    }

    printf("CheckSeg \n");
    printf("Computing inverses of cluster covariances, step3\n");
    for (k = nclust-1; k >= 0; k--)
    {
        precisionfix = (1e-8);
        int inverse_ok = 0;
        while (inverse_ok == 0)
        {
            for (l = ndim-1; l >= 0; l--)
            {
                for (m = ndim-1; m >= 0; m--)
                {
                    (*invcovTempLocal)(l+1, m+1) = clustcovs[k][l][m];
                    if (l == m) (*invcovTempLocal)(l+1, m+1) += precisionfix;
                }
            }
            printf("Computing inverses of cluster covariances, step3, k=%d\n", k);
            (*invcovTempLocal) = (*invcovTempLocal).i(); // inv(Sp)
            clustdets[k] = (*invcovTempLocal).Determinant();

            inverse_ok = 1;
            for (l = ndim-1; l >= 0; l--)
            {
                for (m = ndim-1; m > l; m--)
                {
                    double tempdifference = (*invcovTempLocal)(l+1,m+1) - (*invcovTempLocal)(m+1,l+1);
                    if ((tempdifference < -1) || (tempdifference > 1))
                        inverse_ok = 0;
                }
            }
            precisionfix = precisionfix*10;
        }
        for (l = ndim-1; l >= 0; l--)
        {
            for (m = ndim-1; m >= 0; m--)
            {
                clustinvcovs[k][l][m] = (*invcovTempLocal)(l+1,m+1);
                printf("%e ", (*invcovTempLocal)(l+1,m+1));
            }
            printf("\n");
        }
    }


    for (j=ndata-1;j>=0;j--)
    {
        // Compute distances to clusters and the minimum distance
        int bestclust=-1;
        double bestdist=DBL_MAX;
        for (k=nclust-1;k>=0;k--)
        {
            double tempdist=0;
            for (l=ndim-1;l>=0;l--)
                for (m=ndim-1;m>=0;m--)
                    tempdist += (x[j][l]-clustmeans[k][l])*clustinvcovs[k][l][m]*(x[j][m]-clustmeans[k][m]);
            clustmemberships[j][k] = tempdist;
            if (tempdist<bestdist)
            {
                bestclust=k;
                bestdist=tempdist;
            }
        }
        double tempsum = 0;
        for (k=nclust-1;k>=0;k--)
        {
            clustmemberships[j][k] = clustsizes[k]*exp(-(clustmemberships[j][k]-bestdist)/2)/(pow(2*MYPI,ndim*0.5)*sqrt(clustdets[k]));
            tempsum += clustmemberships[j][k];
        }
        for (k=nclust-1;k>=0;k--)
        {
            clustmemberships[j][k] /= tempsum;
        }
    }

    *result_clustmemberships = clustmemberships;
}


void initializeExactNeighbors
(
        int ndata,
        int ndim,
        int nexact,
        int nclust,
        int *clustind,
        double *clustsizes,
        int ***result_exact
        )
{
    int i, j, k, l, m, j2, m2;
    int clustind_i;
    int **exact;
    int **clusterpoints;
    int *clustnfound;
    int nexact_samecluster, nexact_otherclusters;

    int *pointpermutation;   // An ordering of all the data points 0,...,ndata-1.
    int *reversepermutation; // A reverse lookup: reversepermutation[i] is the
    //   order of number i in the permutation, so that
    //   pointpermutation[reversepermutation[i]] = i.
    int **clusterwise_permutations;        // Same for the points within each cluster
    int **clusterwise_reversepermutations;


    /* Allocate and compute a reverse lookup table from clusters to points in them */
    clusterpoints = (int **)mymalloc(nclust*sizeof(int *));
    clustnfound = (int *)mymalloc(nclust*sizeof(int));
    for (i = nclust-1; i >= 0; i--)
    {
        clustnfound[i] = 0;
        clusterpoints[i] = (int *)mymalloc((int)(clustsizes[i])*sizeof(int));
    }
    if (nclust>0)
    {
        for (i = ndata-1; i >= 0; i--)
        {
            j = clustind[i];
            clusterpoints[j][clustnfound[j]] = i;
            clustnfound[j]++;
        }
    }

    /* Allocate result table */
    exact = (int **)mymalloc(ndata*sizeof(int *));
    for (i = ndata-1; i >= 0; i--)
        exact[i] = (int *)mymalloc(nexact*sizeof(int));

    /* Allocate a table for an overall permutation of the data */
    pointpermutation = (int *)mymalloc(ndata*sizeof(int));
    reversepermutation = (int *)mymalloc(ndata*sizeof(int));
    /* Initialize the permutation */
    for (i = ndata-1; i >= 0; i--)
    {
        pointpermutation[i] = i;   /* The i:th number in the permutation is i */
        reversepermutation[i] = i; /* The order of i in the permutation is i */
    }


    /* For each cluster, allocate a table for a permutation of the data in the cluster */
    clusterwise_permutations = (int **)mymalloc(nclust*sizeof(int *));
    clusterwise_reversepermutations = (int **)mymalloc(nclust*sizeof(int *));
    for (k = nclust-1; k >= 0; k--)
    {
        clusterwise_permutations[k] = (int *)mymalloc(((int)clustsizes[k])*sizeof(int));
        clusterwise_reversepermutations[k] = (int *)mymalloc(ndata*sizeof(int));
        /* Initialize the permutation */

        for (i = ndata-1; i >= 0; i--)
        {
            clusterwise_reversepermutations[k][i] = -i;
        }

        for (i = (int)(clustsizes[k])-1; i >= 0; i--)
        {
            l = clusterpoints[k][i];
            clusterwise_permutations[k][i] = l;
            clusterwise_reversepermutations[k][l] = i;
        }
    }


    for (i = ndata-1; i >= 0; i--)
    {
        if ((i % 100) == 0)
            myprintf("Finding exact neighbors for data point %d\n", i);


        clustind_i = clustind[i];


        /* First always arrange that the point i itself is taken out:
       Swap i with the current first point in the overall permutation,
       and do the same within the clusterwise permutation. */
        k = reversepermutation[i]; /* This is where i currently is in the overall permutation */
        j = pointpermutation[0];        /* This is the number i needs to be swapped with */


        /* Place i at the start */
        reversepermutation[i] = 0;
        pointpermutation[0] = i;
        /* Place j where i used to be */
        reversepermutation[j] = k;
        pointpermutation[k] = j;

        if (nclust>0)
        {
            k = clusterwise_reversepermutations[clustind_i][i]; /* This is where i currently is in the clusterwise permutation */
            j = clusterwise_permutations[clustind_i][0];        /* This is the number i needs to be swapped with */

            /* Place i at the start */
            clusterwise_reversepermutations[clustind_i][i] = 0;
            clusterwise_permutations[clustind_i][0] = i;
            /* Place j where i used to be */
            clusterwise_reversepermutations[clustind_i][j] = k;
            clusterwise_permutations[clustind_i][k] = j;
        }

        /* Take the exact neighbors first from the same cluster as point i, only
       afterwards from other clusters. */
        if (nclust>0)
        {
            if (clustsizes[clustind_i] >= nexact+1)
            {
                nexact_samecluster = nexact;
                nexact_otherclusters = 0;
            }
            else
            {
                nexact_samecluster = clustsizes[clustind_i]-1;
                nexact_otherclusters = nexact - nexact_samecluster;
            }
        }
        else
        {
            nexact_samecluster = 0;
            nexact_otherclusters = nexact;
        }

        for (k = 0; k < nexact_samecluster; k++)
        {
            /* The (k+1) numbers at the start of the permutation are taken */
            m = k+1 + (rand()%((int)(clustsizes[clustind_i]) - (k+1))); /* We take the m:th point in the clusterwise permutation */
            m2 = clusterwise_permutations[clustind_i][m];

            /* Take the point "m2" into use: swap it with point (k+1)
         in both the clusterwise permutation and in the overall permutation */
            j2 = clusterwise_permutations[clustind_i][k+1];        /* This is the number m2 needs to be swapped with */
            /* Place m2 at order k+1 */
            clusterwise_reversepermutations[clustind_i][m2] = k+1;
            clusterwise_permutations[clustind_i][k+1] = m2;
            /* Place j2 where m2 used to be */
            clusterwise_reversepermutations[clustind_i][j2] = m;
            clusterwise_permutations[clustind_i][m] = j2;



            m = reversepermutation[m2];    /* This is where m2 currently is in the overall permutation */
            j2 = pointpermutation[k+1];    /* This is the number m2 needs to be swapped with */

            /* Place m2 at order k+1 */
            reversepermutation[m2] = k+1;
            pointpermutation[k+1] = m2;
            /* Place j2 where m2 used to be */
            reversepermutation[j2] = m;
            pointpermutation[m] = j2;
        }

        /* Take the remaining neighbors from other clusters */
        for (k = 0; k < nexact_otherclusters; k++)
        {
            /* The (nexact_samecluster+k+1) numbers at the start of the permutation are taken */
            m = nexact_samecluster+k+1 + (rand()%(ndata - (nexact_samecluster+k+1))); /* We take the m:th point in the overall permutation */
            m2 = pointpermutation[m];

            /* Take the point "m2" into use: swap it with point (nexact_samecluster+k+1)
         in the overall permutation */
            j2 = pointpermutation[nexact_samecluster+k+1];        /* This is the number m2 needs to be swapped with */
            /* Place m2 at order nexact_samecluster+k+1 */
            reversepermutation[m2] = nexact_samecluster+k+1;
            pointpermutation[nexact_samecluster+k+1] = m2;
            /* Place j2 where m2 used to be */
            reversepermutation[j2] = m;
            pointpermutation[m] = j2;
        }

        /* Now the first point in "pointpermutation" is i, and the next
       nexact points are the selected exact neighbors. The
       permutations are all still valid permutations so they can be
       used as starting points for processing the next query point i */
        for (k = 0; k < nexact; k++)
        {
            exact[i][k] = pointpermutation[k+1];
        }
    }

    for (k = 0; k < nclust; k++)
    {
        myfree(clusterpoints[k]); clusterpoints[k] = NULL;
    }
    myfree(clusterpoints); clusterpoints = NULL;
    myfree(clustnfound); clustnfound = NULL;
    myfree(pointpermutation);
    myfree(reversepermutation);

    for (k = 0; k < nclust; k++)
    {
        myfree(clusterwise_permutations[k]); clusterwise_permutations[k] = NULL;
        myfree(clusterwise_reversepermutations[k]); clusterwise_reversepermutations[k] = NULL;
    }
    myfree(clusterwise_permutations); clusterwise_permutations = NULL;
    myfree(clusterwise_reversepermutations); clusterwise_reversepermutations = NULL;

    
    *result_exact = exact;
    exact = NULL;
}

