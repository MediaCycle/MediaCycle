/* 
 * File:   Classificator.cpp
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

#include <string>

#include "Classificator.h"

using namespace std;
using namespace Torch;

Classificator::Classificator() {
}

Classificator::Classificator(CrossValidationType _CTV, int train_number, string _saveName, int _class_against_the_others, int _the_seed)
{
    CTV=_CTV;
    if(CTV==KFOLD)
    {
        nFolds=train_number;
    }
    if(CTV==RANDOMSETS)
    {
        training_proportion=train_number;
    }
    class_against_the_others=_class_against_the_others;

    time ( &rawtime );
    saveFile=new DiskXFile(_saveName.c_str(),"a");
    saveFile->printf("----- Classification results ----- %s\n", ctime(&rawtime));
    saveClassifierOutputs=NULL;
    
    if (_the_seed == -1){
        Random::seed();
    }
    else{
        Random::manualSeed((long) _the_seed);
    }
    mat_data=NULL;
    allocator=new Allocator;
}


Classificator::Classificator(const Classificator& orig) {
}

Classificator::~Classificator() {
    delete allocator;
    delete mat_data;
    delete saveFile;
    if(saveClassifierOutputs)
    {
        delete saveClassifierOutputs;
    }
}


int Classificator::LoadData(std::string filename, int n_inputs, int n_outputs, bool one_file_one_sequence, int max_load, bool binary)
{
    one_file_is_one_sequence=one_file_one_sequence;
    dataName=filename;
    float z;
        // Create the training dataset
    if(mat_data)
    {
        delete mat_data;
    }
    mat_data=new MatDataSet(filename.c_str(), n_inputs, n_outputs, one_file_one_sequence, max_load, binary);

    nFeatures=mat_data->n_inputs;
    if(one_file_is_one_sequence)
    {
        nFrames=mat_data->inputs_array[0]->n_frames;
    }
    else
    {
        nFrames=mat_data->n_examples;
    }
    
    nClasses = -1;
    if(one_file_is_one_sequence)
    {
        for (int t = 0; t < nFrames; t++) {
            z= mat_data->targets_array[0]->frame_size;
            z= mat_data->targets_array[0]->n_frames;
            z = mat_data->targets_array[0]->frames[t][0];
            
            //float z = mat_data->targets->frames[t][0];
            if (z > nClasses)
                nClasses = (int) z;
            /*message("Example %d, class value: %f",t,z);
            for(int i=0; i< nFeatures;i++)
            {
                message("Value %d: %f",i,mat_data->inputs_array[0]->frames[t][i]);
            }*/
        }
    }
    else
    {
        for (int t = 0; t < mat_data->n_examples; t++) {
            mat_data->setExample(t);
            z = mat_data->targets->frames[0][0];
            if (z > nClasses)
                nClasses = (int) z;
            /*message("Example %d, class value: %f",t,z);
            for(int i=0; i< nFeatures;i++)
            {
                message("Value %d: %f",i,mat_data->inputs_array[0]->frames[t][i]);
            }*/
        }
    }
    nClasses++;
    
    message("Database loaded. \n%d frames;\n%d features;\n %d classes detected\n",nFrames, nFeatures, nClasses);
    saveFile->printf("Loaded file: %s, nFrames: %d; nFeatures: %d, nClasses: %d", filename.c_str(), nFrames, nFeatures, nClasses);

    return 0;
}

int Classificator::use_only_first_features(int n)
{
    if(!mat_data)
    {
        return 1;
    }
    if(n>=nFeatures)
    {
        message("The number of features (%d) is already lower than %d", nFeatures, n);
        return 2;
    }

    int i,j;
    float ** feat=0;
    feat= new float* [nFrames];
    for(i=0;i<nFrames;i++)
    {
        feat[i]=new float[n];
        for(j=0;j<n;j++)
        {
            feat[i][j]=mat_data->inputs_array[0]->frames[i][j];
        }
        delete[] mat_data->inputs_array[0]->frames[i];
    }
    delete[] mat_data->inputs_array[0]->frames;
    mat_data->inputs_array[0]->frames=feat;
    mat_data->inputs_array[0]->frame_size=n;
    nFeatures=n;
    saveFile->printf("Using opnly the first %d features", nFeatures);

    return 0;
}


