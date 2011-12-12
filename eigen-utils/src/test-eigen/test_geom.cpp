#include <eigen_utils/eigen_utils.hpp>

using namespace Eigen;
using namespace eigen_utils;
using namespace std;

int main(int argc, char * argv[])
{

  Vector3d mag_vec_est(-0.185755, -0.176576, 0);
  Vector3d g_vec_est(-1.4253, -0.235576, -9.70993);

  //set orientation
  Quaterniond quat_g_vec, quat_mag;
  quat_mag.setFromTwoVectors(mag_vec_est, Vector3d::UnitX());
  quat_g_vec.setFromTwoVectors(g_vec_est, -Vector3d::UnitZ());

  eigen_dump(mag_vec_est);
  eigen_dump(g_vec_est);
  eigen_dump(quat_mag.toRotationMatrix().eulerAngles(2,1,0)*180/M_PI);
  eigen_dump(quat_g_vec.toRotationMatrix().eulerAngles(2,1,0)*180/M_PI);

}
