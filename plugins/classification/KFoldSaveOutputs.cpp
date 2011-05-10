// Copyright (C) 2003--2004 Ronan Collobert (collober@idiap.ch)
//                and Samy Bengio (bengio@idiap.ch)
//                
// This file is part of Torch 3.1.
//
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "KFoldSaveOutputs.h"
#include <Random.h>

namespace Torch {

KFoldSaveOutputs::KFoldSaveOutputs(Trainer* trainer_, int kfold_)
{
  kfold = kfold_;
  trainer = trainer_;

  train_subsets = (int**)allocator->alloc(sizeof(int*)*kfold);
  test_subsets = (int**)allocator->alloc(sizeof(int*)*kfold);
  n_train_subsets = (int*)allocator->alloc(sizeof(int)*kfold);
  n_test_subsets = (int*)allocator->alloc(sizeof(int)*kfold);

  for (int i=0;i<kfold;i++)
  {
    train_subsets[i] = NULL;
    test_subsets[i] = NULL;
  }
}

void KFoldSaveOutputs::sample(int n_examples)
{
  int *mix_subset = (int *)Allocator::sysAlloc(sizeof(int)*n_examples);
  Random::getShuffledIndices(mix_subset, n_examples);
  int fucking_hack_because_round_sucks = n_examples/kfold;
  int taille_subset = ( (((real)n_examples)/((real)kfold)) - (real)fucking_hack_because_round_sucks >= 0.5 ? fucking_hack_because_round_sucks+1 : fucking_hack_because_round_sucks );
  for(int i = 0; i < kfold; i++)
  {
    n_train_subsets[i] = 0;
    n_test_subsets[i] = 0;

    for(int j = 0; j < i*taille_subset; j++)
      train_subsets[i][n_train_subsets[i]++] = mix_subset[j];
    for(int j = i*taille_subset; j < min((i+1)*taille_subset, n_examples); j++)
      test_subsets[i][n_test_subsets[i]++] = mix_subset[j];
    if(i == kfold-1)
    {
      for(int j = min((i+1)*taille_subset, n_examples); j < n_examples; j++)
        test_subsets[i][n_test_subsets[i]++] = mix_subset[j];
    }
    else
    {
      for(int j = (i+1)*taille_subset; j < n_examples; j++)
        train_subsets[i][n_train_subsets[i]++] = mix_subset[j];
    }
  }
  free(mix_subset);
}

void KFoldSaveOutputs::crossValidate(DataSet *data, MeasurerList *train_measurers, MeasurerList *test_measurers, MeasurerList *cross_valid_measurers)
{
  for (int i=0;i<kfold;i++)
  {
    train_subsets[i] = (int*)allocator->realloc(train_subsets[i], sizeof(int)*data->n_examples);
    test_subsets[i] = (int*)allocator->realloc(test_subsets[i], sizeof(int)*data->n_examples);
  }

  sample(data->n_examples);

  if(cross_valid_measurers)
  {
    for(int i = 0; i < cross_valid_measurers->n_nodes; i++)
      cross_valid_measurers->nodes[i]->reset();
  }

  for(int i = 0; i < kfold; i++)
  {
    data->pushSubset(train_subsets[i], n_train_subsets[i]);
    trainer->machine->reset();

    trainer->train(data, train_measurers);
    data->popSubset();

    data->pushSubset(test_subsets[i], n_test_subsets[i]);
    trainer->test(test_measurers);
    data->popSubset();

    if(cross_valid_measurers)
    {
      for(int j = 0; j < cross_valid_measurers->n_nodes; j++)
        cross_valid_measurers->nodes[j]->measureExample();
    }
  }
  
  if(cross_valid_measurers)
  {
    for(int i = 0; i < cross_valid_measurers->n_nodes; i++)
    {
      cross_valid_measurers->nodes[i]->measureIteration();
      cross_valid_measurers->nodes[i]->measureEnd();
    }
  }
}

void KFoldSaveOutputs::crossValidateJay(DataSet *data, DiskXFile *saveOutputs, int nClasses, MeasurerList *train_measurers, MeasurerList *test_measurers, MeasurerList *cross_valid_measurers)
{
  for (int i=0;i<kfold;i++)
  {
    train_subsets[i] = (int*)allocator->realloc(train_subsets[i], sizeof(int)*data->n_examples);
    test_subsets[i] = (int*)allocator->realloc(test_subsets[i], sizeof(int)*data->n_examples);
  }

  sample(data->n_examples);

  if(cross_valid_measurers)
  {
    for(int i = 0; i < cross_valid_measurers->n_nodes; i++)
      cross_valid_measurers->nodes[i]->reset();
  }

  int p,q;

  int** confmat=(int**)allocator->alloc(nClasses*sizeof(int*));
  for(int i=0;i<nClasses;i++)
  {
      confmat[i]=(int*)allocator->alloc(nClasses*sizeof(int));
  }
  int* computedClass=(int*)allocator->alloc(2*sizeof(int));
  int* targetClass=(int*)allocator->alloc(2*sizeof(int));;
  double *maxval=(double*)allocator->alloc(2*sizeof(double));;
  double sum;


  for(int i = 0; i < kfold; i++)
  {
      message("\nFold %d:", i+1);
      computedClass=(int*)allocator->realloc(computedClass,n_test_subsets[i]*sizeof(int));
      targetClass=(int*)allocator->realloc(targetClass,n_test_subsets[i]*sizeof(int));
      maxval=(double*)allocator->realloc(maxval, n_test_subsets[i]*sizeof(double));
    data->pushSubset(train_subsets[i], n_train_subsets[i]);
    trainer->machine->reset();

    trainer->train(data, train_measurers);
    data->popSubset();

    data->pushSubset(test_subsets[i], n_test_subsets[i]);
    trainer->test(test_measurers);
    saveOutputs->printf("-Fold %d\n",i);
    for(p=0;p<n_test_subsets[i];p++)
    {
        data->setExample(p);
        trainer->machine->forward(data->inputs);
        sum=0;
        targetClass[p]=0;
        
        for(q=0;q<data->n_targets;q++)
        {
            if(data->n_targets>1)
            {
                if((int)data->targets->frames[0][q]) // works if class encoded in one-hot-format
                {
                    targetClass[p]=q;
                }
            }
            else
            {
                targetClass[p]=_biggerthanzero(data->targets->frames[0][0]);
            }
            if(q==0)
            {
                maxval[p]=trainer->machine->outputs->frames[0][q];
                if(data->n_targets>1) // MLP case with one-hot format
                {
                    computedClass[p]=0;
                }
                else  //SVM case
                {
                    computedClass[p]=_biggerthanzero(trainer->machine->outputs->frames[0][q]);
                }
            }
            else
            {
                if(trainer->machine->outputs->frames[0][q]>maxval[p])
                {
                    maxval[p]=trainer->machine->outputs->frames[0][q];
                    computedClass[p]=q;
                }
            }
            saveOutputs->printf("%e ",trainer->machine->outputs->frames[0][q]);
            //sum+=exp(trainer->machine->outputs->frames[0][q]);
        }
        saveOutputs->printf("%d %d %d\n", computedClass[p], targetClass[p], test_subsets[i][p]);
    }
    _computeConfMat(targetClass, computedClass, nClasses, n_test_subsets[i], true);


    data->popSubset();

    if(cross_valid_measurers)
    {
      for(int j = 0; j < cross_valid_measurers->n_nodes; j++)
        cross_valid_measurers->nodes[j]->measureExample();
    }
  }

  if(cross_valid_measurers)
  {
    for(int i = 0; i < cross_valid_measurers->n_nodes; i++)
    {
      cross_valid_measurers->nodes[i]->measureIteration();
      cross_valid_measurers->nodes[i]->measureEnd();
    }
  }
}

int KFoldSaveOutputs::_printConfMat(int* confusion, int n_classes)
{
    int i,k, tot=0;
    float accuracy=0;
    for (i=0; i<n_classes; i++)
    {
        //print("%d:\t",i);
        for(k=0;k<n_classes;k++)
        {
            //saveFile->printf("%d ", confusion[i][k]);
            tot+=confusion[i*n_classes+k];
            message("ComputedClass %d; TargetClass %d; N:%d", i, k, confusion[i*n_classes+k]);
         
        }
        accuracy+=confusion[i*n_classes+i];
        //saveFile->printf("\n");
       
    }
    accuracy/=tot;
    message("Global accuracy: %f", 100*accuracy);
    //saveFile->printf("Global accuracy: %f\n", 100*accuracy);
}

int KFoldSaveOutputs::_biggerthanzero(real v)
{
    if(v>=0)
    {
        return 1;
    }
    else
    {
        return 0; // so that we can print a confusion matrix
    }
}


int KFoldSaveOutputs::_computeConfMat(int* targets, int* computed, int n_classes, int n_frames, bool print)
{

    Allocator a;
    int *confmat=new int[n_classes*n_classes];
    //int** confmat;
    int i, k, m;
    float accuracy=0;
    for(i=0;i<n_classes*n_classes;i++)
    {
            confmat[i]=0;

    }
    for(m=0;m<n_frames;m++)
    {
            confmat[(computed[m]*n_classes)+targets[m]]++;
    }
    if(print)
    {
        _printConfMat(confmat,n_classes);
    }
    delete[] confmat;
}

KFoldSaveOutputs::~KFoldSaveOutputs()
{
}

}





