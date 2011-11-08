#ifndef __eigen_numerical_h__
#define __eigen_numerical_h__

#include <stdio.h>
#include <Eigen/Dense>
#include "eigen_utils_common.hpp"

namespace eigen_utils {

template<typename DerivedQ, typename DerivedA, typename Derivedb, typename Derivedx>
void quadProgEliminationSolve(const Eigen::MatrixBase<DerivedQ> & Q, const Eigen::MatrixBase<DerivedA> & A,
    const Eigen::MatrixBase<Derivedb> & b, Eigen::MatrixBase<Derivedx> & x_star)
{
  int m = A.rows();
  int n = A.cols();
  assert(n>=m);

  assert(Q.cols()==Q.rows());
  assert(Q.cols()==n);
  assert(b.rows()==m);
  assert(b.cols()==1);
  assert(x_star.cols()==1);
  assert(x_star.rows()==n);

  if (m == n) {
//    Eigen::ColPivHouseholderQR<Eigen::MatrixBase<DerivedA> > A_decomp;
//    A_decomp.compute(A);
//    if (!A_decomp.isInvertible()) {
//      fprintf(stderr,
//          "Warning: A matrix for fully constrained quadProgEliminationSolve in %s is not invertible, line %d\n",
//          __FILE__, __LINE__);
//    }
//    x_star = A_decomp.solve(b);
    x_star = A.colPivHouseholderQr().solve(b);
  }
  else {
    Eigen::MatrixXd B = A.leftCols(m);
    Eigen::MatrixXd R = A.rightCols(n - m);

    Eigen::MatrixXd Q_BB = Q.topLeftCorner(m, m);
    Eigen::MatrixXd Q_BR = Q.topRightCorner(m, n - m);
    Eigen::MatrixXd Q_RB = Q.bottomLeftCorner(n - m, m);
    Eigen::MatrixXd Q_RR = Q.bottomRightCorner(n - m, n - m);

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> B_decomp;
    B_decomp.compute(B);
    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> B_transpose_decomp;
    B_transpose_decomp.compute(B.transpose());

    if (!B_decomp.isInvertible()) {
      fprintf(stderr, "Warning: B matrix in A=[B R] for quadProgEliminationSolve in %s is not invertible, line %d\n",
          __FILE__, __LINE__);
    }

    Eigen::MatrixXd B_inv_R = B_decomp.solve(R);

    //f_prime_RR=2*b'*(B'\(Q_BR-(Q_BB/B)*R));
    Eigen::VectorXd f_prime_RR;
//    eigen_dump(b);
//    eigen_dump(Q_BR);
//    eigen_dump(R);
//    eigen_dump(Q_BB);
//    eigen_dump(B_inv_R);
    f_prime_RR = (2 * b.transpose() * (B_transpose_decomp.solve(Q_BR - Q_BB * B_inv_R))).transpose();

    //    Q_prime_RR=...
    //          Q_RR+R'*(B'\Q_BB/B)*R...
    //          -(R'/B')*Q_BR...
    //          -(Q_RB/B)*R;
    Eigen::MatrixXd Q_prime_RR =
        Q_RR + B_inv_R.transpose() * Q_BB * B_inv_R
            - B_inv_R.transpose() * Q_BR
            - Q_RB * B_inv_R;

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> Q_prime_RR_decomp;
    Q_prime_RR_decomp.compute(Q_prime_RR);
    if (!Q_prime_RR_decomp.isInvertible()) {
      fprintf(stderr, "Warning: Q_primer_RR matrix in quadProgEliminationSolve in %s is not invertible, line %d\n",
          __FILE__, __LINE__);
    }

    //    x_R_star = -(2*Q_prime_RR)\f_prime_RR';
    //    x_B_star = B\(b-R*x_R_star);
    //    x_star = [x_B_star;x_R_star];
    x_star.bottomRows(n - m) = -0.5 * Q_prime_RR_decomp.solve(f_prime_RR);
    x_star.topRows(m) = B_decomp.solve(b - R * x_star.bottomRows(n - m));
  }
}

}

#endif