int Classificator::test_svm(int degree, float param1, float param2, float accuracy, float cache_size, float error_margin, int iter_shrink)
{
    // 1) building the data set
    DataSet *data = NULL;
    MeanVarNorm *mv_norm = NULL;
    Sequence *class_labels = NULL;
    if(class_against_the_others<0&&nClasses>2)  // not possible for SVM, we have more than 2 classes -> we take class 0 against the others
    {
        class_against_the_others=0;
        message("SVM cannot deal with more than 2 classes.\n");
    }
    if (class_against_the_others >= 0) {
        message("Training/testing class %d against the others (%d classes detected)", class_against_the_others, nClasses);

        class_labels = new(allocator) Sequence(nClasses, 1);
        for (int i = 0; i < nClasses; i++) {
            if (i == class_against_the_others)
                class_labels->frames[i][0] = 1;
            else
                class_labels->frames[i][0] = -1;
        }
    } else {
        class_labels = new(allocator) Sequence(2, 1);
        class_labels->frames[0][0] = -1;
        class_labels->frames[1][0] = 1;
    }
    data = new(allocator) ClassFormatDataSet(mat_data, class_labels);

    //=================== Create the SVM... =========================
    SVM *svm = NULL;
    Kernel *kernel = NULL;

    if (degree > 0)
        kernel = new(allocator) PolynomialKernel(degree, param1, param2);
    else
        kernel = new(allocator) GaussianKernel(1. / (param1 * param1));

    svm = new(allocator) SVMClassification(kernel);


    svm->setROption("C", error_margin);
    svm->setROption("cache size", cache_size);


    // The list of measurers...
    MeasurerList measurers;

    TwoClassFormat *class_format = new(allocator) TwoClassFormat(data);
    saveFile->printf("\n===== SVM =====\n");
    ClassMeasurer *class_meas = new(allocator) ClassMeasurer(svm->outputs, data, class_format, saveFile, true); // on lui fait écrire la matrice de confusion pour chaque fold de test
    measurers.addNode(class_meas);
    class_meas->printConfusionMatrix();

    //=================== The Trainer ===============================

    QCTrainer trainer(svm);

    //=================== Let's go... ===============================

    // KFold

    KFold k(&trainer, nFolds);
    k.crossValidate(data, NULL, &measurers);

    return 0;
}


int Classificator::test_mlp(int n_hidden, int max_iter, real learning_rate, real accuracy, real weight_decay, real decay)
{
    // Create the training dataset (normalize inputs)
    DataSet *data = NULL;
    MeanVarNorm *mv_norm = NULL;
    // Sequence *class_labels = NULL;
    mv_norm = new(allocator) MeanVarNorm(mat_data);
    mat_data->preProcess(mv_norm);
    data = new(allocator) ClassFormatDataSet(mat_data, nClasses);


    // The class format
    OneHotClassFormat *class_format = NULL;
    class_format = new(allocator) OneHotClassFormat(nClasses);  // we could have only one output if only 2 classes


    return _test_mlp(data, class_format, n_hidden, max_iter, learning_rate, accuracy, weight_decay, decay);
}

int Classificator::test_mlp(int n_hidden_to_test, int* hidden_numbers, int max_iter, float learning_rate, float accuracy, float weight_decay, float decay)
{
    // Create the training dataset (normalize inputs)
    DataSet *data = NULL;
    MeanVarNorm *mv_norm = NULL;
    // Sequence *class_labels = NULL;
    mv_norm = new(allocator) MeanVarNorm(mat_data);
    mat_data->preProcess(mv_norm);
    data = new(allocator) ClassFormatDataSet(mat_data, nClasses);


    // The class format
    OneHotClassFormat *class_format = NULL;
    class_format = new(allocator) OneHotClassFormat(nClasses);  // we could have only one output if only 2 classes


   for(int i=0; i<n_hidden_to_test; i++) {
       _test_mlp(data, class_format, hidden_numbers[i], max_iter, learning_rate, accuracy, weight_decay, decay);
   }
    return 0;
}


