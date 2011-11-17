#ifndef LASERSIM3D_H_
#define LASERSIM3D_H_

#include <octomap/octomap.h>
#include <bot_core/bot_core.h>

namespace laser_util {

class LaserSim3D {
public:
  LaserSim3D(const octomap::OcTree * ocTree, int nranges, float rad0, float radstep, float max_range,
      int numHeightBeamsStart = 0, int numHeightBeamsEnd = 0);
  ~LaserSim3D();

  const octomap::OcTree * map;
  double * laserFramePoints;
  bot_core_planar_lidar_t * laser_msg;
  float laser_max_range;

  const bot_core_planar_lidar_t * simulate(BotTrans * curr_pose, int64_t utime = 0);
};

} /* namespace laser_util */
#endif /* LASERSIM_H_ */
