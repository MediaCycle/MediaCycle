/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "coloroptcostfunction.hh"
#include <math.h>

ColorOptCostFunction::ColorOptCostFunction(CostFunction &dimRedMethod,
    DynamicDouble betaL,
    DynamicDouble betaC,
    DynamicDouble betaG,
    double Lmin,
    double Lmax,
    double Cmin,
    std::ostream& feedback)
    :dimRedMethod(dimRedMethod),betaL(betaL),betaC(betaC),betaG(betaG),
    Lmin(Lmin),Lmax(Lmax),Cmin(Cmin),ppow(4),feedback(feedback)
{
  /* Using sRGB (D65 white point) */
  double xn=0.312713, yn=0.329016;

  Xn=xn/yn;
  Yn=1.0;
  Zn=(1.0-xn+yn)/yn;

  xyz2rgb[0][0]=3.2410;
  xyz2rgb[0][1]=-1.5374;
  xyz2rgb[0][2]=-0.4986;
  xyz2rgb[1][0]=-0.9692;
  xyz2rgb[1][1]=1.8760;
  xyz2rgb[1][2]=0.0416;
  xyz2rgb[2][0]=0.0556;
  xyz2rgb[2][1]=-0.204;
  xyz2rgb[2][2]=1.057;


  rgb2xyz[0][0]=0.4124;
  rgb2xyz[0][1]=0.3576;
  rgb2xyz[0][2]=0.1805;
  rgb2xyz[1][0]=0.2126;
  rgb2xyz[1][1]=0.7152;
  rgb2xyz[1][2]=0.0722;
  rgb2xyz[2][0]=0.0193;
  rgb2xyz[2][1]=0.1192;
  rgb2xyz[2][2]=0.9505;

}

void
ColorOptCostFunction::XYZ2lin_sRGB(double X, double Y, double Z,
                                   double &lR, double &lG, double &lB)
{
  lR=xyz2rgb[0][0]*X+xyz2rgb[0][1]*Y+xyz2rgb[0][2]*Z;
  lG=xyz2rgb[1][0]*X+xyz2rgb[1][1]*Y+xyz2rgb[1][2]*Z;
  lB=xyz2rgb[2][0]*X+xyz2rgb[2][1]*Y+xyz2rgb[2][2]*Z;

}

void
ColorOptCostFunction::lin_sRGB2XYZ(double lR, double lG, double lB,
                                   double &X, double &Y, double &Z)
{
  X=rgb2xyz[0][0]*lR+rgb2xyz[0][1]*lG+rgb2xyz[0][2]*lB;
  Y=rgb2xyz[1][0]*lR+rgb2xyz[1][1]*lG+rgb2xyz[1][2]*lB;
  Z=rgb2xyz[2][0]*lR+rgb2xyz[2][1]*lG+rgb2xyz[2][2]*lB;

}

void ColorOptCostFunction::lin_sRGB2sRGB(double lR, double lG, double lB,
    double &R, double &G, double &B)
{

  double a=0.055;
  if (lR<=0.00304)
    R=12.92*lR;
  else
    R=(1+a)*pow(lR,(1.0/2.4))-a;

  if (lG<=0.00304)
    G=12.92*lG;
  else
    G=(1+a)*pow(lG,(1.0/2.4))-a;

  if (lB<=0.00304)
    B=12.92*lB;
  else
    B=(1+a)*pow(lB,(1.0/2.4))-a;
}


void ColorOptCostFunction::sRGB2lin_sRGB(double R, double G, double B,
    double &lR, double &lG, double &lB)
{
  double a=0.055;

  if (R>0.04045)
    lR=pow((R+a)/(1+a),2.4);
  else
    lR=R/12.92;

  if (G>0.04045)
    lG=pow((G+a)/(1+a),2.4);
  else
    lG=G/12.92;

  if (B>0.04045)
    lB=pow((B+a)/(1+a),2.4);
  else
    lB=B/12.92;
}




