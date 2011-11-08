#include "eigen_rand.hpp"
#include "eigen_utils_common.hpp"

namespace eigen_utils {

using namespace Eigen;

void randn_identity(Eigen::VectorXd & vec)
{
  for (int ii = 0; ii < vec.rows(); ii++) {
    vec(ii, 0) = bot_gauss_rand(0, 1);
  }
}


void fitParticles(const Eigen::MatrixXd & state_samples, Eigen::VectorXd & weights
    , Eigen::VectorXd & mean, Eigen::MatrixXd & covariance)
{

  int num_samples = state_samples.cols();
  int N = state_samples.rows();

  double sum_weights = weights.sum();
  mean = state_samples * weights / sum_weights;

  Eigen::VectorXd diff(N);
  covariance = Eigen::MatrixXd::Zero(N, N);
  for (int ii = 0; ii < num_samples; ii++)
  {
    diff = mean - state_samples.col(ii);
    covariance += diff * diff.transpose() * weights(ii);
  }
  covariance = covariance / sum_weights;
}


}

