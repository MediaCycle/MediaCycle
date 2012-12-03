//
//  Sne.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//  This class is an adaptation of the matlab code written by Laurens van der Maaten, 2008 ( Maastricht University). The algorithm is described in:
//L.J.P. van der Maaten and G.E. Hinton. Visualizing High-Dimensional Data Using t-SNE. Journal of Machine Learning Research 9(Nov):2579-2605, 2008
//http://homepage.tudelft.nl/19j49/t-SNE.html

#include "Sne.h"

using namespace arma;

Sne::Sne(){
    kk=10;
    tol=1e-4;
}

bool Sne::setDistanceMatrix(arma::mat D){
    if (D.is_square()==false)
        return false;
    if (prod(prod(D!=D.t())))
        return false;
    D=D/max(max(D));
    D=pow(D,2);
    cout<<"D:"<<D<<endl;
    return d2p(D);
}

bool Sne::d2p(arma::mat Dist){
    
    uword N=Dist.n_cols;
    P.resize(N,N);
    colvec beta=ones(N,1);
    double log2K=log2(kk);
    for (uword i=0;i<N;i++){
        cout<<"Sne::d2p"<<i<<endl;
        double betamin=-1*datum::inf;
        double betamax=datum::inf;
        double H;
        rowvec thisP;
        Hbeta(H,thisP,Dist.row(i),i,beta(i));
        double Hdiff=H-log2K;
        uword tries=0;
        while ((fabs(Hdiff)>tol)&&(tries<50)){
            if (Hdiff>0){
                betamin=beta(i);
                if (is_finite(betamax)==false){
                    beta(i)=beta(i)*2;
                }
                else{
                    beta(i)=(beta(i)+betamax)/2;
                }
            }
            else{
                betamax=beta(i);
                if (is_finite(betamin)==false){
                    beta(i)=beta(i)/2;
                }
                else{
                    beta(i)=(beta(i)+betamin)/2;
                }
            }
            Hbeta(H,thisP,Dist.row(i),i,beta(i));
            Hdiff=H-log2K;
            tries++;
        }
        P.row(i)=thisP;
    }
    cout<<"Mean value of sigma: "<<mean(sqrt(1/beta))<<endl;
    cout<<"Minimum value of sigma: "<<min(sqrt(1/beta))<<endl;
    cout<<"Maximum value of sigma: "<<max(sqrt(1/beta))<<endl;
    cout<<"P:"<<endl<<P<<endl;
    return true;
}

void Sne::Hbeta(double &H,rowvec &thisP,rowvec Dist,int ind,double beta){
    thisP.resize(Dist.size());
    thisP=exp(-Dist*beta);
    double sumP=sum(thisP)-exp(-Dist(ind)*beta);
    
    //H=log(sumP)+beta*sum(Dist%thisP)/sumP;
    thisP=thisP/sumP;
    rowvec tempH=(-1*thisP*log2(trans(thisP)));
    H=tempH.at(0);
    return;
    
}

bool Sne::setFeatureMatrix(arma::mat F){//TODO Verify if it's necessay an ACP or not
    cout<<"F:"<<F<<endl;
    mat D(F.n_rows,F.n_rows) ;
    for (uword i=0;i<F.n_rows;i++){
        for (uword j=0;j<i;j++){
            D(j,i)=(sum(pow((F.row(i)-F.row(j)),2)));
        }
        D(i,i)=0.0;
        
    }
    D=symmatu(D);
    return d2p(D);
}
arma::mat Sne::compute(int ndim){
    int n=this->P.n_rows;
    double momentum = 0.5;                              // initial momentum
    double final_momentum = 0.8;                               // value to which momentum is changed
    uword mom_switch_iter = 250;                              // iteration at which momentum is changed
    uword stop_lying_iter = 100;                              // iteration at which lying about P-values is stopped
    uword max_iter = 1000;                                    // maximum number of iterations
    uword epsilon = 1;                                      // initial learning rate
    double min_gain = .01;                                     // minimum gain for delta-bar-delta
    
    double _const=sum(sum(P%log(P)));
    
    // Initialize the solution
    mat ydata(n,ndim);
    ydata=0.0001*randn(n,ndim);
    mat y_incs  = zeros(ydata.n_rows,ydata.n_cols);
    mat gains = ones(ydata.n_rows,ydata.n_cols);
    for (int iter=0;iter<max_iter;iter++){
        //cout<<"ydata:"<<endl<<ydata<<endl;
        colvec sum_ydata=sum(pow(ydata,2),1);
        mat Dy2=zeros(n,n);
        for (uword j=0;j<n;j++)
            for (uword i=0;i<j;i++){
                Dy2(i,j)=(sum(pow((ydata.row(i)-ydata.row(j)),2)));
            }
        Dy2=symmatu(Dy2);
        Dy2=exp(-1*Dy2);
        mat Q(n,n);
        double sumDy2=fmax(sum(Dy2.row(0).subvec(1,n-1)),datum::eps);
        Q.row(0)=Dy2.row(0)/sumDy2;
        for (uword i=1;i<n-1;i++){
            rowvec tempDy2(n-1);
            tempDy2.subvec(0, i-1)=Dy2.row(i).subvec(0, i-1);
            tempDy2.subvec(i, n-2)=Dy2.row(i).subvec(i+1, n-1);
             sumDy2=fmax(sum(tempDy2),datum::eps);
            Q.row(i)=Dy2.row(i)/sumDy2;
        }
        sumDy2=fmax(sum(Dy2.row(n-1).subvec(0,n-2)),datum::eps);
        Q.row(n-1)=Dy2.row(n-1)/sumDy2;
        mat L=P-Q;
        L=L+trans(L);
        mat y_grads(ydata.n_rows,ydata.n_cols);
        for (uword i=0;i<n;i++){
            mat ydiff(ydata.n_rows,ydata.n_cols);
            for (uword j=0;j<n;j++)
                ydiff.row(j)=ydata.row(i)-ydata.row(j);
            y_grads.row(i)=2*L.row(i)*ydiff;
        }
        cout<<"y_grads"<<y_grads<<endl;
        gains=(gains+0.2)%((y_grads%y_incs)<=0)+(gains*0.8)%((y_grads%y_incs)>0);
        gains=(gains>=min_gain)%gains+(gains<min_gain)*min_gain;
        cout<<"gains"<<gains<<endl;
        y_incs=momentum*y_incs+epsilon*(gains%y_grads);
        cout<<"y_incs"<<y_incs<<endl;
        ydata=ydata+y_incs;
        ydata=ydata-ones(n,1)*mean(ydata,0);
        if (iter==mom_switch_iter){
            momentum = final_momentum;
        }
        float cost=_const-sum(sum(P%log(Q)));
        cout<<"Iteration "<<iter<<": error is "<<cost<<endl;
    }
    ydata=ydata-ones(n,1)*mean(ydata);
    ydata=ydata/(ones(n,1)*stddev(ydata));
    cout<<"ydata:"<<endl<<ydata<<endl;
    return ydata;
}