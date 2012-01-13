#ifndef __stl_string_utils_hpp__
#define __stl_string_utils_hpp__
#include <sstream>

namespace stl_utils {

template<class T>
inline const std::string to_string(const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

}
#endif
