#ifndef LCMGLSTORE_HPP_
#define LCMGLSTORE_HPP_

#include <lcm/lcm.h>
#include <bot_lcmgl_client/lcmgl.h>
#include <map>
#include <string>

namespace lcmgl_utils {

class LcmglStore {
public:
  LcmglStore(lcm_t * lcm);
  virtual ~LcmglStore();

  /**
   * Get an lcmgl object by name, creating it if necessary
   */
  bot_lcmgl_t * getLcmgl(const std::string & name);

  /**
   * Call bot_lcmgl_switch_buffer() on all of the managed lcmgl objects
   */
  void switchAllBuffers();

  lcm_t * lcm;
  std::map<std::string, bot_lcmgl_t *> lcmgls;
};

} /* namespace lcmgl_utils */
#endif /* LCMGLSTORE_HPP_ */
