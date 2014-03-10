/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#ifndef COLOROPTCOSTFUNCTION_HH
#define COLOROPTCOSTFUNCTION_HH

#include "dataset.hh"
#include "costfunction.hh"
#include "dynamicdouble.hh"

#include <iostream>


class ColorOptCostFunction : public CostFunction
{
private:

  CostFunction &dimRedMethod;
  DynamicDouble betaL;
  DynamicDouble betaC;
  DynamicDouble betaG;

  double Xn,Yn,Zn; //white point

  double xyz2rgb[3][3];
  double rgb2xyz[3][3];

  double Lmin;
  double Lmax;
  double Cmin;

  int ppow;


  /* Temporary values used to calculate the derivative */
  // DataMatrix dDval;
  //  vector<DataMatrix> ddist;

  std::ostream& feedback;



  void  Lab2XYZ(double L, double a, double b, double &X, double &Y, double &Z);
  void  XYZ2Lab(double X, double Y, double Z, double &L, double &a, double &b);

  void lin_sRGB2XYZ(double lR, double lG, double lB, double &X, double &Y, double &Z);
  void XYZ2lin_sRGB(double X, double Y, double Z, double &lR, double &lG, double &lB);

  void lin_sRGB2sRGB(double lR, double lG, double lB, double &sR, double &sG, double &sB);
  void sRGB2lin_sRGB(double sR, double sG, double sB, double &lR, double &lG, double &lB);


  double lin_sRGB2sRGB_trunc(double lR, double lG, double lB, double &sR, double &sG, double &sB);

  void fastLab2XYZ(double L, double a, double b,
                   double &X, double &Y, double &Z);

  double colorGamutPenalty(double L, double a, double b);

  void colorGamutGradient(double L, double a, double b,
                          double &dL, double &da, double &db);

  double colorCost(const DataMatrix& projData);

public:
  ColorOptCostFunction(CostFunction &dimRedMethod,
                       DynamicDouble betaL,
                       DynamicDouble betaC,
                       DynamicDouble betaG,
                       double Lmin,
                       double Lmax,
                       double Cmin,
                       std::ostream& feedback);

  void convert(const DataMatrix &inCol, DataMatrix & outCol);

  void reverseConvert(const DataMatrix &inCol, DataMatrix & outCol);


  // convert with gamut mapping, returns the average amount of error if colors were adjusted.

  double convertToGamut(const DataMatrix &inCol, DataMatrix & outCol);

  void violations(const DataMatrix & projData, double &projCost,
                  double & colCost,
                  long & numLminViol, long & numLmaxViol, long & numCminViol,
                  long & numGamutViol);

  /* See CostFunction for the rest. */

  double getGradient(const DataMatrix& projData, DataMatrix& gradient);

  double evaluate(const DataMatrix& data);


  void reportParameters(std::string& str) { str = ""; }
  void updateDynamicParameters(size_t currentRound, size_t totalRounds,
                               const DataMatrix& projData);

  void updateDataRepresentation(const DataMatrix& newData);

};

#endif
