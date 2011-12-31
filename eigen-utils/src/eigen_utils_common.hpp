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

//utilities for flattening an symmetric matrices (ie cov matrices)
Eigen::VectorXd flattenSymmetric(Eigen::MatrixXd symm);
Eigen::MatrixXd unflattenSymmetric(Eigen::VectorXd flat);

bool hasNan(const Eigen::MatrixXd &m);
void assertNoNan(const Eigen::MatrixXd &m);

template<typename Derived>
int numNonZeros(const Eigen::DenseBase<Derived> & m)
{
  int nnz = 0;
  for (int i = 0; i < m.size(); i++) {
    if (m(i))
      nnz++;
  }
  return nnz;
}

template<typename Derived>
Eigen::ArrayXi findNonZeros(const Eigen::DenseBase<Derived> & arr)
{
  int nnz = numNonZeros(arr);
  Eigen::ArrayXi nz(nnz);
  int cnt = 0;
  for (int i = 0; i < arr.size(); i++) {
    if (arr(i))
      nz(cnt++) = i;
  }
  return nz;
}

}  //namespace eigen_utils
#endif
