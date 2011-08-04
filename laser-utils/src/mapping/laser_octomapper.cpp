#include "laser_octomapper.hpp"
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

using namespace std;
using namespace octomap;
//TODO: make me a parameter
static int scan_skip = 0;
static int beam_skip = 0;
static double publish_interval = 5;

#define MISS_INC -0.1
#define HIT_INC 1.0

static void on_laser(const lcm_recv_buf_t *rbuf, const char *channel, const bot_core_planar_lidar_t *msg,
    void *user_data)
{
  LaserOctomapper * self = (LaserOctomapper *) user_data;
  Laser_projector * proj = (Laser_projector*) g_hash_table_lookup(self->laser_projectors, channel);

  static int counter = 0;
  if (counter++ % (scan_skip + 1) != 0)
    return;

  laser_projected_scan * lscan = laser_create_projected_scan_from_planar_lidar(proj, msg, bot_frames_get_root_name(
      self->frames));
  if (lscan != NULL)
    self->addProjectedScan(lscan);

  self->processScansInQueue();

  if (msg->utime - self->last_publish_time > publish_interval * 1e6) {
    self->last_publish_time = msg->utime;
    self->publish_map();
  }

}

void LaserOctomapper::addProjectedScan(laser_projected_scan * lscan)
{
  lscans_to_be_processed.push_back(lscan);
}

LaserOctomapper::LaserOctomapper(char * logFname) :
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
  float res = .1;
  ocTree = new OcTree(res);

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

LaserOctomapper::~LaserOctomapper()
{
  delete ocTree;
  //TODO: don't leak everything else?
}

void LaserOctomapper::publish_map()
{ //TODO:maybe we should compress?
  fprintf(stderr, "Publishing map... ");
  double minX, minY, minZ, maxX, maxY, maxZ;
  ocTree->getMetricMin(minX, minY, minZ);
  ocTree->getMetricMax(maxX, maxY, maxZ);
  printf("\nmap bounds: [%.2f, %.2f, %.2f] - [%.2f, %.2f, %.2f]\n", minX, minY, minZ, maxX, maxY, maxZ);

  bot_core_raw_t msg;
  msg.utime = bot_timestamp_now();

  std::stringstream datastream;
  ocTree->writeBinaryConst(datastream);
  std::string datastring = datastream.str();
  msg.data = (uint8_t *) datastring.c_str();
  msg.length = datastring.size();
  bot_core_raw_t_publish(lcm_pub, "OCTOMAP", &msg);
  fprintf(stderr, "done! \n");
}

void LaserOctomapper::save_map()
{
  fprintf(stderr, "Saving map... ");
  ocTree->writeBinaryConst("octomap.bt");
  fprintf(stderr, "done! \n");
}

void LaserOctomapper::processScansInQueue()
{
  bot_tictoc("addLaser");
  list<laser_projected_scan *>::iterator it;
  for (it = lscans_to_be_processed.begin(); it != lscans_to_be_processed.end(); it++) {
    laser_projected_scan * lscan = *it;
    if (!lscan->projection_status) {
      laser_update_projected_scan(lscan->projector, lscan, "local");
    }
    if (!lscan->projection_status)
      break;
    else {
      bot_tictoc("processScan");
      octomap::point3d origin(lscan->origin.trans_vec[0], lscan->origin.trans_vec[1], lscan->origin.trans_vec[2]);
      for (int i = 0; i < lscan->npoints; i += (beam_skip + 1)) {
        double max_range = lscan->projector->max_range - 1.0;
        if (lscan->invalidPoints[i] > 1)
          continue;
        else if (lscan->invalidPoints[i] == 1) { //maxrange
          //          octomap::point3d endpoint(lscan->points[i].x, lscan->points[i].y, lscan->points[i].z);
          //    ocTree->insertRay(origin, endpoint, lscan->projector->max_range-10);
          //TODO:
        }
        else {
          octomap::point3d endpoint(lscan->points[i].x, lscan->points[i].y, lscan->points[i].z);
          ocTree->updateNode(endpoint, true); // integrate 'occupied' measurement
          //          ocTree->insertRay(origin, endpoint, lscan->projector->max_range);
        }

      }
      bot_tictoc("processScan");

      laser_destroy_projected_scan(lscan);
    }
  }
  if (it != lscans_to_be_processed.begin()) {
    lscans_to_be_processed.erase(lscans_to_be_processed.begin(), it); //destructor was already called in the loop
  }
  bot_tictoc("addLaser");
}

/**
 * Shutdown
 */
static LaserOctomapper * _map3d;
static void shutdown_module(int unused __attribute__((unused)))
{
  fprintf(stderr, "shutting down!\n");
  _map3d->save_map(); //TODO make an option?
  _map3d->publish_map();
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

  LaserOctomapper *map3d = new LaserOctomapper(logFName);
  _map3d = map3d;
  while (true) {
    int ret = lcm_handle(map3d->lcm_recv);
    if (ret != 0)
      break;//log is done...
  }
  shutdown_module(1);

}