double
ColorOptCostFunction::lin_sRGB2sRGB_trunc(double lR, double lG, double lB,
    double &R, double &G, double &B)
{
  double trunc=0.0;
  int count=0;

  lin_sRGB2sRGB(lR,lG,lB,R,G,B);

  if (R>1.0)
  {
    trunc+=R-1.0;
    R=1.0;
    ++count;
  }
  else if (R<0.0)
  {
    trunc-=R;
    R=0.0;
    ++count;
  }


  if (G>1.0)
  {
    trunc+=G-1.0;
    ++count;
    G=1.0;
  }
  else if (G<0.0)
  {
    trunc-=G;
    G=0.0;
    ++count;
  }


  if (B>1.0)
  {
    trunc+=B-1.0;
    ++count;
    B=1.0;
  }
  else if (B<0.0)
  {
    trunc-=B;
    ++count;
    B=0.0;
  }
  if (count==0)
    return 0.0;
  else
    return trunc/(double)count;
}


void
ColorOptCostFunction::XYZ2Lab(double X, double Y, double Z,
                              double &L, double &a, double &b)
{
  double delta=6.0/29.0;
  double d=1.0/(3*delta*delta);
  double e=pow(delta,3.0);

  if ((Y/Yn)>e)
  {
    /* L=116*(Y/Yn)^(1/3) */
    L=116*pow((Y/Yn),1.0/3.0)-16;

    if ((X/Xn)>e)
    {
      a=500*(pow(X/Xn,1.0/3.0)-pow(Y/Yn,1.0/3.0));
    }
    else
    {
      a=500*(d*X/Xn+16.0/116.0-pow(Y/Yn,1.0/3.0));
    }

    if ((Z/Zn)>e)
    {
      b=200*(pow(Y/Yn,1.0/3.0)-pow(Z/Zn,1.0/3.0));
    }
    else
    {
      b=200*(pow(Y/Yn,1.0/3.0)-d*Z/Zn-16.0/116.0);
    }

  }
  else
  {
    L=903.3*(Y/Yn);

    if ((X/Xn)>e)
    {
      a=500*(pow(X/Xn,1.0/3.0)-d*Y/Yn-16.0/116.0);
    }
    else
    {
      a=500*d*(X/Xn-Y/Yn);
    }

    if ((Z/Zn)>e)
    {
      b=200*(d*Y/Yn+16.0/116.0-pow(Z/Zn,1.0/3.0));
    }
    else
    {
      b=200*d*(Y/Yn-Z/Zn);
    }

  }
}


void
ColorOptCostFunction::Lab2XYZ(double L, double a, double b,
                              double &X, double &Y, double &Z)

{
  double delta=6.0/29.0;

  double fy=(L+16)/116;
  double fx=fy+a/500.0;
  double fz=fy-b/200.0;

  if (fy>delta)
  {
    Y=Yn*pow(fy,3.0);
  }
  else
  {
    Y=(fy-16.0/116.0)*3.0*delta*delta*Yn;
  }

  if (fx>delta)
  {
    X=Xn*pow(fx,3.0);
  }
  else
  {
    X=(fx-16.0/116.0)*3.0*delta*delta*Xn;
  }

  if (fz>delta)
  {
    Z=Zn*pow(fz,3.0);
  }
  else
  {
    Z=(fz-16.0/116.0)*3.0*delta*delta*Zn;
  }
}

void
ColorOptCostFunction::fastLab2XYZ(double L, double a, double b,
                                  double &X, double &Y, double &Z)

{
  // this ignores the change in the function on small values.
  double fy=(L+16)/116;
  double fx=fy+a/500.0;
  double fz=fy-b/200.0;

  Y=Yn*pow(fy,3.0);
  X=Xn*pow(fx,3.0);
  Z=Zn*pow(fz,3.0);

}

