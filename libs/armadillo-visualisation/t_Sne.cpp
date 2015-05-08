//
//  t_Sne.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//  This class is an adaptation of the matlab code written by Laurens van der Maaten, 2008 ( Maastricht University). The algorithm is described in:
//L.J.P. van der Maaten and G.E. Hinton. Visualizing High-Dimensional Data Using t-t_Sne. Journal of Machine Learning Research 9(Nov):2579-2605, 2008
//http://homepage.tudelft.nl/19j49/t-Sne.html

#include "t_Sne.h"

using namespace arma;

t_Sne::t_Sne(){
    kk=30;
    tol=1e-4;
    max_iter = 1500;
    convergence_threshold = 0.00001;
}

bool t_Sne::setDistanceMatrix(arma::mat D){
    if (D.is_square()==false)
        return false;
    if (prod(prod(D!=D.t())))
        return false;
    D=D/max(max(D));
    D=pow(D,2);
    return d2p(D);
}

bool t_Sne::d2p(arma::mat Dist){
    
    uword N=Dist.n_cols;
    P.resize(N,N);
    colvec beta=ones(N,1);
    double logK=log(kk);
    for (uword i=0;i<N;i++){
        //cout<<"t_Sne::d2p"<<i<<endl;
        double betamin=-1*datum::inf;
        double betamax=datum::inf;
        double H;
        rowvec thisP;
        Hbeta(H,thisP,Dist.row(i),i,beta(i));
        double Hdiff=H-logK;
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
            Hdiff=H-logK;
            //cout<<"tries:"<<tries<<" Hdiff: "<<Hdiff<<" tol: "<<tol<<endl;
            //cout<<"test:"<<(fabs(Hdiff)>tol)<<endl;
            tries++;
        }
        P.row(i)=thisP;
    }
    cout<<"Mean value of sigma: "<<mean(sqrt(1/beta))<<endl;
    cout<<"Minimum value of sigma: "<<min(sqrt(1/beta))<<endl;
    cout<<"Maximum value of sigma: "<<max(sqrt(1/beta))<<endl;
    return true;
}

void t_Sne::Hbeta(double &H,rowvec &thisP,rowvec Dist,int ind,double beta){
    thisP.resize(Dist.size());
    thisP=exp(-Dist*beta);
    thisP(ind)=0;
    double sumP=sum(thisP);
    H=log(sumP)+beta*(sum(Dist%thisP))/sumP;
    thisP=thisP/sumP;
    //cout<<"thisP:"<<thisP<<endl;
    //cout<<"H:"<<H<<endl;
    return;
    
}

bool t_Sne::setFeatureMatrix(arma::mat X){//TODO Verify if it's necessay an ACP or not
    //cout<<"X:"<<X<<endl;
    X=X-min(min(X));
    X=X/max(max(X));
    mat coeff;
    mat M;
    princomp(coeff, M, X);
    //cout<<"pca:"<<M<<endl;
    colvec sum_X=sum(pow(M,2),1);
    mat D=sum_X*ones(1,sum_X.n_rows)+ones(sum_X.n_rows,1)*trans(sum_X)-2*(M*trans(M));
    return d2p(D);
}
arma::mat t_Sne::compute(int ndim,arma::mat yInit){
    bool initFlag=true;
    if (yInit.n_elem==0||yInit.n_cols!=ndim)
        initFlag=false;
    int n=this->P.n_rows;
    double momentum = 0.5;          // initial momentum
    double final_momentum = 0.8;    // value to which momentum is changed
    uword mom_switch_iter = 250;    // iteration at which momentum is changed
    uword stop_lying_iter = 100;    // maximum number of iterations
    uword epsilon = 500;            // initial learning rate
    double min_gain = .01;          // minimum gain for delta-bar-delta
    int convergence_iter = 2;       // must converge twice in a row
    int convergence_count = 0;
    
    for (uword i=0;i<n;i++)
        P(i,i)=0;
    P=0.5*(P+trans(P));
    P=P+datum::eps;
    P=P/sum(sum(P));
    
    //cout<<"P after:"<<endl<<P<<" sum: "<<sum(sum(P))<<endl;
    double _const=sum(sum(P%log(P)));
    if (!initFlag)
        P=P*4;
    // Initialize the solution
 
    mat ydata(n,ndim);
    if (initFlag){
        ydata=yInit;
    }
    else
        ydata=0.0001*randn(n,ndim);
    
    mat y_incs  = zeros(ydata.n_rows,ydata.n_cols);
    mat gains = ones(ydata.n_rows,ydata.n_cols);
    double costMin=datum::inf;
    mat yfinal(n,ndim);
    for (int iter=0;iter<max_iter;iter++){
        //cout<<"ydata: "<<ydata<<endl;
        colvec sum_ydata=sum(pow(ydata,2),1);
        //cout<<"sumy"<<sum_ydata<<endl;
        mat num=ones(n,n)/(ones(n,n)+sum_ydata*ones(1,n)+ones(n,1)*trans(sum_ydata)-2*(ydata*trans(ydata)));
        num.diag()=zeros(n,1);
        //cout<<"num : "<<num<<endl;
        mat Q=num/sum(sum(num))+datum::eps;
        mat L=(P-Q)%num;
        mat y_grads=4*(diagmat(sum(L))-L)*ydata;
        //cout<<"y_grads"<<y_grads<<endl;
        gains=(gains+0.2)%((y_grads%y_incs)<=0)+(gains*0.8)%((y_grads%y_incs)>0);
        gains=(gains>=min_gain)%gains+(gains<min_gain)*min_gain;
        //cout<<"gains"<<gains<<endl;
        y_incs=momentum*y_incs-epsilon*(gains%y_grads);
        //cout<<"y_incs"<<y_incs<<endl;
        ydata=ydata+y_incs;
        ydata=ydata-ones(n,1)*mean(ydata,0);
        //cout<<"ydata:"<<endl<<ydata<<endl;
        if (iter==mom_switch_iter){
            momentum = final_momentum;
        }
        if ((iter == stop_lying_iter)&&(!initFlag) )//&& ~initial_solution
            P = P / 4;
        float cost=_const-sum(sum(P%log(Q)));
        if (cost<costMin){
            float dcost = (costMin - cost)/costMin;
            costMin=cost;
            yfinal=ydata;

            //convergence ?
            if (dcost < convergence_threshold) {
                //how many times did we get a convergence
                //in a row
                convergence_count++;
                if (convergence_count >= convergence_iter) {
                    //we had 'convergence_iter' convergences in a row
                    //--> we can consider it's not happenstance
                    break;
                }
            } else {
                convergence_count = 0;
            }
        } else {
            convergence_count = 0;
        }
        //if (iter%10==0)
        //    cout<<"Iteration "<<iter<<": error is "<<cost<<endl;
    }
    ydata=ydata-ones(n,1)*mean(ydata);
    ydata=ydata/(ones(n,1)*stddev(ydata));
   // yfinal=yfinal-ones(n,1)*mean(yfinal);
    //yfinal=yfinal/(ones(n,1)*stddev(yfinal));
    return yfinal;
}

