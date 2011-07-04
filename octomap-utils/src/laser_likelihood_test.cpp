#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <bot_core/bot_core.h>
#include <pthread.h>
#include <deque>
#include <bot_param/param_util.h>
#include <GL/gl.h>
#include <vector>
#include <iostream>
#include <sstream>
#include "octomap_util.hpp"

#include <list>
#include <octomap/octomap.h>
#include <bot_param/param_client.h>
#include <bot_frames/bot_frames.h>
#include <laser_utils/laser_util.h>

using namespace std;
using namespace octomap;
//TODO: make me a parameter
static int scan_skip = 2;
static int beam_skip = 2;
static double spatial_decimation = .2;

class LaserLikelihooder {
public:
  LaserLikelihooder(char * logFname);
  ~LaserLikelihooder();
  void addProjectedScan(laser_projected_scan * lscan);
  void processScansInQueue();

  BotParam *param;
  BotFrames *frames;
  bool fromLog;
  char * logFName;
  lcm_t *lcm_pub; //two different ones for running from log
  lcm_t *lcm_recv; //will point to same place if running live

  octomap::OcTree * ocTree;

  GHashTable * laser_projectors;
  std::list<laser_projected_scan *> lscans_to_be_processed;

  int64_t last_publish_time;

};

static void on_laser(const lcm_recv_buf_t *rbuf, const char *channel, const bot_core_planar_lidar_t *msg,
    void *user_data)
{
  LaserLikelihooder * self = (LaserLikelihooder *) user_data;
  Laser_projector * proj = (Laser_projector*) g_hash_table_lookup(self->laser_projectors, channel);

  static int counter = 0;
  if (counter++ % (scan_skip + 1) != 0)
    return;

  laser_projected_scan * lscan = laser_create_projected_scan_from_planar_lidar(proj, msg, bot_frames_get_root_name(
      self->frames));
  if (lscan != NULL)
    self->addProjectedScan(lscan);

  self->processScansInQueue();

}

void LaserLikelihooder::addProjectedScan(laser_projected_scan * lscan)
{
  lscans_to_be_processed.push_back(lscan);
}

LaserLikelihooder::LaserLikelihooder(char * logFname) :
  last_publish_time(-1)
{
  lcm_pub = bot_lcm_get_global(NULL);
  if (logFname != NULL) {
    fromLog = true;
    char provider_buf[1024];
    sprintf(provider_buf, "file://%s?speed=0", logFname);
    lcm_recv = lcm_create(provider_buf);
  }
  else {
    printf("running mapping from LCM\n");
    lcm_recv = lcm_pub;

  }
  param = bot_param_get_global(lcm_pub, 0);
  frames = bot_frames_get_global(lcm_pub, param);

  //TODO: these should be parameters
  ocTree = new OcTree("/home/abachrac/stuff/pods_stuff/Quad/build/bin/octomap.bt");

  laser_projectors = g_hash_table_new(g_str_hash, g_str_equal);

  char **planar_lidar_names = bot_param_get_all_planar_lidar_names(param);
  if (planar_lidar_names) {
    for (int pind = 0; planar_lidar_names[pind] != NULL; pind++) {
      char conf_path[1024];
      bot_param_get_planar_lidar_prefix(param, planar_lidar_names[pind], conf_path, sizeof(conf_path));
      char channel_path[1024];
      sprintf(channel_path, "%s.lcm_channel", conf_path);
      char * channel_name;
      int ret = bot_param_get_str(param, channel_path, &channel_name);
      if (ret < 0) {
        printf("No LCM Channel for lidar %s\n", planar_lidar_names[pind]);
        continue;
      }
      printf("subscribing to channel %s for laser %s\n", channel_name, planar_lidar_names[pind]);
      bot_core_planar_lidar_t_subscribe(lcm_recv, channel_name, on_laser, this);
      g_hash_table_insert(laser_projectors, strdup(channel_name), laser_projector_new(param, frames,
          planar_lidar_names[pind], 1));
      free(channel_name);
    }
    g_strfreev(planar_lidar_names);
  }
  else {
    fprintf(stderr, "["__FILE__":%d] Error: Could not"
    " get lidar names.\n", __LINE__);
    exit(1);
  }
}

LaserLikelihooder::~LaserLikelihooder()
{
  delete ocTree;
  //TODO: don't leak everything else?
}

void LaserLikelihooder::processScansInQueue()
{
  bot_tictoc("addLaser");
  list<laser_projected_scan *>::iterator it;
  for (it = lscans_to_be_processed.begin(); it != lscans_to_be_processed.end(); it++) {
    laser_projected_scan * lscan = *it;
    if (!lscan->projection_status) {
      laser_update_projected_scan(lscan->projector, lscan, "body");
    }
    //decimate the scan
    int lastAdd =-1e6;
    for (int i = 0; i < lscan->npoints; i++) {
      if (lscan->invalidPoints[i])
        continue;
      if ((i - lastAdd) > beam_skip ||
          bot_vector_dist_3d(point3d_as_array(&lscan->points[i]), point3d_as_array(&lscan->points[lastAdd])) > spatial_decimation ||
          bot_vector_dist_3d(point3d_as_array(&lscan->points[i]), lscan->origin.trans_vec) > (lscan->aveSurroundRange + 1.8
              * lscan->stddevSurroundRange) ||
              i<lscan->projector->surroundRegion[0] || i>lscan->projector->surroundRegion[1]) {
        lastAdd = i;
      }
      else{
        lscan->invalidPoints[i]= 3;
        lscan->numValidPoints--;
      }
    }
    printf("processing %d scans\n",lscan->numValidPoints);
    bot_tictoc("processScan");
    BotTrans body_to_local;
    bot_frames_get_trans(frames, "body", "local", &body_to_local);
    for (int k = 0; k < 1000; k++) {
      BotTrans tmp = body_to_local;
      tmp.trans_vec[0] += bot_gauss_rand(0,.5);
      tmp.trans_vec[1] += bot_gauss_rand(0,.5);
      tmp.trans_vec[2] += bot_gauss_rand(0,.5);
      double like1 = evaluateLaserLikelihood(ocTree, lscan, &tmp);
    }

    bot_tictoc("processScan");
    laser_destroy_projected_scan(lscan);
  }
  if (it != lscans_to_be_processed.begin()) {
    lscans_to_be_processed.erase(lscans_to_be_processed.begin(), it); //destructor was already called in the loop
  }
  bot_tictoc("addLaser");
}

/**
 * Shutdown
 */
static LaserLikelihooder * _map3d;
static void shutdown_module(int unused __attribute__((unused)))
{
  fprintf(stderr, "shutting down!\n");
  bot_tictoc_print_stats(BOT_TICTOC_AVG);
  exit(1);
}

/**
 * Main
 */
int main(int argc, char *argv[])
{

  signal(SIGINT, shutdown_module);
  char * logFName = NULL;
  if (argc > 1)
    logFName = argv[1];

  LaserLikelihooder *map3d = new LaserLikelihooder(logFName);
  _map3d = map3d;
  while (true) {
    int ret = lcm_handle(map3d->lcm_recv);
    if (ret != 0)
      break;//log is done...
  }
  shutdown_module(1);

}