int Classificator::_test_mlp(DataSet* data, OneHotClassFormat* class_format, int n_hidden, int max_iter, real learning_rate, real accuracy, real weight_decay, real decay)
{
    //=================== Create the MLP... =========================
    ConnectedMachine mlp;
    if (n_hidden > 0) {
        // ce qui est en commentaire sert à faire une reseau de neurones de 2 couches cachées
        Linear *c1 = new(allocator) Linear(nFeatures, n_hidden);
        c1->setROption("weight decay", weight_decay);
        Sigmoid *c2 = new(allocator) Sigmoid(n_hidden);
        Linear *c3 = new(allocator) Linear(n_hidden, nClasses);
        // Linear *c3 = new(allocator) Linear(n_hu, n_hu);
        c3->setROption("weight decay", weight_decay);
        mlp.addFCL(c1);
        mlp.addFCL(c2);
        mlp.addFCL(c3);
        //Sigmoid *c4=new(allocator) Sigmoid(nClasses);
        LogSoftMax *c4 = new(allocator) LogSoftMax(nClasses);

        mlp.addFCL(c4);

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // Linear *c5 = new(allocator) Linear(n_hu, n_outputs);
        //c1->setROption("weight decay", weight_decay);
        //LogSoftMax *c6 = new(allocator) LogSoftMax(n_outputs);
        //mlp.addFCL(c5);
        //mlp.addFCL(c6);
    } else {
        Linear *c1 = new(allocator) Linear(nFeatures, nClasses);
        c1->setROption("weight decay", weight_decay);
        mlp.addFCL(c1);
        LogSoftMax *c2 = new(allocator) LogSoftMax(nClasses);
        mlp.addFCL(c2);
    }

    // Initialize the MLP
    mlp.build();
    mlp.setPartialBackprop();


    // The list of measurers...
    MeasurerList measurers;
    // Measurers on the training dataset
    saveFile->printf("\n==== MLP with %d hidden units =====\n",n_hidden);
    message("==== MLP with %d hidden units =====",n_hidden);
    ClassMeasurer *class_meas = new(allocator) ClassMeasurer(mlp.outputs, data, class_format, saveFile, true); // ressort les matrices de confusion
    measurers.addNode(class_meas);

    //=================== The Trainer ===============================

    Criterion *criterion = NULL;
    criterion = new(allocator) ClassNLLCriterion(class_format); // obligatoire car c'est le seul compatible avec la couche LogSoftMax de sortie
    //criterion=new(allocator) MSECriterion(nFeatures);

    // The Gradient Machine Trainer
    StochasticGradient trainer(&mlp, criterion);
    trainer.setIOption("max iter", max_iter);
    trainer.setROption("end accuracy", accuracy);
    trainer.setROption("learning rate", learning_rate);
    trainer.setROption("learning rate decay", decay);


    //=================== Let's go... ===============================


    char b[200];
    sprintf(b, "/media/Data/CompToux/cough_classification/MLP_outputs/%sMLP%dNeurons%s.txt", dataName.c_str(), n_hidden, ctime(&rawtime));
    DiskXFile *mlp_outputs=new(allocator) DiskXFile(b,"a");
    KFoldSaveOutputs k(&trainer, nFolds);
    k.crossValidateJay(data, mlp_outputs, nClasses, NULL, &measurers);


    return 0;
}

int Classificator::test_gmm(int n_gaussians, real prior, real accuracy, int max_iter_kmeans, real threshold, int max_iter_gmm)
{
    if(CTV==KFOLD)
    {
        //1) we create the folds: one training set per class and per fold, one testing set per fold
        int *** training_subsets;   // the training subsets: one vector of indexes per fold and per class
        int ** n_training_subsets; // size of each training subset: one number per fold and per class
        int ** testing_subsets;     // the testing subsets: one vector per fold
        int * n_testing_subsets;    // size of each training subset: one number per fold
        _make_subsets(&training_subsets, &n_training_subsets, &testing_subsets, &n_testing_subsets);
        
        // 2) we train and test the GMMs
        _gaussian_kfold(n_gaussians, training_subsets, n_training_subsets, testing_subsets, n_testing_subsets, prior, accuracy, max_iter_kmeans, threshold, max_iter_gmm);

        //check number per testing and training test
        /*int i, j;
        for(i=0;i<nFolds;i++)
        {
            message("Subset %d; n tests: %d", i+1, n_testing_subsets[i]);
            for(j=0;j<nClasses;j++)
            {
                message("\tClass %d: n_train: %d", j, n_training_subsets[i][j]);
            }
        }*/
 
  
    }
    return 0;
}

int Classificator::test_gmm(int n_gaussians_to_test, int* gaussian_numbers, float prior, float accuracy, int max_iter_kmeans, float threshold, int max_iter_gmm)
{
    if(CTV==KFOLD)
    {
        //1) we create the folds: one training set per class and per fold, one testing set per fold
        int *** training_subsets;   // the training subsets: one vector of indexes per fold and per class
        int ** n_training_subsets; // size of each training subset: one number per fold and per class
        int ** testing_subsets;     // the testing subsets: one vector per fold
        int * n_testing_subsets;    // size of each training subset: one number per fold
        _make_subsets(&training_subsets, &n_training_subsets, &testing_subsets, &n_testing_subsets);
        //check number per testing and training test
        int i, j;
        for(i=0;i<nFolds;i++)
        {
            message("Subset %d; n tests: %d", i+1, n_testing_subsets[i]);
            for(j=0;j<nClasses;j++)
            {
                message("\tClass %d: n_train: %d", j, n_training_subsets[i][j]);
            }
        }

        // 2) we train and test the GMMs
        for(int i=0; i<n_gaussians_to_test;i++)
        {
        _gaussian_kfold(gaussian_numbers[i], training_subsets, n_training_subsets, testing_subsets, n_testing_subsets, prior, accuracy, max_iter_kmeans, threshold, max_iter_gmm);
        }

        


    }
    return 0;
}

