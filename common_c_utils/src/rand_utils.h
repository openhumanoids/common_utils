/*
 * carmen3d_rand_utils.h
 *
 *  Created on: Dec 20, 2009
 *      Author: abachrac
 */

//wrapper code around the fast ziggurat random number renerators

#ifndef CARMEN3D_RAND_UTILS_H_
#define CARMEN3D_RAND_UTILS_H_

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// random number between [0, 1)
static inline float randf()
{
    return ((float) rand()) / (RAND_MAX + 1.0);
}

static inline float signed_randf()
{
    return randf()*2.0 - 1.0;
}
//random number between mi and ma
static inline float randf_in_range(float mi, float ma)
{
    return randf()*(ma-mi) + mi;
}


// return a random integer between [0, bound)
static inline int irand(int bound)
{
    int v = (int) (randf()*bound);
    assert(v >= 0);
    assert(v < bound);
    return v;
}

double gauss_rand(double mu, double sigma);

#ifdef __cplusplus
}
#endif

#endif /* CARMEN3D_RAND_UTILS_H_ */
