#ifndef __eigen_util_h__
#define __eigen_util_h__

#include <Eigen/Dense>
#include <Eigen/Cholesky>
#include <math.h>
#include <bot_lcmgl_client/lcmgl.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define eigen_dump(MAT) cout << #MAT << endl << (MAT) << endl

namespace eigen_utils {

double atan2Vec(const Eigen::Vector2d & vec);

Eigen::Vector2d angleToVec(double angle);

void angleToVec(double angle, Eigen::Vector2d & unit_vec);

/*
 * computes quat such that: quat*vec1 = vec2
 */
void alignVectors(const Eigen::Vector3d & vec1, const Eigen::Vector3d & vec2, Eigen::Quaterniond & quat)
{
  Eigen::Vector3d axis = vec1.cross(vec2);
  axis = axis / axis.norm();
  double angle = acos(vec1.dot(vec2)) / (vec1.norm() * vec2.norm());

  quat = Eigen::Quaterniond(Eigen::AngleAxisd(angle, axis));
}

template<int N>
void double_array_to_vector(const double * array, Eigen::Matrix<double, N, 1> & vector)
{
  for (int ii = 0; ii < N; ii++) {
    vector(ii, 0) = array[ii];
  }
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

  Eigen::Vector2d A = eig_vecs.block<2, 1> (0, 0);
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
  xyz.block<2, 1> (0, 0) = mu2d;
  bot_lcmgl_cov_ellispe(lcmgl, cov, xyz, scale, fill);
}

template<int N>
Eigen::Matrix<double, N, 1> randn(const Eigen::Matrix<double, N, N> & cov)
{
  int num_samps = 12;
  //first get independent gaussian vector
  Eigen::Matrix<double, N, 1> vec = Eigen::Matrix<double, N, 1>::Zero();

  for (int ii = 0; ii < num_samps; ii++) {
    vec += Eigen::Matrix<double, N, 1>::Random();
  }
  vec /= 2;
  Eigen::Matrix<double, N, N> chol_decomp = cov.llt().matrixL();
  return chol_decomp * vec;
}
}

#endif