int Classificator::_gaussian_kfold(int n_gaussians, int*** training_subsets, int** n_training_subsets, int** testing_subsets, int* n_testing_subsets, real prior, real accuracy, int max_iter_kmeans, real threshold, int max_iter_gmm)
{
    int i,j;
    saveFile->printf("\n==== GMM with %d gaussians, %d folds ====\n", n_gaussians, nFolds);
    char b[200];
    sprintf(b, "/media/Data/CompToux/cough_classification/ROC/%sGMM%dGauss%s.txt", dataName.c_str(), n_gaussians, ctime(&rawtime));
    if(saveClassifierOutputs)
    {
        delete saveClassifierOutputs;
    }
    saveClassifierOutputs=new DiskXFile(b,"a");
    
    saveClassifierOutputs->printf("----- Data: %s; GMM outputs; %d gaussians; %d features  ----- %s\n", dataName.c_str(), n_gaussians, nFeatures, ctime(&rawtime));

    for(i=0;i<nFolds;i++)
    {
        message("GMM with %d gaussians per class, %d features; Fold %d", n_gaussians, nFeatures, i+1);
        saveClassifierOutputs->printf("-Fold %d-\n", i);
        _gaussian_train_and_test(n_gaussians, training_subsets[i], n_training_subsets[i], testing_subsets[i], n_testing_subsets[i], prior, accuracy, max_iter_kmeans, threshold, max_iter_gmm);
    }
    return 0;
}

