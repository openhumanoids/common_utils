#include "laser_simulator.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bot_core/bot_core.h>
#include <bot_param/param_util.h>
#include <geom_utils/geometry.h>

using namespace std;
using namespace occ_map;

static gboolean pub_handler(gpointer userdata)
{

  LaserSimulator * sim = (LaserSimulator *) userdata;
  sim->PublishLaser();
  return TRUE;
}

bool LaserSimulator::isNearMapBorder(const double location[2], double range)
{
  return location[0] - map->xy0[0] < range ||
      map->xy1[0] - location[0] < range ||
      location[1] - map->xy0[1] < range ||
      map->xy1[1] - location[1] < range;

}

bool LaserSimulator::getMapBorderInstersection(const double P0[2], const double P1[2], double intersect[2])
{

  double P2[2], P3[2];
  P2[0] = map->xy0[0];
  P2[1] = map->xy0[1];

  P3[0] = map->xy0[0];
  P3[1] = map->xy1[1];
  if (geom_line_seg_line_seg_intersect_2d(POINT2D(P0), POINT2D(P1), POINT2D(P2), POINT2D(P3), POINT2D(intersect)))
    return true;
  P3[0] = map->xy1[0];
  P3[1] = map->xy0[1];
  if (geom_line_seg_line_seg_intersect_2d(POINT2D(P0), POINT2D(P1), POINT2D(P2), POINT2D(P3), POINT2D(intersect)))
    return true;

  P2[0] = map->xy1[0];
  P2[1] = map->xy1[1];

  P3[0] = map->xy0[0];
  P3[1] = map->xy1[1];
  if (geom_line_seg_line_seg_intersect_2d(POINT2D(P0), POINT2D(P1), POINT2D(P2), POINT2D(P3), POINT2D(intersect)))
    return true;
  P3[0] = map->xy1[0];
  P3[1] = map->xy0[1];
  if (geom_line_seg_line_seg_intersect_2d(POINT2D(P0), POINT2D(P1), POINT2D(P2), POINT2D(P3), POINT2D(intersect)))
    return true;

  return false;

}

void LaserSimulator::PublishLaser()
{
  bot_tictoc("publishLaser");
  //get the current location of the lidar
  BotTrans curr_pose;
  bot_frames_get_trans(frames, laser_name, bot_frames_get_root_name(frames), &curr_pose);

  bool nearMapBorder = isNearMapBorder(curr_pose.trans_vec, laser_max_range);

  //do the ray tracing
  double local_xyz[3];
  double hitPoint[2];
  double s, c;
  for (int i = 0; i < laser_msg.nranges; i++) {
    double * laser_xyz = laserFramePoints + 3 * i;
    bot_trans_apply_vec(&curr_pose, laser_xyz, local_xyz);

    double borderInstersect[2];
    if (nearMapBorder && getMapBorderInstersection(curr_pose.trans_vec, local_xyz, borderInstersect)) {
      // crop beam to edge of the map
      local_xyz[0] = borderInstersect[0];
      local_xyz[1] = borderInstersect[1];
    }

    if (map->collisionCheck(curr_pose.trans_vec, local_xyz, occupancy_thresh, hitPoint)) {
      laser_msg.ranges[i] = bot_vector_dist_2d(curr_pose.trans_vec, hitPoint) + bot_gauss_rand(0, .004); //TODO: add noise
    }
    else {
      laser_msg.ranges[i] = laser_max_range;
    }
  }

  bot_frames_get_trans_latest_timestamp(frames, laser_name, bot_frames_get_root_name(frames), &laser_msg.utime);
  bot_core_planar_lidar_t_publish(lcm, laser_channel, &laser_msg);
  bot_tictoc("publishLaser");

  int now = bot_timestamp_now();
  if (now - last_print_time > 1e6) {
    fprintf(stderr, ".");
    last_print_time = now;
  }

}

LaserSimulator::LaserSimulator(lcm_t * _lcm, BotParam * _param, BotFrames * _frames, string mapFname,
    string _laser_name) :
    lcm(_lcm), param(_param), frames(_frames), last_print_time(-1)
{
  publish_freq = 40.0;

  laser_name = strdup(_laser_name.c_str());
  laser_channel = bot_param_get_planar_lidar_lcm_channel(param, laser_name);

  //TODO: remove hardcoded parameters for a hokuyo UTM
  laser_msg.nranges = 1081;
  laser_msg.ranges = new float[1081];
  laser_msg.nintensities = 0;
  laser_msg.rad0 = -0.75 * M_PI;
  laser_msg.radstep = bot_to_radians(0.25);
  laser_max_range = 40.0;

  map = new FloatPixelMap(mapFname.c_str());
  occupancy_thresh = .85; //TODO:

  laserFramePoints = (double *) calloc(3 * laser_msg.nranges, sizeof(double));
  for (int i = 0; i < laser_msg.nranges; i++) {
    double * laser_xyz = laserFramePoints + 3 * i;
    double theta = laser_msg.rad0 + laser_msg.radstep * i;
    laser_xyz[0] = laser_max_range * cos(theta);
    laser_xyz[1] = laser_max_range * sin(theta);
    laser_xyz[2] = 0;
  }

  g_timeout_add((guint) 1.0 / publish_freq * 1000, pub_handler, this);

}

void usage(char * name)
{
  fprintf(stderr, "usage: %s map-filename [lidar-name]\n", name);
  fprintf(stderr, " assumes the coordinate frames are all set up in the bot-param-server,\n");
  fprintf(stderr, " and that the pose of the laser will be updated in BotFrames\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  string map_fname;
  string laser_name = "laser";

  if (argc < 2 || argc > 3)
    usage(argv[0]);
  map_fname = argv[1];
  if (argc > 2)
    laser_name = argv[2];

  GMainLoop * mainloop = g_main_loop_new(NULL, FALSE);

  lcm_t * lcm = bot_lcm_get_global(NULL);
  bot_glib_mainloop_attach_lcm(lcm);
  bot_signal_pipe_glib_quit_on_kill(mainloop);
  BotParam * param = bot_param_get_global(lcm, 0);
  BotFrames * frames = bot_frames_get_global(lcm, param);

  fprintf(stderr, "Publishing the simulated lider '%s'\n", laser_name.c_str());

  LaserSimulator *sim = new LaserSimulator(lcm, param, frames, map_fname, laser_name);

  // run
  g_main_loop_run(mainloop);

  //shutdown
  fprintf(stderr, "Shutting Down\n");
  bot_tictoc_print_stats(BOT_TICTOC_AVG);

}
