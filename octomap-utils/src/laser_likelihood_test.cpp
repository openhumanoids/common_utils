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
#include <bot_lcmgl_client/lcmgl.h>

using namespace std;
using namespace octomap;
//TODO: make me a parameter
static int scan_skip = 2;
static int beam_skip = 3;
static double spatial_decimation = .25;

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
  bot_lcmgl_t * lcmgl;

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

  laser_projected_scan * lscan = laser_create_projected_scan_from_planar_lidar(proj, msg,"body");
  if (lscan != NULL
  )
    self->addProjectedScan(lscan);

  self->processScansInQueue();

}

void LaserLikelihooder::addProjectedScan(laser_projected_scan * lscan)
{
  lscans_to_be_processed.push_back(lscan);
}

LaserLikelihooder::LaserLikelihooder(char * octomapFname) :
    last_publish_time(-1)
{
  lcm_pub = bot_lcm_get_global(NULL);
//  if (logFname != NULL) {
//    fromLog = true;
//    char provider_buf[1024];
//    sprintf(provider_buf, "file://%s?speed=0", logFname);
//    lcm_recv = lcm_create(provider_buf);
//  }
//  else {
    printf("running mapping from LCM\n");
    lcm_recv = lcm_pub;

//  }
  param = bot_param_get_global(lcm_pub, 0);
  frames = bot_frames_get_global(lcm_pub, param);
  lcmgl = bot_lcmgl_init(lcm_pub, "laser_likelihooder");

  //  ocTree = new OcTree("/home/abachrac/stuff/pods_stuff/Quad/build/bin/octomap.bt");
  ocTree = new OcTree(octomapFname);
  ocTree->toMaxLikelihood();

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
      g_hash_table_insert(laser_projectors, strdup(channel_name),
          laser_projector_new(param, frames, planar_lidar_names[pind], 1));
      free(channel_name);
      break;
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
    //decimate the scan
    int lastAdd = -1e6;
    for (int i = 0; i < lscan->npoints; i++) {
      if (lscan->invalidPoints[i])
        continue;
      if ((i - lastAdd) > beam_skip
          || bot_vector_dist_3d(point3d_as_array(&lscan->points[i]), point3d_as_array(&lscan->points[lastAdd]))
              > spatial_decimation
          || bot_vector_dist_3d(point3d_as_array(&lscan->points[i]), lscan->origin.trans_vec)
              > (lscan->aveSurroundRange + 1.8 * lscan->stddevSurroundRange) || i < lscan->projector->surroundRegion[0]
          || i > lscan->projector->surroundRegion[1]) {
        lastAdd = i;
      }
      else {
        lscan->invalidPoints[i] = 3;
        lscan->numValidPoints--;
      }
    }
    bot_tictoc("processScan");
    BotTrans body_to_local;
    bot_frames_get_trans(frames, "body", bot_frames_get_root_name(frames), &body_to_local);
    double like1 = evaluateLaserLikelihood(ocTree, lscan, &body_to_local);
    int gridSizeXY = 21;
    int gridSizeZ = 1;
    double gridRes = .025;
    bot_lcmgl_point_size(lcmgl, 5);
    bot_lcmgl_enable(lcmgl, GL_DEPTH_TEST);
    bot_lcmgl_depth_func(lcmgl, GL_LESS);
    bot_lcmgl_begin(lcmgl, GL_POINTS);
    for (int i = 0; i < gridSizeXY; i++) {
      for (int j = 0; j < gridSizeXY; j++) {
        for (int k = 0; k < gridSizeZ; k++) {
          BotTrans tmp = body_to_local;
          tmp.trans_vec[0] += gridRes * (i - gridSizeXY / 2);
          tmp.trans_vec[1] += gridRes * (j - gridSizeXY / 2);
          tmp.trans_vec[2] += gridRes * (k - gridSizeZ / 2);
          double like = evaluateLaserLikelihood(ocTree, lscan, &tmp);
          bot_lcmgl_vertex3f(lcmgl, tmp.trans_vec[0], tmp.trans_vec[1], tmp.trans_vec[2]);
          float * color = bot_color_util_jet(like / (lscan->numValidPoints * 3.5));
          bot_lcmgl_color3f(lcmgl, color[0], color[1], color[2]);
        }
      }
    }
    bot_lcmgl_end(lcmgl);

    gridSizeXY = 1;
    gridSizeZ = 21;
    bot_lcmgl_begin(lcmgl, GL_POINTS);
    for (int i = 0; i < gridSizeXY; i++) {
      for (int j = 0; j < gridSizeZ; j++) {
        for (int k = 0; k < gridSizeZ; k++) {
          BotTrans tmp = body_to_local;
          tmp.trans_vec[0] += gridRes * (i - gridSizeXY / 2);
          tmp.trans_vec[1] += gridRes * (j - gridSizeZ / 2);
          tmp.trans_vec[2] += gridRes * (k - gridSizeZ / 2);
          double like = evaluateLaserLikelihood(ocTree, lscan, &tmp);
          bot_lcmgl_vertex3f(lcmgl, tmp.trans_vec[0], tmp.trans_vec[1], tmp.trans_vec[2]);
          float * color = bot_color_util_jet(like / (lscan->numValidPoints * 3.5));
          bot_lcmgl_color3f(lcmgl, color[0], color[1], color[2]);
        }
      }
    }
    bot_lcmgl_end(lcmgl);

    double like = evaluateLaserLikelihood(ocTree, lscan, &body_to_local);
    printf("%d scans, like=%f ", lscan->numValidPoints, like);
    bot_trans_print_trans(&body_to_local);
    printf("\n");
    bot_lcmgl_point_size(lcmgl, 10);
    bot_lcmgl_color3f(lcmgl, bot_color_util_yellow[0], bot_color_util_yellow[1], bot_color_util_yellow[2]);
    bot_lcmgl_begin(lcmgl, GL_POINTS);

    for (int i=0;i<lscan->npoints;i++){
       if (lscan->invalidPoints[i]!=0)
         continue;
       double proj_xyz[3];
       double llike = getOctomapLogLikelihood(ocTree,proj_xyz);
       lcmglColor3fv(bot_color_util_jet(llike/3.511031));
       bot_trans_apply_vec(&body_to_local,point3d_as_array(&lscan->points[i]),proj_xyz);
       bot_lcmgl_vertex3f(lcmgl, proj_xyz[0], proj_xyz[1], proj_xyz[2]);

    }

    bot_lcmgl_end(lcmgl);
    bot_lcmgl_disable(lcmgl, GL_DEPTH_TEST);
    bot_lcmgl_switch_buffer(lcmgl);

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
  char * octomapFname = NULL;
  if (argc > 1)
    octomapFname = argv[1];

  LaserLikelihooder *map3d = new LaserLikelihooder(octomapFname);
  _map3d = map3d;
  while (true) {
    int ret = lcm_handle(map3d->lcm_recv);
    if (ret != 0)
      break; //log is done...
  }
  shutdown_module(1);

}