int Classificator::_singleClass_GMM_train_and_test(int n_gaussians, int* training_subsets, int n_training_subsets, int* testing_subsets, int n_testing_subset, double* computedLikelihood, float prior, float accuracy, int max_iter_kmeans, float threshold, int max_iter_gmm, int classe)
{

    DiskXFile kmeans_train_results("/media/Data/CompToux/cough_classification/KMeanstrainresults","a");
    DiskXFile gmm_train_results("/media/Data/CompToux/cough_classification/GMMtrainresults","a");
    //DiskXFile gmm_test_results("/media/Data/CompToux/cough_classification/GMMtestresults","a");
   
    kmeans_train_results.printf("----- KMEans Train Results ----- %s\n", ctime(&rawtime));
    gmm_train_results.printf("----- GMM Train Results ----- %s\n", ctime(&rawtime));



    Allocator *a=new Allocator;
    //1) Training
    mat_data->pushSubset(training_subsets,n_training_subsets);
    //=================== Create the GMM... =========================


                // create a KMeans object to initialize the GMM
                KMeans kmeans(mat_data->n_inputs, n_gaussians);
                kmeans.setROption("prior weights", prior);

                // the kmeans trainer
                EMTrainer kmeans_trainer(&kmeans);
                kmeans_trainer.setROption("end accuracy", accuracy);
                kmeans_trainer.setIOption("max iter", max_iter_kmeans);

                // the kmeans measurer
                MeasurerList kmeans_measurers;
                NLLMeasurer nll_kmeans_measurer(kmeans.log_probabilities, mat_data, &kmeans_train_results);
                kmeans_measurers.addNode(&nll_kmeans_measurer);

                // create the GMM
                DiagonalGMM gmm(mat_data->n_inputs, n_gaussians, &kmeans_trainer);

                // set the training options
                real* thresh = (real*) a->alloc(mat_data->n_inputs * sizeof (real));
                initializeThreshold(mat_data, thresh, threshold, one_file_is_one_sequence);
                gmm.setVarThreshold(thresh);
                gmm.setROption("prior weights", prior);
                gmm.setOOption("initial kmeans trainer measurers", &kmeans_measurers);


                //=================== Measurers and Trainer  ===============================

                // Measurers on the training dataset
                MeasurerList measurers;
                NLLMeasurer nll_meas(gmm.log_probabilities, mat_data, &gmm_train_results);
                measurers.addNode(&nll_meas);

                // The Gradient Machine Trainer
                EMTrainer trainer(&gmm);
                trainer.setIOption("max iter", max_iter_gmm);
                trainer.setROption("end accuracy", accuracy);

                //=================== Let's go... ===============================
                trainer.train(mat_data, &measurers);

    mat_data->popSubset();

    //2) Testing // There must be an easier method using simply forward...
    mat_data->pushSubset(testing_subsets,n_testing_subset);
    char c[200];
    sprintf(c, "/media/Data/CompToux/cough_classification/gauss_output/%sGMM%dGaussClass%d_%s.txt", dataName.c_str(), n_gaussians, classe, ctime(&rawtime));
    DiskXFile sorties_machine(c, "a");
    sorties_machine.printf("Poids des gaussiennes:\n");
    for (int p = 0; p < n_gaussians; p++) {
        sorties_machine.printf(" %g ", gmm.log_weights[p]); //on sauve les poids de pondération des gaussiennes
    }
    sorties_machine.printf("\nLog probas - TargetClass - Index:\n");
    for(int j=0;j<n_testing_subset;j++)
    {
        mat_data->setExample(j);
        gmm.forward(mat_data->inputs);
        computedLikelihood[j]=LOG_ZERO;
        for (int h = 0; h < n_gaussians; h++) {
                        sorties_machine.printf("%f ", gmm.log_probabilities_g->frames[0][h]); // pour chaque gaussienne et chaque frame on sauve le log de la vraissemblance (valeur qui peut être >1)
                        computedLikelihood[j]=logAdd(computedLikelihood[j],gmm.log_probabilities_g->frames[0][h]+gmm.log_weights[h]);
                    }
        sorties_machine.printf("%f %d\n",mat_data->targets->frames[0][0],testing_subsets[j]);
    }
    
    
    
                /*MeasurerList measurersT;
                NLLMeasurer nll_measT(gmm.log_probabilities, mat_data, &gmm_test_results);
                measurersT.addNode(&nll_measT);


                //=================== The Trainer ===============================

                // The Gradient Machine Trainer
                EMTrainer trainerT(&gmm);

                //=================== Let's go... ===============================

                trainerT.test(&measurersT);
                message("N_Frame de data: %d ", n_testing_subset);

                char c[100];
                sprintf(c, "/home/jerome/NetBeansProjects/cough_torch_classification/%dGauss.txt", n_gaussians);
                DiskXFile sorties_machine(c, "a");
                sorties_machine.printf("Poids des gaussiennes: ");
                for (int p = 0; p < n_gaussians; p++) {
                    sorties_machine.printf(" %g ", gmm.log_weights[p]); //on sauve les poids de pondération des gaussiennes
                }
                sorties_machine.printf("\nLog probas:");
                for (int p = 0; p < n_testing_subset; p++) {
                    real f,g;
                    f=gmm.outputs->frame_size;
                    g=gmm.outputs->n_frames;
                    computedLikelihood[p]=0;
                    //mat_data->setExample(p);
                    for (int h = 0; h < n_gaussians; h++) {
                        sorties_machine.printf("%f ", gmm.log_probabilities_g->frames[p][h]); // pour chaque gaussienne et chaque frame on sauve le log de la vraissemblance (valeur qui peut être >1)
                        computedLikelihood[p]+=exp(gmm.log_probabilities_g->frames[p][h]+gmm.log_weights[h]);
                    }
                    sorties_machine.printf("\n");
                }*/

    mat_data->popSubset();

    delete a;
    return 0;
}

int Classificator::_gaussian_train_and_test(int n_gaussians, int** training_subsets, int* n_training_subsets, int* testing_subsets, int n_testing_subset, real prior, real accuracy, int max_iter_kmeans, real threshold, int max_iter_gmm)
{
    Allocator* allo=new Allocator;
    int i,j;
    
    //for computing the class
    int* computedClass=(int*)allo->alloc(sizeof(int)*n_testing_subset);
    int* targetClass=(int*)allo->alloc(sizeof(int)*n_testing_subset);
    real* maxLikelihood=(real*)allo->alloc(sizeof(real)*n_testing_subset);
    double** currentLikelihood=(double**)allo->alloc(sizeof(double*)*nClasses);

    saveClassifierOutputs->printf("Priors: ");
    for(i=0;i<nClasses;i++)
    {
        saveClassifierOutputs->printf("%d ", n_training_subsets[i]);
    }
    saveClassifierOutputs->printf("\n %d ClassesLogLikelihoods - TargetClass - ComputedClass - Index\n", nClasses);
    
    for(i=0;i<nClasses;i++)
    {
        currentLikelihood[i]=(double*)allo->alloc(sizeof(double)*n_testing_subset);
        _singleClass_GMM_train_and_test(n_gaussians, training_subsets[i], n_training_subsets[i], testing_subsets, n_testing_subset, currentLikelihood[i], prior, accuracy, max_iter_kmeans, threshold, max_iter_gmm, i);
        if(i==0)
        {
            for(j=0;j<n_testing_subset;j++)
            {
                computedClass[j]=0;
                maxLikelihood[j]=currentLikelihood[i][j];
                mat_data->pushSubset(testing_subsets,n_testing_subset);
                mat_data->setExample(j);
                targetClass[j] = mat_data->targets->frames[0][0];
                mat_data->popSubset();
            }
        }
        else
        {
            for(j=0;j<n_testing_subset;j++)
            {
                if(currentLikelihood[i][j]>maxLikelihood[j])
                {
                computedClass[j]=i;
                maxLikelihood[j]=currentLikelihood[i][j];
                }
            }
        }
    }

    for(i=0;i<n_testing_subset;i++)
    {
        for(j=0;j<nClasses;j++)
        {
            saveClassifierOutputs->printf("%e ", currentLikelihood[j][i]);
        }
        saveClassifierOutputs->printf("%d %d %d\n", targetClass[i], computedClass[i], testing_subsets[i]);
    }

    //write results
    _computeConfusionMat(targetClass,computedClass,nClasses,n_testing_subset,true);

    delete allo;
    return 0;
}

