#include "LcmglStore.hpp"

namespace lcmgl_utils {

using namespace std;

LcmglStore::LcmglStore(lcm_t * lcm_) :
    lcm(lcm_)
{

}

LcmglStore::~LcmglStore()
{
  std::map<string, bot_lcmgl_t *>::iterator it;
  for (it = lcmgls.begin(); it != lcmgls.end(); it++) {
    bot_lcmgl_t * lcmgl = it->second;
    bot_lcmgl_destroy(lcmgl);
  }
}

void LcmglStore::switchAllBuffers()
{
  std::map<string, bot_lcmgl_t *>::iterator it;
  for (it = lcmgls.begin(); it != lcmgls.end(); it++) {
    bot_lcmgl_t * lcmgl = it->second;
    bot_lcmgl_switch_buffer(lcmgl);
  }
}

bot_lcmgl_t * LcmglStore::getLcmgl(const std::string & name)
{
  std::map<string, bot_lcmgl_t *>::iterator it;
  it = lcmgls.find(name);
  if (it == lcmgls.end()) {
    bot_lcmgl_t * lcmgl = bot_lcmgl_init(lcm, name.c_str());
    lcmgls.insert(pair<string, bot_lcmgl_t *>(name, lcmgl));
    return lcmgl;
  }
  else {
    return it->second;
  }
}

} /* namespace lcmgl_utils */
