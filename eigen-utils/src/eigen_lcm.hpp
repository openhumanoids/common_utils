#ifndef EIGEN_LCM_HPP_
#define EIGEN_LCM_HPP_

#include <lcmtypes/eigen_utils/eigen_dense_t.hpp>

namespace eigen_utils {

template<typename Derived>
eigen_dense_t toLcmMsg(Eigen::DenseBase<Derived> & mat)
{
  eigen_dense_t ret_msg; //make use of return value optimization?
  ret_msg.rows = mat.rows();
  ret_msg.cols = mat.cols();
  ret_msg.data_sz = ret_msg.rows * ret_msg.cols * sizeof(typename Derived::Scalar);
  ret_msg.data.resize(ret_msg.data_sz);

  //ugly hack to treat the vector as a C-style array
  typename Derived::Scalar* dataP = (typename Derived::Scalar*) &ret_msg.data[0];
  //let eigen do the copying :-)
  Eigen::Map<typename Derived::PlainObject>(dataP, ret_msg.rows, ret_msg.cols) = mat;

  return ret_msg;
}

template<typename Derived>
typename Derived::PlainObject fromLcmMsg(const eigen_utils::eigen_dense_t * msg)
{
  typename Derived::PlainObject ret;
  //ugly hack to treat the vector as a C-style array
  const typename Derived::Scalar* dataP = (const typename Derived::Scalar*) &msg->data[0];

  //let eigen do the copying :-)
  ret = Eigen::Map<const typename Derived::PlainObject>(dataP, msg->rows, msg->cols);
  return ret;
}
}
#endif /* EIGEN_LCM_HPP_ */
