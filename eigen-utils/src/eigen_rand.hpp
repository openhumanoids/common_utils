#ifndef __eigen_rand_h__
#define __eigen_rand_h__

#include <Eigen/Dense>
#include <Eigen/Cholesky>
#include <math.h>
#include <bot_core/bot_core.h>

namespace eigen_utils {

//todo see about replacing templated functions with MatrixBase arguments and then checking for num rows and cols
/**
 * generate random normal vector with mu=0, Sigma = I
 */
template<int N>
void randn_identity(Eigen::Matrix<double, N, 1> & vec)
{
  for (int ii = 0; ii < N; ii++) {
    vec(ii, 0) = bot_gauss_rand(0, 1);
  }
}

void randn_identity(Eigen::VectorXd & vec);

template<int N>
Eigen::Matrix<double, N, 1> randn(const Eigen::Matrix<double, N, N> & cov)
{
  Eigen::Matrix<double, N, 1> vec;
  randn_identity(vec);
  Eigen::Matrix<double, N, N> chol_decomp = cov.llt().matrixL();
  return chol_decomp * vec;
}

/**
 * unnormalized log likelihood
 */
template<int N>
double loglike_unnormalized(const Eigen::Matrix<double, N, 1> & x, const Eigen::Matrix<double, N, 1> & mu
    , const Eigen::Matrix<double, N, N> & sigma)
{
  Eigen::Matrix<double, N, 1> diff = mu - x;
  return -0.5 * diff.transpose() * sigma.ldlt().solve(diff);
}

/**
 * unnormalized log likelhihood using information matrix
 */
template<int N>
double loglike_information_unnormalized(const Eigen::Matrix<double, N, 1> & x, const Eigen::Matrix<double, N, 1> & mu
    , const Eigen::Matrix<double, N, N> & sigma_inv)
{
  Eigen::Matrix<double, N, 1> diff = mu - x;
  return -0.5 * diff.transpose() * sigma_inv * diff;
}

template<int N>
double normpdf(const Eigen::Matrix<double, N, 1> & x, const Eigen::Matrix<double, N, 1> & mu
    , const Eigen::Matrix<double, N, N> & sigma)
{
  Eigen::Matrix<double, N, 1> diff = mu - x;

  double exponent = -0.5 * diff.transpose() * sigma.ldlt().solve(diff);

  return exp(exponent) / (pow(2 * M_PI,((double) N) / 2.0) * pow(sigma.determinant(), 0.5));
}

template<int N>
void fitParticles(
    const Eigen::Matrix<double, N, Eigen::Dynamic> & state_samples, Eigen::VectorXd & weights
    , Eigen::Matrix<double, N, 1> & mean, Eigen::Matrix<double, N, N> & covariance)
{

  int num_samples = state_samples.cols();

  double sum_weights = weights.sum();
  mean = state_samples * weights / sum_weights;

  Eigen::Matrix<double, N, 1> diff;
  covariance = Eigen::Matrix<double, N, N>::Zero();
  for (int ii = 0; ii < num_samples; ii++)
  {
    diff = mean - state_samples.block(0, ii, N, 1);
    covariance += diff * diff.transpose() * weights(ii);
  }
  covariance = covariance / sum_weights;
}

void fitParticles(const Eigen::MatrixXd & state_samples, Eigen::VectorXd & weights
    , Eigen::VectorXd & mean, Eigen::MatrixXd & covariance);
}

#endif
