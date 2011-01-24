#ifndef _MATHUTIL_H
#define _MATHUTIL_H
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifndef PI
#define PI 3.14159265358979323846264338
#endif

#define to_radians(x) ( (x) * (PI / 180.0 ))
#define to_degrees(x) ( (x) * (180.0 / PI ))
#define clamp_value(x,min,max) (x < min ? min : (x > max ? max : x))

static inline double sq(double v)
{
  return v*v;
}

static inline double sgn(double v)
{
  return (v>=0) ? 1 : -1;
}

#define TWOPI_INV (0.5/PI)
#define TWOPI (2*PI)

/** valid only for v > 0 **/
static inline double mod2pi_positive(double vin)
{
    double q = vin * TWOPI_INV + 0.5;
    int qi = (int) q;

    return vin - qi*TWOPI;
}

/** Map v to [-PI, PI] **/
static inline double mod2pi(double vin)
{
    if (vin < 0)
        return -mod2pi_positive(-vin);
    else
        return mod2pi_positive(vin);
}

/** Return vin such that it is within PI degrees of ref **/
static inline double mod2pi_ref(double ref, double vin)
{
    return ref + mod2pi(vin - ref);
}

static inline int theta_to_int(double theta, int max)
{
    theta = mod2pi_ref(PI, theta);
    int v = (int) (theta / ( 2 * PI ) * max);

    if (v==max)
        v = 0;

    assert (v >= 0 && v < max);

    return v;
}

static inline void 
math_util_sincos(double theta, double *s, double *c)
{
    *s = sin(theta);
    *c = cos(theta);
}

static inline int imin(int a, int b)
{
    return (a < b) ? a : b;
}

static inline int imax(int a, int b)
{
    return (a > b) ? a : b;
}

static inline int64_t imin64(int64_t a, int64_t b)
{
    return (a < b) ? a : b;
}

static inline int64_t imax64(int64_t a, int64_t b)
{
    return (a > b) ? a : b;
}

static inline int iclamp(int v, int minv, int maxv)
{
    return imax(minv, imin(v, maxv));
}

static inline double fclamp(double v, double minv, double maxv)
{
    return fmax(minv, fmin(v, maxv));
}


#endif