/*int Classificator::_gaussian_train_and_test(int n_gaussians, int** training_subsets, int* n_training_subsets, int* testing_subsets, int n_testing_subset, real prior, real accuracy, int max_iter_kmeans, real threshold, int max_iter_gmm)
{
    Allocator* allo=new Allocator;
    int i,j;
    DiskXFile kmeans_train_results("KMeanstrainresults","a");
    DiskXFile gmm_train_results("GMMtrainresults","a");
    KMeans *kmeans=(KMeans*)allo->alloc(sizeof(KMeans)*nClasses);
    EMTrainer *kmeans_trainer=(EMTrainer*)allo->alloc(sizeof(EMTrainer)*nClasses);
    MeasurerList *kmeans_measurers=(MeasurerList*)allo->alloc(sizeof(MeasurerList)*nClasses);
    NLLMeasurer *nll_kmeans_measurers=(NLLMeasurer*)allo->alloc(sizeof(NLLMeasurer)*nClasses);
    DiagonalGMM *gmm=(DiagonalGMM*)allo->alloc(sizeof(DiagonalGMM)*nClasses);
    MeasurerList *measurers=(MeasurerList*)allo->alloc(sizeof(MeasurerList)*nClasses);
    NLLMeasurer *nll_meas=(NLLMeasurer*)allo->alloc(sizeof(NLLMeasurer)*nClasses);
    EMTrainer *trainer=(EMTrainer*)allo->alloc(sizeof(EMTrainer)*nClasses);



    real* thresh = (real*) allo->alloc(mat_data->n_inputs * sizeof (real));
    initializeThreshold(mat_data, thresh, threshold);

    //for computing the class
    int* computedClass=(int*)allo->alloc(sizeof(int)*n_testing_subset);
    int* targetClass=(int*)allo->alloc(sizeof(int)*n_testing_subset);
    real* maxLikelihood=(real*)allo->alloc(sizeof(real)*n_testing_subset);
    real current_prob;

    for(i=0;i<nClasses;i++)
    {
        mat_data->pushSubset(training_subsets[i],n_training_subsets[i]);
        //=================== Create the GMM... =========================
        // create a KMeans object to initialize the GMM
        kmeans[i]=KMeans(mat_data->n_inputs, n_gaussians);
        kmeans[i].setROption("prior weights", prior);

        // the kmeans trainer
        kmeans_trainer[i]=EMTrainer(&kmeans[i]);
        kmeans_trainer[i].setROption("end accuracy", accuracy);
        kmeans_trainer[i].setIOption("max iter", max_iter_kmeans);

        // the kmeans measurer
        kmeans_measurers[i]=MeasurerList();
        nll_kmeans_measurers[i]=NLLMeasurer(kmeans[i].log_probabilities, mat_data, &kmeans_train_results);
        kmeans_measurers[i].addNode(&nll_kmeans_measurers[i]);

        // create the GMM
        gmm[i]=DiagonalGMM(mat_data->n_inputs, n_gaussians, &kmeans_trainer[i]);

        // set the training options
        gmm[i].setVarThreshold(thresh);
        gmm[i].setROption("prior weights", prior);
        gmm[i].setOOption("initial kmeans trainer measurers", &kmeans_measurers[i]);


        //=================== Measurers and Trainer  ===============================

        // Measurers on the training dataset
        measurers[i]=MeasurerList();
        nll_meas[i]=NLLMeasurer(gmm[i].log_probabilities, mat_data, &gmm_train_results);
        measurers[i].addNode(&nll_meas[i]);

        // The Gradient Machine Trainer
        trainer[i]=EMTrainer(&gmm[i]);
        trainer[i].setIOption("max iter", max_iter_gmm);
        trainer[i].setROption("end accuracy", accuracy);

        //=================== Let's go... ===============================
        trainer[i].train(mat_data, &measurers[i]);

        mat_data->popSubset();

        //Now test and assign class
        mat_data->pushSubset(testing_subsets,n_testing_subset);
        gmm[i].forward(mat_data->inputs);
        if(i==0)
        {
            for(j=0;j<n_testing_subset;j++)
            {
                computedClass[j]=0;
                maxLikelihood[j]=gmm[i].log_probabilities->frames[j][0];
                mat_data->setExample(j);
                targetClass[j] = mat_data->targets->frames[0][0];
            }
        }
        else
        {
            for(j=0;j<n_testing_subset;j++)
            {
                current_prob=gmm[i].log_probabilities->frames[j][0];
                if(current_prob>maxLikelihood[j])
                {
                computedClass[j]=i;
                maxLikelihood[j]=current_prob;
                }
            }
        }

        mat_data->popSubset();
    }

    //write results
    _computeConfusionMat(targetClass,computedClass,nClasses,n_testing_subset,true);

    delete allo;
    return 0;
}*/

