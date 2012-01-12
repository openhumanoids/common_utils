#ifndef __EIGEN_LCM_HPP_
#define __EIGEN_LCM_HPP_

#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/eigen_utils/eigen_dense_t.hpp>
#include <vector>
#include <string>

namespace eigen_utils {

template<typename Derived>
eigen_dense_t toLcmMsg(Eigen::DenseBase<Derived> & mat);

template<typename Derived>
typename Derived::PlainObject fromLcmMsg(const eigen_utils::eigen_dense_t * msg);

template<typename LcmType>
typename std::vector<LcmType> loadMsgsFromLog(const char * logfileName, const char * channel);

template<typename T> const std::string typenameToStr();


//include the actual implimentations
#define __EIGEN_LCM_DIRECT_INCLUDE__
#include "eigen_lcm.hxx"
}
#endif /* __EIGEN_LCM_HPP_ */
