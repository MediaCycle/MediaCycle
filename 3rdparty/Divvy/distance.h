/*
 *  distance.h
 *  Divvy
 *
 *  Created by Joshua Lewis on 8/22/11.
 *  Copyright 2011, UC San Diego Natural Computation Lab. All rights reserved.
 *
 */

#ifndef DISTANCE_H
#define DISTANCE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dispatch/dispatch.h>

#ifdef __APPLE__
#include <vecLib/cblas.h>
#include <vecLib/clapack.h>
#else
#include <cblas.h>
#include <lapack.h>
#endif

#include "indexing.h"

void distance(int N, int D, float *data, float *result);

#endif