int Classificator::_printConfusionMat(int** confusion, int n_classes)
{
    int i,k, tot=0;
    float accuracy=0;
    saveFile->printf("# Confusion matrix [rows: observed, colums: desired]:\n");
    for (i=0; i<n_classes; i++)
    {
        //print("%d:\t",i);
        for(k=0;k<n_classes;k++)
        {
            saveFile->printf("%d ", confusion[i][k]);
            tot+=confusion[i][k];
            message("ComputedClass %d; TargetClass %d; N:%d", i, k, confusion[i][k]);
        }
        saveFile->printf("\n");
        accuracy+=confusion[i][i];
    }
    accuracy/=tot;
    message("Global accuracy: %f", 100*accuracy);
    saveFile->printf("Global accuracy: %f\n", 100*accuracy);
}

int Classificator::_computeConfusionMat(int* targets, int* computed, int n_classes, int n_frames, bool print)
{
    Allocator *a= new Allocator;
    int** confmat;
    int i, k, m;
    float accuracy=0;
    confmat=(int**)a->alloc(n_classes*sizeof(int*));
    for(i=0;i<n_classes;i++)
    {
        confmat[i]=(int*)a->alloc(n_classes*sizeof(int));
        for(k=0;k<n_classes;k++)
        {
            confmat[i][k]=0;

        }
    }
    for(m=0;m<n_frames;m++)
    {
            confmat[computed[m]][targets[m]]++;
    }
    if(print)
    {
        _printConfusionMat(confmat,n_classes);
    }

    delete a;
}


