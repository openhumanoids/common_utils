#ifndef __eigen_common_h__
#define __eigen_common_h__

#include <Eigen/Dense>
#include <iostream>
#include <algorithm>

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

/**
 * Compute the median of the values stored in this eigen array.
 *
 * This spares the copy required in the normal version, and therefore will be
 * faster for large datasets.
 *
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code was originally by Nicolas Devillard - 1998. Public domain.
 *
 */
template<typename Derived>
typename Derived::Scalar median_non_const(Eigen::DenseBase<Derived> & arr)
{
  int low, high;
  int median;
  int middle, ll, hh;

  low = 0;
  high = arr.size() - 1;
  median = (low + high) / 2;
  while (1) {
    if (high <= low) /* One element only */
      return arr(median);

    if (high == low + 1) { /* Two elements only */
      if (arr(low) > arr(high))
        std::swap(arr(low), arr(high));
      return arr(median);
    }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr(middle) > arr(high))
      std::swap(arr(middle), arr(high));
    if (arr(low) > arr(high))
      std::swap(arr(low), arr(high));
    if (arr(middle) > arr(low))
      std::swap(arr(middle), arr(low));

    /* Swap low item (now in position middle) into position (low+1) */
    std::swap(arr(middle), arr(low + 1));

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    while (1) {
      do
        ll++;
      while (arr(low) > arr(ll));
      do
        hh--;
      while (arr(hh) > arr(low));

      if (hh < ll)
        break;

      std::swap(arr(ll), arr(hh));
    }

    /* Swap middle item (in position low) back into correct position */
    std::swap(arr(low), arr(hh));

    /* Re-set active partition */
    if (hh <= median)
      low = ll;
    if (hh >= median)
      high = hh - 1;
  }
}

/**
 * Compute the median of the values stored in this eigen array.
 *
 */
template<typename Derived>
typename Derived::Scalar median(const Eigen::DenseBase<Derived> & const_arr)
{

  typename Derived::PlainObject arr = const_arr; //make a local copy... would be nice if this wasn't necessary :-/
  return median_non_const(arr);
}

}  //namespace eigen_utils
#endif
