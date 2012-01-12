#ifndef __EIGEN_LCM_DIRECT_INCLUDE__
#error "\nThis .hxx should not be included directly -- Include the .hpp instead!\n"
#endif

//Generic type
template<typename T>
inline const std::string typenameToStr()
{
  return std::string("not_implemented");
}
// macro to implement specializations for given types
#define EIGEN_LCM_MAKE_TYPENAME_TO_STRING( type ) \
    template<>\
    inline const std::string typenameToStr<type>() {\
       return std::string(#type);\
    }
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(double);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(float);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(int64_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(int32_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(int16_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(int8_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(uint64_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(uint32_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(uint16_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(uint8_t);
EIGEN_LCM_MAKE_TYPENAME_TO_STRING(bool);

template<typename Derived>
eigen_dense_t toLcmMsg(Eigen::DenseBase<Derived> & mat)
{
  eigen_dense_t ret_msg; //make use of return value optimization?
  ret_msg.rows = mat.rows();
  ret_msg.cols = mat.cols();
  ret_msg.data_sz = ret_msg.rows * ret_msg.cols * sizeof(typename Derived::Scalar);
  ret_msg.data.resize(ret_msg.data_sz);
  ret_msg.type = typenameToStr<typename Derived::Scalar>();

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

  assert(msg->data_sz == msg->rows * msg->cols * sizeof(typename Derived::Scalar));
  assert(msg->type == typenameToStr<typename Derived::Scalar>());
  //let eigen do the copying :-)
  ret = Eigen::Map<const typename Derived::PlainObject>(dataP, msg->rows, msg->cols);
  return ret;
}

template<typename LcmType>
typename std::vector<LcmType> loadMsgsFromLog(const char * logfileName, const char * channel)
{
  typename std::vector<LcmType> ret;
  // Open the log file.
  lcm::LogFile log(logfileName, "r");
  if (!log.good()) {
    perror("LogFile");
    fprintf(stderr, "couldn't open log file %s\n", logfileName);
    return ret;
  }

  while (1) {
    // Read a log event.
    const lcm::LogEvent *event = log.readNextEvent();
    if (!event)
      break;

    // Only process messages on the desired channel.
    if (event->channel != channel)
      continue;

    // Try to decode the message.
    LcmType msg;
    if (msg.decode(event->data, 0, event->datalen) != event->datalen)
      continue;

    ret.push_back(msg);
  }
  // Log file is closed automatically when the log variable goes out of scope.

  return ret;
}
