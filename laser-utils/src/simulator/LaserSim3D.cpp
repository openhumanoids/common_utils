#include "LaserSim3D.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bot_core/bot_core.h>
#include <geom_utils/geometry.h>
#include <octomap/octomap.h>

using namespace std;
using namespace octomap;
using namespace laser_util;

const bot_core_planar_lidar_t * LaserSim3D::simulate(BotTrans * curr_pose, int64_t utime)
{
  bot_tictoc("publishLaser");

  //do the ray tracing
  double local_xyz[3];
  for (int i = 0; i < laser_msg->nranges; i++) {
    double * laser_xyz = laserFramePoints + 3 * i;
    bot_trans_apply_vec(curr_pose, laser_xyz, local_xyz);

    point3d origin(curr_pose->trans_vec[0], curr_pose->trans_vec[1], curr_pose->trans_vec[2]);
    point3d direction(local_xyz[0], local_xyz[1], local_xyz[2]);
    direction -= origin;
    point3d hitPoint;

    if (map->castRay(origin, direction, hitPoint, true, laser_max_range)) {
      laser_msg->ranges[i] = origin.distance(hitPoint); //TODO: resonable noise?
    }
    else {
      laser_msg->ranges[i] = laser_max_range;
    }
  }
  laser_msg->utime = utime;
  return laser_msg;

}

LaserSim3D::LaserSim3D(const octomap::OcTree * ocTree, int nranges, float rad0, float radstep, float max_range,
    int numHeightBeamsStart, int numHeightBeamsEnd)
{

  laser_msg = (bot_core_planar_lidar_t *) calloc(1, sizeof(bot_core_planar_lidar_t));
  laser_msg->nranges = nranges;
  laser_msg->ranges = (float *) calloc(nranges, sizeof(float));
  laser_msg->nintensities = 0;
  laser_msg->rad0 = rad0;
  laser_msg->radstep = radstep;
  laser_max_range = max_range;

  map = ocTree;

  laserFramePoints = (double *) calloc(3 * laser_msg->nranges, sizeof(double));
  for (int i = 0; i < laser_msg->nranges; i++) {
    double * laser_xyz = laserFramePoints + 3 * i;
    double theta = laser_msg->rad0 + laser_msg->radstep * i;
    laser_xyz[0] = laser_max_range * cos(theta);
    laser_xyz[1] = laser_max_range * sin(theta);
    laser_xyz[2] = 0;
  }

  float mirrorDist = .15;
  for (int i = 0; i < abs(numHeightBeamsStart); i++) {
    double * laser_xyz = laserFramePoints + 3 * i;
    double theta = laser_msg->rad0 + laser_msg->radstep * i;
    laser_xyz[0] = mirrorDist * cos(theta);
    laser_xyz[1] = mirrorDist * sin(theta);
    laser_xyz[2] = -bot_sgn(numHeightBeamsStart) * (laser_max_range - mirrorDist);
  }

  for (int i = laser_msg->nranges - abs(numHeightBeamsEnd); i < laser_msg->nranges; i++) {
    double * laser_xyz = laserFramePoints + 3 * i;
    double theta = laser_msg->rad0 + laser_msg->radstep * i;
    laser_xyz[0] = mirrorDist * cos(theta);
    laser_xyz[1] = mirrorDist * sin(theta);
    laser_xyz[2] = -bot_sgn(numHeightBeamsEnd) * (laser_max_range - mirrorDist);
  }

}

LaserSim3D::~LaserSim3D()
{

  free(laserFramePoints);
  bot_core_planar_lidar_t_destroy(laser_msg);
}
