#ifndef __eigen_util_h__
#define __eigen_util_h__

#include <Eigen/Dense>
#include <Eigen/Cholesky>
#include <Eigen/StdVector>
#include <math.h>
#include <iostream>
#include <bot_lcmgl_client/lcmgl.h>
#include <bot_core/bot_core.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace eigen_utils {

#define eigen_dump(MAT) std::cout << #MAT << std::endl << (MAT) << std::endl

double atan2Vec(const Eigen::Vector2d & vec);

Eigen::Vector2d angleToVec(double angle);

void angleToVec(double angle, Eigen::Vector2d & unit_vec);

/*
 * returns the skew symmetric matrix corresponding to vec.cross(<other vector>)
 */
Eigen::Matrix3d skewHat(const Eigen::Vector3d & vec)
{
  Eigen::Matrix3d skew_hat;
  skew_hat << 0, -vec(2), vec(1), vec(2), 0, -vec(0), -vec(1), vec(0), 0;
  return skew_hat;
}

/**
 * returns the exponential coordinates of quat1 - quat2
 * (quat2.inverse() * quat1)
 */
Eigen::Vector3d subtractQuats(const Eigen::Quaterniond & quat1, const Eigen::Quaterniond & quat2)
{
  Eigen::Quaterniond quat_resid = quat2.inverse() * quat1;
  Eigen::AngleAxisd angle_axis_resid(quat_resid);

  double angle = angle_axis_resid.angle();
  angle = bot_mod2pi(angle);
  return angle_axis_resid.axis() * angle;
}

void quaternionToBotDouble(double bot_quat[4], const Eigen::Quaterniond & eig_quat)
{
  bot_quat[0] = eig_quat.coeffs()(3);
  bot_quat[1] = eig_quat.coeffs()(0);
  bot_quat[2] = eig_quat.coeffs()(1);
  bot_quat[3] = eig_quat.coeffs()(2);
}

void botDoubleToQuaternion(Eigen::Quaterniond & eig_quat, const double bot_quat[4])
{
  eig_quat.coeffs()(3) = bot_quat[0];
  eig_quat.coeffs()(0) = bot_quat[1];
  eig_quat.coeffs()(1) = bot_quat[2];
  eig_quat.coeffs()(2) = bot_quat[3];
}

static inline void bot_lcmgl_vertex3d(bot_lcmgl_t * lcmgl, Eigen::Vector3d & vec)
{
  bot_lcmgl_vertex3d(lcmgl, vec(0), vec(1), vec(2));
}

void bot_lcmgl_cov_ellispe(bot_lcmgl_t * lcmgl, const Eigen::Matrix2d & cov, const Eigen::Vector3d & mu3d,
    double scale = 1, bool fill = false)
{
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eigen_solver(cov);
  Eigen::Vector2d eig_vals = eigen_solver.eigenvalues();
  Eigen::Matrix2d eig_vecs = eigen_solver.eigenvectors();

  Eigen::Vector2d A = eig_vecs.block<2, 1>(0, 0);
  double a = sqrt(eig_vals(0, 0)) * scale;
  double b = sqrt(eig_vals(1, 0)) * scale;
  double h = mu3d(0);
  double k = mu3d(1);
  double phi = atan2Vec(A);

  if (fill)
    bot_lcmgl_begin(lcmgl, GL_POLYGON);
  else
    bot_lcmgl_begin(lcmgl, GL_LINE_LOOP);

  double d_theta = M_PI / 30;
  double t = 0;
  while (t < 2 * M_PI) {
    double x = h + a * cos(t) * cos(phi) - b * sin(t) * sin(phi);
    double y = k + b * sin(t) * cos(phi) + a * cos(t) * sin(phi);
    bot_lcmgl_vertex3d(lcmgl, x, y, mu3d(2));
    t += d_theta;
  }
  bot_lcmgl_end(lcmgl);
}

void bot_lcmgl_cov_ellispe(bot_lcmgl_t * lcmgl, const Eigen::Matrix2d & cov, const Eigen::Vector2d & mu2d,
    double scale = 1, bool fill = false)
{
  Eigen::Vector3d xyz = Eigen::Vector3d::Zero();
  xyz.block<2, 1>(0, 0) = mu2d;
  bot_lcmgl_cov_ellispe(lcmgl, cov, xyz, scale, fill);
}

/**
 * generate random normal vector with mu=0, Sigma = I
 */
template<int N>
void randn_identity(Eigen::Matrix<double, N, 1> & vec)
{
  vec = Eigen::Matrix<double, N, 1>::Zero();
  int num_samps = 12;
  for (int ii = 0; ii < num_samps; ii++) {
    vec += Eigen::Matrix<double, N, 1>::Random();
  }
  vec /= 2;
}

template<int N>
Eigen::Matrix<double, N, 1> randn(const Eigen::Matrix<double, N, N> & cov)
{
  Eigen::Matrix<double, N, 1> vec;
  randn_identity(vec);
  Eigen::Matrix<double, N, N> chol_decomp = cov.llt().matrixL();
  return chol_decomp * vec;
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
  weights /= weights.sum();
  mean = state_samples * weights;

  Eigen::Matrix<double, N, 1> diff;
  covariance = Eigen::Matrix<double, N, N>::Zero();
  for (int ii = 0; ii < num_samples; ii++)
  {
    diff = mean - state_samples.block(0, ii, N, 1);
    covariance += diff * diff.transpose() * weights(ii);
  }
}

}

#endif
