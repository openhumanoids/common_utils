#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include <glib.h>

#include <bot_core/bot_core.h>
#include <bot_param_client/param_client.h>
#include "globals.h"


//#define dbg(...) fprintf(stderr, __VA_ARGS__)
#define dbg(...)

#define err(...) fprintf(stderr, __VA_ARGS__)

#define MAX_REFERENCES ((1LL << 60))

static lcm_t *global_lcm = NULL;
static int global_lcm_attached_to_glib = 0;
static int64_t global_lcm_refcount = 0;

static GStaticRecMutex _mutex = G_STATIC_REC_MUTEX_INIT;

lcm_t *
globals_get_lcm_full(const char * provider,int attach_to_glib)
{
  g_static_rec_mutex_lock(&_mutex);

  if (global_lcm_refcount == 0) {
    assert (! global_lcm);

    global_lcm = lcm_create(provider);
    if (!global_lcm) {
      goto fail;
    }
    if (attach_to_glib) {
      bot_glib_mainloop_attach_lcm(global_lcm);
      global_lcm_attached_to_glib = 1;
    }
  }
  else if (attach_to_glib && !global_lcm_attached_to_glib) {
//    fprintf(stderr, "WARNING, requested lcm is attached to LCM, but it was already allocated, and NOT attached\n");
    bot_glib_mainloop_attach_lcm(global_lcm);
    global_lcm_attached_to_glib = 1;
  }

  assert (global_lcm);

  if (global_lcm_refcount < MAX_REFERENCES)
    global_lcm_refcount++;
  lcm_t *result = global_lcm;
  g_static_rec_mutex_unlock(&_mutex);
  return result;
  fail: g_static_rec_mutex_unlock(&_mutex);
  return NULL;
}

void globals_release_lcm(lcm_t *lcm)
{
  g_static_rec_mutex_lock(&_mutex);
  if (global_lcm_refcount == 0) {
    fprintf(stderr, "ERROR: singleton LC refcount already zero!\n");
    g_static_rec_mutex_unlock(&_mutex);
    return;
  }
  if (lcm != global_lcm) {
    fprintf(stderr, "ERROR: %p is not the singleton LC (%p)\n", lcm, global_lcm);
  }
  global_lcm_refcount--;

  if (global_lcm_refcount == 0) {
    bot_glib_mainloop_detach_lcm(global_lcm);
    lcm_destroy(global_lcm);
    global_lcm = NULL;
  }
  g_static_rec_mutex_unlock(&_mutex);
}

static BotConf *global_config = NULL;
static int64_t global_config_refcount = 0;

BotConf *
globals_get_config(int keep_updated)
{
  g_static_rec_mutex_lock(&_mutex);

  if (global_config_refcount == 0) {
    assert (! global_config);

    if (!global_lcm_refcount)
      globals_get_lcm();

    global_config = bot_param_new_from_server(global_lcm,keep_updated);
    if (!global_config) {
      goto fail;
    }
  }

  assert (global_config);

  if (global_config_refcount < MAX_REFERENCES)
    global_config_refcount++;
  BotConf *result = global_config;
  g_static_rec_mutex_unlock(&_mutex);
  return result;
  fail: g_static_rec_mutex_unlock(&_mutex);
  fprintf(stderr,"ERROR: Could Not Get BotConf!!!\n");
  return NULL;
}

void globals_release_config(BotConf *config)
{
  g_static_rec_mutex_lock(&_mutex);
  if (global_config_refcount == 0) {
    fprintf(stderr, "ERROR: singleton config refcount already zero!\n");
    g_static_rec_mutex_unlock(&_mutex);
    return;
  }
  if (config != global_config) {
    fprintf(stderr, "ERROR: %p is not the singleton BotConf (%p)\n", config, global_config);
  }
  global_config_refcount--;

  if (global_config_refcount == 0) {
    bot_conf_free(global_config);
    global_config = NULL;
  }
  g_static_rec_mutex_unlock(&_mutex);
}

static GHashTable *_lcmgl_hashtable = NULL;

bot_lcmgl_t *
globals_get_lcmgl(const char *name, const int create_if_missing)
{
  g_static_rec_mutex_lock(&_mutex);

  if (!global_lcm_refcount)
    globals_get_lcm();

  if (!_lcmgl_hashtable) {
    _lcmgl_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
  }

  bot_lcmgl_t *lcmgl = g_hash_table_lookup(_lcmgl_hashtable, name);
  if (!lcmgl && create_if_missing) {
    lcmgl = bot_lcmgl_init(global_lcm, name);
    g_hash_table_insert(_lcmgl_hashtable, strdup(name), lcmgl);
  }

  g_static_rec_mutex_unlock(&_mutex);
  return lcmgl;
}

GHashTable *
globals_get_lcmgl_hashtable(void)
{
  return _lcmgl_hashtable;
}
