#ifndef __eigen_rigidbody_h__
#define __eigen_rigidbody_h__

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <bot_core/bot_core.h>
#include <lcmtypes/rigid_body_pose_t.h>

namespace eigen_utils {

/*
 * returns the skew symmetric matrix corresponding to vec.cross(<other vector>)
 */
Eigen::Matrix3d skewHat(const Eigen::Vector3d & vec);

/**
 * returns the exponential coordinates of quat1 - quat2
 * (quat2.inverse() * quat1)
 */
Eigen::Vector3d subtractQuats(const Eigen::Quaterniond & quat1, const Eigen::Quaterniond & quat2);

void quaternionToBotDouble(double bot_quat[4], const Eigen::Quaterniond & eig_quat);

void botDoubleToQuaternion(Eigen::Quaterniond & eig_quat, const double bot_quat[4]);

Eigen::Quaterniond setQuatEulerAngles(const Eigen::Vector3d & eulers);

const double g_val = 9.80665; //gravity
const double rho_val = 1.2; //air density kg/m^3
const Eigen::Vector3d g_vec = -g_val * Eigen::Vector3d::UnitZ(); //ENU gravity vector

/**
 * Basic Rigid Body State representation
 *
 * The chi part of the state vector represents attitude perterbations (exponential coordinates)
 */
class RigidBodyState {
public:
  enum{
    angular_velocity_ind = 0,
    velocity_ind = 3,
    chi_ind = 6,
    position_ind = 9,
    acceleration_ind = 12,
    basic_num_states = 15
  };

  Eigen::Quaterniond quat;
  Eigen::VectorXd vec;
  int64_t utime;

protected:
  RigidBodyState(int state_dim) :
      vec(Eigen::VectorXd::Zero(state_dim)), utime(0), quat(Eigen::Quaterniond::Identity())
  {

  }

public:
  RigidBodyState() :
      vec(Eigen::VectorXd::Zero(basic_num_states)), utime(0), quat(Eigen::Quaterniond::Identity())
  {

  }

  RigidBodyState(const Eigen::VectorXd & arg_vec) :
      vec(arg_vec)
  {
    quat = Eigen::Quaterniond::Identity();
    this->chiToQuat();
    utime = 0;
  }

  RigidBodyState(const Eigen::VectorXd & arg_vec, const Eigen::Quaterniond & arg_quat) :
      vec(arg_vec), quat(arg_quat)
  {
    utime = 0;
  }

  RigidBodyState(const rigid_body_pose_t * pose) :
      vec(basic_num_states)
  {
    Eigen::Map<const Eigen::Vector3d> velocity_map(pose->vel);
    Eigen::Map<const Eigen::Vector3d> angular_velocity_map(pose->rotation_rate);
    Eigen::Map<const Eigen::Vector3d> position_map(pose->pos);
    Eigen::Map<const Eigen::Vector3d> acceleration_map(pose->accel);

    this->velocity() = velocity_map;
    this->angularVelocity() = angular_velocity_map;
    this->position() = position_map;
    this->acceleration() = acceleration_map;
    this->chi() = Eigen::Vector3d::Zero();

    eigen_utils::botDoubleToQuaternion(this->quat, pose->orientation);

    this->utime = pose->utime;
  }

  /**
   * phi, theta, psi (roll, pitch, yaw)
   */
  Eigen::Vector3d getEulerAngles() const
  {
    Eigen::Vector3d eulers = this->quat.toRotationMatrix().eulerAngles(2, 1, 0);
    return eulers;
  }

  /**
   * phi, theta, psi (roll, pitch, yaw)
   */
  void setQuatEulerAngles(const Eigen::Vector3d & eulers)
  {
    this->quat = eigen_utils::setQuatEulerAngles(eulers);
  }

  void getPose(rigid_body_pose_t * pose)
  {
    memcpy(pose->rotation_rate, this->angularVelocity().data(), 3 * sizeof(double));
    memcpy(pose->vel, this->velocity().data(), 3 * sizeof(double));
    memcpy(pose->pos, this->position().data(), 3 * sizeof(double));
    memcpy(pose->accel, this->acceleration().data(), 3 * sizeof(double));
    eigen_utils::quaternionToBotDouble(pose->orientation, this->quat);
    pose->utime = this->utime;
  }

