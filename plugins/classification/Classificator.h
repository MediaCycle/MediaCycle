/* 
 * File:   Classificator.h
 * Author: jerome
 *
 * @date February 14, 2011
 * @copyright (c) 2011 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef _CLASSIFICATOR_H
#define	_CLASSIFICATOR_H

// Torch3
#include <MatDataSet.h>
#include <TwoClassFormat.h>
#include <ClassMeasurer.h>
#include <MSEMeasurer.h>
#include <QCTrainer.h>
#include <CmdLine.h>
#include <Random.h>
#include <SVMRegression.h>
#include <SVMClassification.h>
#include <KFold.h>
#include <DiskXFile.h>
#include <ClassFormatDataSet.h>
#include <MeanVarNorm.h>
#include <string>

#include <ClassNLLCriterion.h>
#include <MSECriterion.h>
#include <OneHotClassFormat.h>
#include <ClassMeasurer.h>
#include <MSEMeasurer.h>

#include <StochasticGradient.h>

#include <ConnectedMachine.h>
#include <Linear.h>
#include <Tanh.h>
#include <Sigmoid.h>
#include <LogSoftMax.h>
#include <Random.h>

#include <EMTrainer.h>
#include <DiagonalGMM.h>
#include <KMeans.h>
#include <DiskMatDataSet.h>
#include <NLLMeasurer.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <log_add.h>

// Local classes
//#include "KFold_CompToux.h"
#include "KFoldSaveOutputs.h"


enum CrossValidationType {
    KFOLD, // +1 and -1
    RANDOMSETS // gaussian kernel
};

enum ClassifierType{
    ALL,
    SupportVector,
    MultiLayerPerceptron,
    GaussianMixture
};

class Classificator {
public:
    Classificator();
    Classificator(CrossValidationType _CTV, int train_number, std::string _saveName, int _class_against_the_others=-1, int _the_seed=-1);
    Classificator(const Classificator& orig);
    int LoadData(std::string filename, int n_inputs, int n_outputs=-1, bool one_file_one_sequence=false, int max_load=-1, bool binary=false);
    virtual ~Classificator();

    int test_svm(int degree=-1, real param1=10, real param2=1, real accuracy=0.01, real cache_size=50, real error_margin=100, int iter_shrink=100);
    int test_mlp(int n_hidden, int max_iter=100, real learning_rate=100, real accuracy=0.00001, real weight_decay=0, real decay=0);
    int test_mlp(int n_hidden_to_test, int *hidden_numbers, int max_iter=100, real learning_rate=100, real accuracy=0.00001, real weight_decay=0, real decay=0);
    int test_gmm(int n_gaussians, real prior=0.001, real accuracy=0.0001, int max_iter_kmeans=150, real threshold=0.001, int max_iter_gmm=150);
    int test_gmm(int n_gaussians_to_test, int *gaussian_numbers, real prior=0.001, real accuracy=0.0001, int max_iter_kmeans=150, real threshold=0.001, int max_iter_gmm=150);
    int use_only_first_features(int n);

private:
    CrossValidationType CTV;
    int nFolds;
    int training_proportion; // percentage of samples for training (the rest is used for testing)
    int class_against_the_others;
    ClassifierType CT;
    Torch::DiskXFile* saveFile;
    Torch::DiskXFile* saveClassifierOutputs;
    int nClasses;
    Torch::MatDataSet* mat_data;
    Torch::Allocator* allocator;
    int nFrames;
    int nFeatures;
    bool one_file_is_one_sequence;
    time_t rawtime;
    std::string dataName;

    int _test_mlp(Torch::DataSet* data, Torch::OneHotClassFormat* class_format, int n_hidden, int max_iter=100, real learning_rate=100, real accuracy=0.00001, real weight_decay=0, real decay=0);
    int _make_subsets(int **** training_subsets, int *** n_training_subsets, int *** testing_subsets, int ** n_testing_subsets);
    int _gaussian_kfold(int n_gaussians, int*** training_subsets, int** n_training_subsets, int ** testing_subsets, int* n_testing_subsets, real prior=0.001, real accuracy=0.0001, int max_iter_kmeans=150, real threshold=0.001, int max_iter_gmm=150);
    int _gaussian_train_and_test(int n_gaussians, int** training_subsets, int* n_training_subsets, int* testing_subsets, int n_testing_subset, real prior=0.001, real accuracy=0.0001, int max_iter_kmeans=150, real threshold=0.001, int max_iter_gmm=150);
    int _singleClass_GMM_train_and_test(int n_gaussians, int* training_subsets, int n_training_subsets, int* testing_subsets, int n_testing_subset, double *computedLikelihood, real prior=0.001, real accuracy=0.0001, int max_iter_kmeans=150, real threshold=0.001, int max_iter_gmm=150, int classe=0);
    int _printConfusionMat(int** confusion, int n_classes);
    int _computeConfusionMat(int* targets, int* computed, int n_classes, int n_frames, bool print);

};

void initializeThreshold(Torch::DataSet* data, real* thresh, real threshold, bool one_file_is_one_sequence);
int compute_std(Torch::DataSet* data, real* standard_dev);

#endif	/* _CLASSIFICATOR_H */

