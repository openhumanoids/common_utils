#include "eigen_utils.hpp"

namespace eigen_utils {

using namespace Eigen;

double atan2Vec(const Eigen::Vector2d & vec)
{
  return atan2(vec(1), vec(0));
}

Eigen::Vector2d angleToVec(double angle)
{
  Eigen::Vector2d unit_vec;
  unit_vec << cos(angle), sin(angle);
  return unit_vec;
}

void angleToVec(double angle, Eigen::Vector2d & unit_vec)
{
  unit_vec << cos(angle), sin(angle);
}

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

void bot_gl_cov_ellipse(const Eigen::Matrix2d & cov, double scale)
{
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eigen_solver(cov);
  Eigen::Vector2d eig_vals = eigen_solver.eigenvalues();
  Eigen::Matrix2d eig_vecs = eigen_solver.eigenvectors();

  double a = sqrt(eig_vals(0, 0)) * scale;
  double b = sqrt(eig_vals(1, 0)) * scale;

  Eigen::Vector2d A = eig_vecs.block<2, 1>(0, 0);
  Eigen::Vector2d B = eig_vecs.block<2, 1>(0, 1);

  A.normalize();
  B.normalize();
  A *= a;
  B *= b;
  double phi = atan2Vec(A);

  glBegin(GL_LINES);
  glVertex2d(-A(0), -A(1));
  glVertex2d(A(0), A(1));
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(-B(0), -B(1));
  glVertex2d(B(0), B(1));
  glEnd();

  bot_gl_draw_ellipse(b, a, phi, 100);
}

void bot_lcmgl_vertex3d(bot_lcmgl_t * lcmgl, Eigen::Vector3d & vec)
{
  bot_lcmgl_vertex3d(lcmgl, vec(0), vec(1), vec(2));
}

void bot_lcmgl_cov_ellispe(bot_lcmgl_t * lcmgl, const Eigen::Matrix2d & cov, const Eigen::Vector3d & mu3d,
    double scale, bool fill)
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
    double scale, bool fill)
{
  Eigen::Vector3d xyz = Eigen::Vector3d::Zero();
  xyz.block<2, 1>(0, 0) = mu2d;
  bot_lcmgl_cov_ellispe(lcmgl, cov, xyz, scale, fill);
}

void bot_lcmgl_rotate_frame(bot_lcmgl_t * lcmgl, const Eigen::Quaterniond & quat, const Eigen::Vector3d & pos)
{
  Matrix3d rot_mat = quat.toRotationMatrix();
  Affine3d trans;
  trans.linear() = rot_mat;
  trans.translation() = pos;

  lcmglPushMatrix();
  lcmglMultMatrixd(trans.data());
}

void bot_lcmgl_rotate_frame_from_ned_to_enu(bot_lcmgl_t * lcmgl)
{
  Matrix3d rot_mat;
  rot_mat << 0, 1, 0,
      1, 0, 0,
      0, 0, -1;
  Vector3d pos = Vector3d::Zero();
  Quaterniond quat(rot_mat);
  bot_lcmgl_rotate_frame(lcmgl, quat, pos);

}

}

