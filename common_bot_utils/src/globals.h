#ifndef __agile_globals_h__
#define __agile_globals_h__

// file:  globals.h
// desc:  prototypes for accessing global/singleton objects -- objects that
//        will typically be created once throughout the lifespan of a program.

#include <glib.h>
#include <lcm/lcm.h>
#include <bot_core/bot_core.h>
#include <bot_lcmgl_client/lcmgl.h>


#ifdef __cplusplus
extern "C" {
#endif

  lcm_t * globals_get_lcm_full(const char * provider,int attach_to_glib);
  void globals_release_lcm(lcm_t *lcm);
  static inline lcm_t * globals_get_lcm(void)
  {
    return globals_get_lcm_full(NULL,0);
  }

  static inline void globals_reget_lcm(lcm_t ** lcm)
  {
    if (*lcm == NULL)
      *lcm = globals_get_lcm();
  }

  bot_lcmgl_t * globals_get_lcmgl(const char *name, const int create_if_missing);

  // needed to retrieve all lcmgls for switch_buffer
  GHashTable *globals_get_lcmgl_hashtable(void);

  BotConf * globals_get_config(void);
  void globals_release_config(BotConf *config);


#ifdef __cplusplus
}
#endif

#endif
