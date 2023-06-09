#ifndef DISTANCEMATRIXLOADER_HH
#define DISTANCEMATRIXLOADER_HH

#include <string>
#include <fstream>
#include "distancematrix.hh"

class DistanceMatrixLoader
{
private:
  std::ifstream dataFile;

public:
  void loadMatrix (std::string fileName, DistanceMatrix & target);
};

#endif
