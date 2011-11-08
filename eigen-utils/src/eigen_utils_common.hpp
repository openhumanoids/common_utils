#ifndef __eigen_common_h__
#define __eigen_common_h__

#include <Eigen/Dense>
#include <iostream>

namespace eigen_utils {
#define eigen_dump(MAT) std::cout << #MAT << std::endl << (MAT) << std::endl

#define eigen_matlab_dump(MAT) std::cout << #MAT << "=[" << (MAT) << "];\n"

#define var_dump(VAR) std::cout << #VAR << std::endl << (VAR) << std::endl


double atan2Vec(const Eigen::Vector2d & vec);

Eigen::Vector2d angleToVec(double angle);

void angleToVec(double angle, Eigen::Vector2d & unit_vec);

}

#endif
