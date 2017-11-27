cmake_minimum_required(VERSION 2.8.3)
project(eigen_utils_lcmtypes)

find_package(catkin REQUIRED COMPONENTS
lcm_catkin
)

catkin_package(
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS lcm_catkin
)

FindLCM(../../eigen-utils/)

AddLCM(
eigen_utils_eigen_dense_t
eigen_utils_eigen_matrixxd_t
eigen_utils_matlab_rpc_ack_t
eigen_utils_matlab_rpc_command_t
eigen_utils_matlab_rpc_return_t
eigen_utils_pose_t
)

GenerateLCM()

catkin_package(LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS lcm_catkin
  INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include)
