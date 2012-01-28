#ifndef __eigen_rand_h__
#define __eigen_rand_h__

#include <Eigen/Dense>
#include <Eigen/Cholesky>
#include <math.h>
#include <bot_core/bot_core.h>

namespace eigen_utils {

//todo see about replacing templated functions with MatrixBase arguments and then checking for num rows and cols
/**
 * Fill an Eigen vector with gaussian random numbers with mu=0, Sigma = I
 */
template<typename scalarType, int N>
void randn_identity(Eigen::Matrix<scalarType, N, 1> & vec)
{
  for (int ii = 0; ii < vec.size(); ii++) {
    vec(ii) = bot_gauss_rand(0, 1);
  }
}

template<typename scalarType, int N>
Eigen::Matrix<scalarType, N, 1> randn(const Eigen::Matrix<scalarType, N, N> & cov)
{
  Eigen::Matrix<scalarType, N, 1> vec;
  randn_identity(vec);
  Eigen::Matrix<scalarType, N, N> chol_decomp = cov.llt().matrixL();
  return chol_decomp * vec;
}

/**
 * unnormalized log likelihood
 */
template<typename scalarType, int N>
scalarType loglike_unnormalized(const Eigen::Matrix<scalarType, N, 1> & x, const Eigen::Matrix<scalarType, N, 1> & mu
    , const Eigen::Matrix<scalarType, N, N> & sigma)
{
  Eigen::Matrix<scalarType, N, 1> diff = mu - x;
  return -0.5 * diff.transpose() * sigma.ldlt().solve(diff);
}

/**
 * unnormalized log likelhihood using information matrix
 */
template<typename scalarType, int N>
scalarType loglike_information_unnormalized(const Eigen::Matrix<scalarType, N, 1> & x
    ,const Eigen::Matrix<scalarType, N, 1> & mu , const Eigen::Matrix<scalarType, N, N> & sigma_inv)
{
  Eigen::Matrix<scalarType, N, 1> diff = mu - x;
  return -0.5 * diff.transpose() * sigma_inv * diff;
}

template<typename scalarType, int N>
scalarType normpdf(const Eigen::Matrix<scalarType, N, 1> & x, const Eigen::Matrix<scalarType, N, 1> & mu
    , const Eigen::Matrix<scalarType, N, N> & sigma)
{
  Eigen::Matrix<scalarType, N, 1> diff = mu - x;

  scalarType exponent = -0.5 * diff.transpose() * sigma.ldlt().solve(diff);

  return exp(exponent) / (pow(2 * M_PI, ((scalarType) N) / 2.0) * pow(sigma.determinant(), 0.5));
}

template<typename scalarType, int Ndim, int Nsamples>
void fitParticles(const Eigen::Matrix<scalarType, Ndim, Nsamples> & state_samples
    , Eigen::Matrix<scalarType, Ndim, 1>& weights , Eigen::Matrix<scalarType, Ndim, 1> & mean
    ,Eigen::Matrix<scalarType, Ndim, Ndim> & covariance)
{

  int num_samples = state_samples.cols();
  int N = state_samples.rows();

  scalarType sum_weights = weights.sum();
  mean = state_samples * weights / sum_weights;

  Eigen::Matrix<scalarType, Ndim, 1> diff(N);
  covariance.setZero(N, N);
  for (int ii = 0; ii < num_samples; ii++)
  {
    diff = mean - state_samples.col(ii);
    covariance += diff * diff.transpose() * weights(ii);
  }
  covariance = covariance / sum_weights;
}

}
#endif
