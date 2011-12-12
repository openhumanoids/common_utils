#include "eigen_rigidbody.hpp"
#include "eigen_utils_common.hpp"

namespace eigen_utils {

using namespace Eigen;


std::ostream& operator<<(std::ostream& output, const RigidBodyState & state)
{
  output<< "angularVelocity: " << state.angularVelocity().transpose();
  output<< ", velocity: " << state.velocity().transpose();
  output<< ", chi: " << state.chi().transpose();
  output<< ", position: " << state.position().transpose();
  output<< ", acceleration: " << state.acceleration().transpose();
  output<< ", RPY: " << state.getEulerAngles().transpose();
  return output;
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

Eigen::Quaterniond setQuatEulerAngles(const Eigen::Vector3d & eulers)
{
  Eigen::Quaterniond quat = Eigen::AngleAxisd(eulers(2), Eigen::Vector3d::UnitZ())
      * Eigen::AngleAxisd(eulers(1), Eigen::Vector3d::UnitY())
      * Eigen::AngleAxisd(eulers(0), Eigen::Vector3d::UnitX());

  return quat;
}

}