int Classificator::_make_subsets(int**** training_subsets, int*** n_training_subsets, int*** testing_subsets, int** n_testing_subsets)
{
        int i,j, classe;
        training_subsets[0]=(int***)allocator->alloc(sizeof(int**)*nFolds);//new int**[nFolds];
        n_training_subsets[0]= (int**)allocator->alloc(sizeof(int*)*nFolds);//new int*[nFolds];
        testing_subsets[0]=(int**)allocator->alloc(sizeof(int*)*nFolds);//new int*[nFolds];
        n_testing_subsets[0]=(int*)allocator->alloc(sizeof(int)*nFolds);//new int[nFolds];
        for(i=0;i<nFolds;i++)
        {
            training_subsets[0][i]=(int**)allocator->alloc(sizeof(int*)*nClasses);//new int*[nClasses];
            n_training_subsets[0][i]=(int*)allocator->alloc(sizeof(int)*nClasses);//new int[nClasses];
        }
        int* count_per_class=(int*)allocator->alloc((sizeof(int))*nClasses);

        int *mix_subset = (int *)Allocator::sysAlloc(sizeof(int)*nFrames);
          Random::getShuffledIndices(mix_subset, nFrames);
          int fucking_hack_because_round_sucks = nFrames/nFolds;
          int taille_subset = ( (((real)nFrames)/((real)nFolds)) - (real)fucking_hack_because_round_sucks >= 0.5 ? fucking_hack_because_round_sucks+1 : fucking_hack_because_round_sucks );
          for( i = 0; i < nFolds; i++)
          {
              for(j=0;j<nClasses;j++)
              {
                  n_training_subsets[0][i][j]=0;
              }
              if(i<nFolds-1)
              {
                  n_testing_subsets[0][i]=taille_subset;
              }
              else
              {
                  n_testing_subsets[0][i]=nFrames-(nFolds-1)*taille_subset;
              }
              testing_subsets[0][i]=(int*)allocator->alloc(sizeof(int)*(n_testing_subsets[0][i]));

            // count training sizes and assign testing subsets
            for(j = 0; j < i*taille_subset; j++)
            {
                if(one_file_is_one_sequence)
                {
                classe = (int) mat_data->targets_array[0]->frames[j][0];
                }
                else
                {
                mat_data->setExample(mix_subset[j]);
                classe = (int) mat_data->targets->frames[0][0];
                }
                n_training_subsets[0][i][classe]++;
            }
            for(j = i*taille_subset; j < i*taille_subset+(n_testing_subsets[0][i]); j++)
            {
              testing_subsets[0][i][j-i*taille_subset] = mix_subset[j];
            }
            for(j = (i+1)*taille_subset; j < nFrames; j++)
            {
                if(one_file_is_one_sequence)
                {
                classe = (int) mat_data->targets_array[0]->frames[j][0];
                }
                else
                {
                mat_data->setExample(mix_subset[j]);
                classe = (int) mat_data->targets->frames[0][0];
                }
                n_training_subsets[0][i][classe]++;
            }

            // now assign training subsets

              for(j=0;j<nClasses; j++)
              {
                  training_subsets[0][i][j]=(int*)allocator->alloc(sizeof(int)*(n_training_subsets[0][i][j]));
                  count_per_class[j]=0;
              }

              for(j = 0; j < i*taille_subset; j++)
                {
                    if(one_file_is_one_sequence)
                        {
                        classe = (int) mat_data->targets_array[0]->frames[j][0];
                        }
                        else
                        {
                        mat_data->setExample(mix_subset[j]);
                        classe = (int) mat_data->targets->frames[0][0];
                        }
                    training_subsets[0][i][classe][count_per_class[classe]++]=mix_subset[j];
                }
                for(j = (i+1)*taille_subset; j < nFrames; j++)
                {
                    if(one_file_is_one_sequence)
                        {
                        classe = (int) mat_data->targets_array[0]->frames[j][0];
                        }
                        else
                        {
                        mat_data->setExample(mix_subset[j]);
                        classe = (int) mat_data->targets->frames[0][0];
                        }
                    training_subsets[0][i][classe][count_per_class[classe]++]=mix_subset[j];
                }

              //check
              /*for(j=0;j<nClasses;j++)
              {
                  if(count_per_class[j]-n_training_subsets[0][i][j])
                  {
                      message("Problem: Fold %d; Class %d; count_per_class: %d; foreseen_count: %d", i, j, count_per_class[j], n_training_subsets[0][i][j]);
                  }
                  else
                  {
                      message("Ok");
                  }
              }*/

          }
          free(mix_subset);
}


void initializeThreshold(DataSet* data, real* thresh, real threshold, bool one_file_is_one_sequence) {
    if(!one_file_is_one_sequence)
    {
    MeanVarNorm norm(data);
    real* ptr = norm.inputs_stdv;
    real* p_var = thresh;
    for (int i = 0; i < data->n_inputs; i++)
        *p_var++ = *ptr * *ptr++ * threshold;
    }
    else
    {
        Allocator *a=new Allocator;
        real* ptr = (real*) a->alloc((sizeof(real))*data->n_inputs);
        //compute_std(data, ptr);
        real* p_var = thresh;
        for (int i = 0; i < data->n_inputs; i++)
            *p_var++=threshold;
        //*p_var++ = *ptr * *ptr++ * threshold;
        delete a;
    }
}

int compute_std(DataSet* data, real* ptr)
{
    int n_frames, n_features, i, j;
    n_features=data->n_inputs;
    n_frames=data->n_examples;
    Allocator *b=new Allocator;
    real *moyennes=(real*)b->alloc(sizeof(int)*n_features);
    for(j=0;j<n_features;j++)
        {
            moyennes[j]=0;
            ptr[j]=0;
        }

    for(i=0;i<n_frames;i++)
    {
        data->setExample(i);
        for(j=0;j<n_features;j++)
        {
            moyennes[j]+=data->inputs->frames[0][j];
            ptr[j]+=data->inputs->frames[0][j]*data->inputs->frames[0][j];
        }
    }
    for(j=0;j<data->inputs->frame_size;j++)
        {
            ptr[j]/=data->inputs->n_frames;
            ptr[j]-=moyennes[j]*moyennes[j];
            ptr[j]=sqrt(ptr[j]);
        }


    delete b;
    return 0;
}