double
ColorOptCostFunction::colorGamutPenalty(double L, double a, double b)
{
  //Gamut penalty of the form max(0,(rgb-1)^ppow)

  double X,Y,Z;
  double lR,lG,lB;
  double R,G,B;
  double penalty=0;

  fastLab2XYZ(L,a,b,X,Y,Z);
  XYZ2lin_sRGB(X,Y,Z,lR,lG,lB);
  lin_sRGB2sRGB(lR,lG,lB,R,B,G);
  //  std::cerr << "RGB:"<<R<<","<<G<<","<<B;
  if (R<0.0)
  {
    penalty+=pow(fabs(R),ppow);
  }
  if (R>1.0)
  {
    penalty+=pow(R-1.0,ppow);
  }
  if (G<0.0)
    penalty+=pow(fabs(G),ppow);
  if (G>1.0)
    penalty+=pow(G-1.0,ppow);
  if (B<0.0)
    penalty+=pow(fabs(R),ppow);
  if (B>1.0)
    penalty+=pow(R-1.0,ppow);
  //  std::cerr <<", p:"<< penalty<<std::endl;
  return penalty;
}

void
ColorOptCostFunction::colorGamutGradient(double L, double a, double b,
    double &dL, double &da, double &db)
{
  //Gamut penalty of the form max(0,(rgb-1)^ppow)
  double X,Y,Z;
  double lR,lG,lB;
  double R,G,B;

  dL=0;
  da=0;
  db=0;

  fastLab2XYZ(L,a,b,X,Y,Z);
  XYZ2lin_sRGB(X,Y,Z,lR,lG,lB);
  lin_sRGB2sRGB(lR,lG,lB,R,B,G);


  if (R>1.0)
  {
    double dr=((double)ppow)*pow(R-1.0,(double)(ppow-1))*
              (1.0/2.4)*(1.055*pow(lR,(1.0/2.4)-1.0));
    dL+=dr*(xyz2rgb[0][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/116.0+
            xyz2rgb[0][1]*Yn*3.0*((L+16.0)/116.0)*((L+16.0)/116.0)/116.0+
            xyz2rgb[0][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/116.0);

    da+=dr*(xyz2rgb[0][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/500.0);

    db+=dr*(xyz2rgb[0][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/(-200.0));

  }
  else if (R<0.0)
  {

    double dr=((double)ppow)*pow(R,(double)(ppow-1))*12.92;

    dL+=dr*(xyz2rgb[0][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/116.0+
            xyz2rgb[0][1]*Yn*3.0*((L+16.0)/116.0)*((L+16.0)/116.0)/116.0+
            xyz2rgb[0][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/116.0);

    da+=dr*(xyz2rgb[0][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/500.0);

    db+=dr*(xyz2rgb[0][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/(-200.0));
  }


  if (G>1.0)
  {
    double dg=((double)ppow)*pow(G-1.0,(double)(ppow-1))*
              (1.0/2.4)*(1.055*pow(lG,(1.0/2.4)-1.0));
    dL+=dg*(xyz2rgb[1][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/116.0+
            xyz2rgb[1][1]*Yn*3.0*((L+16.0)/116.0)*((L+16.0)/116.0)/116.0+
            xyz2rgb[1][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/116.0);

    da+=dg*(xyz2rgb[1][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/500.0);

    db+=dg*(xyz2rgb[1][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/(-200.0));

  }
  else if (G<0.0)
  {

    double dg=((double)ppow)*pow(G,(double)(ppow-1))*12.92;

    dL+=dg*(xyz2rgb[1][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/116.0+
            xyz2rgb[1][1]*Yn*3.0*((L+16.0)/116.0)*((L+16.0)/116.0)/116.0+
            xyz2rgb[1][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/116.0);

    da+=dg*(xyz2rgb[1][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/500.0);

    db+=dg*(xyz2rgb[1][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/(-200.0));
  }


  if (B>1.0)
  {
    double db=((double)ppow)*pow(B-1.0,(double)(ppow-1))*
              (1.0/2.4)*(1.055*pow(lB,(1.0/2.4)-1.0));
    dL+=db*(xyz2rgb[2][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/116.0+
            xyz2rgb[2][1]*Yn*3.0*((L+16.0)/116.0)*((L+16.0)/116.0)/116.0+
            xyz2rgb[2][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/116.0);

    da+=db*(xyz2rgb[2][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/500.0);

    db+=db*(xyz2rgb[2][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/(-200.0));

  }
  else if (B<0.0)
  {

    double db=((double)ppow)*pow(B,(double)(ppow-1))*12.92;

    dL+=db*(xyz2rgb[2][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/116.0+
            xyz2rgb[2][1]*Yn*3.0*((L+16.0)/116.0)*((L+16.0)/116.0)/116.0+
            xyz2rgb[2][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/116.0);

    da+=db*(xyz2rgb[2][0]*Xn*3.0*(a/500.0+(L+16.0)/116.0)*
            (a/500.0+(L+16.0)/116.0)/500.0);

    db+=db*(xyz2rgb[2][2]*Zn*3.0*((L+16.0)/116.0-b/200.0)*
            ((L+16.0)/116.0-b/200.0)/(-200.0));
  }
}




double
ColorOptCostFunction::getGradient(const DataMatrix& projData,
                                  DataMatrix& gradient)
{

  double sqsum=0.0;
  size_t n=projData.getRows();

  dimRedMethod.getGradient(projData,gradient);

  for (size_t i=0;i<n;++i)
  {
    double dL,da,db;
    colorGamutGradient(projData(i,0), projData(i,1), projData(i,2),
                       dL,da,db);
    gradient(i,0)+=betaG.value()*dL;
    gradient(i,1)+=betaG.value()*da;
    gradient(i,2)+=betaG.value()*db;

    if (projData(i,0)<Lmin)
      gradient(i,0)-=betaL.value()*(double)(ppow)*pow(Lmin-projData(i,0),ppow-1);
    else if (projData(i,0)>Lmax)
      gradient(i,0)+=betaL.value()*(double)(ppow)*pow(projData(i,0)-Lmax,ppow-1);

    double C=sqrt(projData(i,1)*projData(i,1)+projData(i,2)*projData(i,2));

    if (C<Cmin)
    {
      gradient(i,1)-=betaC.value()*(double)(ppow)*pow(Cmin-C,ppow-1)*projData(i,1)/C;
      gradient(i,2)-=betaC.value()*(double)(ppow)*pow(Cmin-C,ppow-1)*projData(i,2)/C;
    }

  }
  for (size_t i=0;i<n;++i)
  {
    sqsum+=gradient(i,0)*gradient(i,0);
    sqsum+=gradient(i,1)*gradient(i,1);
    sqsum+=gradient(i,2)*gradient(i,2);
  }
  return sqsum;
}

double
ColorOptCostFunction::colorCost(const DataMatrix& projData)
{
  double cost=0.0;

  for (size_t i=0;i<projData.getRows();++i)
  {
    cost+=betaG.value()*colorGamutPenalty(projData(i,0), projData(i,1), projData(i,2));
    if (projData(i,0)>Lmax)
    {
      cost+=betaL.value()*pow(projData(i,0)-Lmax,ppow);
    }
    else  if (projData(i,0)<Lmin)
    {
      cost+=betaL.value()*pow(Lmin-projData(i,0),ppow);
    }
    double C=sqrt(projData(i,1)*projData(i,1)+projData(i,2)*projData(i,2));
    if (C<Cmin)
      cost+=betaC.value()*pow(Cmin-C,ppow);
  }
  return cost;
}

double
ColorOptCostFunction::evaluate(const DataMatrix& projData)
{
  double cost=dimRedMethod.evaluate(projData);
  cost+=colorCost(projData);
  // std::cerr << "Cost:"<<cost<<std::endl;
  return cost;
}

void
ColorOptCostFunction::violations(const DataMatrix & projData,
                                 double &projCost,
                                 double & colCost,
                                 long & numLminViol,
                                 long & numLmaxViol,
                                 long & numCminViol,
                                 long & numGamutViol)
{
  projCost=dimRedMethod.evaluate(projData);
  colCost=colorCost(projData);

  numLminViol=0;
  numLmaxViol=0;
  numCminViol=0;
  numGamutViol=0;

  for (size_t i=0;i<projData.getRows();++i)
  {
    double p=colorGamutPenalty(projData(i,0), projData(i,1), projData(i,2));
    if (p>0.0)
    {
      ++numGamutViol;
    }
    if (projData(i,0)>Lmax)
      ++numLmaxViol;
    else  if (projData(i,0)<Lmin)
      ++numLminViol;

    double C=sqrt(projData(i,1)*projData(i,1)+projData(i,2)*projData(i,2));

    if (C<Cmin)
      ++numCminViol;
    std::cerr << i<< " p:"<<p<<", L:"<<projData(i,0)<<", C:"<<C<<std::endl;
  }


}

void
ColorOptCostFunction::updateDynamicParameters(size_t currentRound,
    size_t totalRounds,
    const DataMatrix& projData)
{

  dimRedMethod.updateDynamicParameters(currentRound, totalRounds,projData);
  betaL.update(currentRound, totalRounds);
  betaC.update(currentRound, totalRounds);
  betaG.update(currentRound, totalRounds);
  std::cerr << "betaL,C,G:"<<betaL.value()<<" "<<betaC.value()<<" "<<betaG.value()<<std::endl;
}

void
ColorOptCostFunction::updateDataRepresentation(const DataMatrix& newData)
{
  dimRedMethod.updateDataRepresentation(newData);
}

void
ColorOptCostFunction::convert(const DataMatrix &inCol, DataMatrix & outCol)
{
  double X,Y,Z,lR,lG,lB;

  for (size_t i=0;i<inCol.getRows();++i)
  {

    Lab2XYZ(inCol(i,0),inCol(i,1),inCol(i,2),X,Y,Z);
    XYZ2lin_sRGB(X,Y,Z,lR,lG,lB);
    lin_sRGB2sRGB(lR,lG,lB,outCol(i,0),outCol(i,1),outCol(i,2));

  }
}

void
ColorOptCostFunction::reverseConvert(const DataMatrix &inCol, DataMatrix & outCol)
{

  double X,Y,Z,lR,lG,lB;

  for (size_t i=0;i<inCol.getRows();++i)
  {
    sRGB2lin_sRGB(inCol(i,0),inCol(i,1),inCol(i,2),lR,lG,lB);
    lin_sRGB2XYZ(lR,lG,lB,X,Y,Z);
    XYZ2Lab(X,Y,Z,outCol(i,0),outCol(i,1),outCol(i,2));
  }
}


// convert with gamut mapping, returns true if colors were adjusted.

double
ColorOptCostFunction::convertToGamut(const DataMatrix &inCol, DataMatrix & outCol)
{
  double X,Y,Z,lR,lG,lB;
  double meanError=0.0;
  long count=0;
  for (size_t i=0;i<inCol.getRows();++i)
  {

    Lab2XYZ(inCol(i,0),inCol(i,1),inCol(i,2),X,Y,Z);
    XYZ2lin_sRGB(X,Y,Z,lR,lG,lB);
    double trunc=lin_sRGB2sRGB_trunc(lR,lG,lB,outCol(i,0),outCol(i,1),outCol(i,2));
    if (trunc!=0.0)
    {
      meanError+=trunc;
      ++count;
    }
  }
  if (count==0)
    return 0.0;
  else
    return meanError/(double)count;
}