  typedef Eigen::Block<Eigen::VectorXd, 3, 1> Block3Element;
  typedef const Eigen::Block<const Eigen::VectorXd, 3, 1> ConstBlock3Element;

  inline Block3Element velocity()
  {
    return vec.block<3, 1>(RigidBodyState::velocity_ind, 0);
  }

  inline Block3Element chi()
  {
    return vec.block<3, 1>(RigidBodyState::chi_ind, 0);
  }

  inline Block3Element position()
  {
    return vec.block<3, 1>(RigidBodyState::position_ind, 0);
  }

  inline Block3Element angularVelocity()
  {
    return vec.block<3, 1>(RigidBodyState::angular_velocity_ind, 0);
  }

  inline Block3Element acceleration()
  {
    return vec.block<3, 1>(RigidBodyState::acceleration_ind, 0);
  }

  inline Eigen::Quaterniond & orientation()
  {
    return this->quat;
  }

  inline const Eigen::Quaterniond & orientation() const
  {
    return this->quat;
  }

  //const returns
  inline ConstBlock3Element velocity() const
  {
    return vec.block<3, 1>(RigidBodyState::velocity_ind, 0);
  }

  inline ConstBlock3Element chi() const
  {
    return vec.block<3, 1>(RigidBodyState::chi_ind, 0);
  }

  inline ConstBlock3Element position() const
  {
    return vec.block<3, 1>(RigidBodyState::position_ind, 0);
  }

  inline ConstBlock3Element angularVelocity() const
  {
    return vec.block<3, 1>(RigidBodyState::angular_velocity_ind, 0);
  }

  inline ConstBlock3Element acceleration() const
  {
    return vec.block<3, 1>(RigidBodyState::acceleration_ind, 0);
  }

  void chiToQuat()
  {
    double chi_norm = this->chi().norm();
    if (chi_norm > .000001) { //tolerance check
      Eigen::Quaterniond dquat;
      dquat = Eigen::AngleAxisd(chi_norm, this->chi() / chi_norm);
      this->quat *= dquat;
      this->chi() = Eigen::Vector3d::Zero();
    }
  }

  void quatToChi()
  {
    this->chi() = eigen_utils::subtractQuats(this->quat, Eigen::Quaterniond::Identity());
    this->quat = Eigen::Quaterniond::Identity();
  }

  /**
   * add state on right (postmultiplies orientation)
   */
  void addState(const RigidBodyState & state_to_add)
  {
    this->vec += state_to_add.vec;
    this->chiToQuat();
    this->quat *= state_to_add.quat;
  }

  /**
   * subtract state (premultiplies inverse of state_to_subtract.quat
   */
  void subtractState(const RigidBodyState & state_to_subtract)
  {
    this->vec -= state_to_subtract.vec;
    this->quat = state_to_subtract.quat.inverse() * this->quat;
  }

  void getBotTrans(BotTrans * bot_trans) const
      {
    Eigen::Vector3d delta_vec = this->position();
    memcpy(bot_trans->trans_vec, delta_vec.data(), 3 * sizeof(double));

    eigen_utils::quaternionToBotDouble(bot_trans->rot_quat, this->quat);
  }

  bool hasNan()
  {
    for (int ii = 0; ii < vec.rows(); ii++) {
      if (isnan(this->vec(ii)))
        return true;
    }
    return false;
  }

  static Eigen::Vector3i angularVelocityInds()
  {
    return Eigen::Vector3i::LinSpaced(angular_velocity_ind, angular_velocity_ind + 2);
  }

  static Eigen::Vector3i velocityInds()
  {
    return Eigen::Vector3i::LinSpaced(velocity_ind, velocity_ind + 2);
  }

  static Eigen::Vector3i chiInds()
  {
    return Eigen::Vector3i::LinSpaced(chi_ind, chi_ind + 2);
  }

  static Eigen::Vector3i positionInds()
  {
    return Eigen::Vector3i::LinSpaced(position_ind, position_ind + 2);
  }
};

std::ostream& operator<<(std::ostream& output, const RigidBodyState & state);

}

#endif