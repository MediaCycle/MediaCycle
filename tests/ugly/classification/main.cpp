/* 
 * File:   main.cpp
 * Author: jerome
 *
 * @date February 15, 2011
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

#include <stdlib.h>
#include <Classificator.h>
#include <KFoldSaveOutputs.h>
#include <MatDataSet.h>
#include <CmdLine.h>
#include <string>

using namespace Torch;
using namespace std;


/*
 * 
 */
int main(int argc, char** argv) {
    
    CmdLine cmd;
    char *file_in, *file_out, *nUnits;
    CrossValidationType cvt;
    int nFolds;
    int max_load, n_inputs, n_outputs, binary, one_file_one_seq, class_against_the_others, random_seed, classific, training_proportion;
    ClassifierType CT;
    
    const char *help= "Program to train and test Classificators through Torch. Default, KFold with 10 folds";
    
    cmd.info(help);
    cmd.addText("\nArguments:");
    cmd.addICmdArg("Classificator",&classific, "The classificator: 0 for ALL, 1 for SVM, 2 for MLP, 3 for GMM");
    cmd.addSCmdArg("Input File", &file_in, "The input file (containing the labels at the end)");
    cmd.addSCmdArg("Output File", &file_out, "The output file");
  
    cmd.addICmdOption("-NFolds", &nFolds, 10, "The number of folds");
    cmd.addICmdOption("-ninputs", &n_inputs, -1, "The number of input features");
    cmd.addICmdOption("-noutputs", &n_outputs, 1, "The size of the output frames");
    cmd.addICmdOption("-load", &max_load, -1, "The maximum number of examples to load");
    cmd.addICmdOption("-class_against", &class_against_the_others, -1, "Label of the class to train against the others");
    cmd.addICmdOption("-one_file_one_seq", &one_file_one_seq, 0, "1 to have one file, one sequence");
    cmd.addICmdOption("-bin", &binary, 0, "1 to specifiy the input file is binary");
    cmd.addICmdOption("-random", &random_seed, -1, "Given random seed");
    cmd.addICmdOption("-randomset", &training_proportion, -1, "Avoid KFold and specify percentage of examples for training");
    cmd.addSCmdOption("-nUnits", &nUnits, "8", "The number of hidden neurons or gaussians. Several values can be given, separated by '_'");

    cmd.read(argc, argv);

    if(training_proportion>0)
    {
        cvt=RANDOMSETS;
        if(training_proportion<1||training_proportion>100)
        {
            message("ERROR - Training proportion (%) must be between 1 and 100");
            return 1;
        }
        nFolds=training_proportion;
    }
    else
    {
        cvt=KFOLD;
        if(nFolds<2)
        {
            message("ERROR - There must be at least 2 folds");
            return 2;
        }
    }

    string t=nUnits;
    int N=1, i=0, k=0, p, num;
    p=t.find_first_of("_",i);
    while(p!=t.npos)
    {
        //message("p: %d", p);
        N++;
        i=p+1;
        p=t.find_first_of("_",i);
    }
    int *Units=new int[N];
    i=0;
    p=t.find_first_of("_",i);
    while(p!=t.npos)
    {
        if(i)
        {
          num=atoi(t.substr(i,p-1).c_str());
        }
        else
        {
          num=atoi(t.substr(i,p).c_str());
        }
        
        if(num)
        {
            Units[k]=num;
            //message("Units number %d: %d", k, num);
            k++;
        }
        else
        {
            N--;
        }
        i=p+1;
        p=t.find_first_of("_",i);
    }
    num=atoi(t.substr(i,t.length()-i).c_str());
     //message("Num: %d", num);
        if(num)
        {
            Units[k]=num;
            //message("Units number %d: %d", k, num);
        }
        else
        {
            N--;
        }


    Classificator cc(cvt,nFolds,file_out,class_against_the_others,random_seed);
    //CrossValidationType cvt=KFOLD;
    //Classificator cc(cvt,10,"/media/Data/CompToux/cough_classification/TestMLP_cough10_fev21_2.txt",-1,-1);
    cc.LoadData(file_in,n_inputs,n_outputs,(bool)one_file_one_seq,max_load,(bool)binary);

    switch(classific)
    {
        case 0:
            cc.test_svm(-1,10,0);
            cc.test_mlp(N,Units);
            cc.test_gmm(N,Units);
            break;
        case 1:
            cc.test_svm(-1,10,0);
            break;
        case 2:
            cc.test_mlp(N,Units);
            break;
        case 3:
            cc.test_gmm(N,Units);
            break;
    }
    //cc.use_only_first_features(10);
    //return 0;
    //int hidden_units[7]={4,8,16,32,64,128,256};
    //cc.test_mlp(6,hidden_units);
    //cc.test_mlp(7, hidden_units);
    //cc.test_mlp(8);

   //cc.test_gmm(7, hidden_units);
    //cc.test_gmm(4);
   
    
    //cc.test_svm(-1,10,0);

    delete[] Units;

    return (EXIT_SUCCESS);
}

/*int **buffer;
    int N=5;
    int i, j, k;
    int taille=50000000/sizeof(int);
    buffer= (int**) malloc(N*sizeof(int*));
    for(i=0;i<N;i++)
    {
        buffer[i]=(int*)malloc(taille*sizeof(int));
    }

    if (buffer==NULL)
    {
        printf("Buffer non alloué");
        exit (1);
    }
    printf("Buffer alloué\n");
    for(j=0;j<N;j++){
        printf("%d\n",j);
        for(int i=0; i<taille; i++){
            buffer[j][i]=rand();
        }


    }

    sleep(5);

    free(buffer);
    return 0;*/