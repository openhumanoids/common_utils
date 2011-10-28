#ifndef LASERSIM_H_
#define LASERSIM_H_

#include <occ_map/PixelMap.hpp>
#include <bot_core/bot_core.h>

namespace laser_util {

class LaserSim {
public:
  LaserSim(occ_map::FloatPixelMap * map, int nranges, float rad0, float radstep, float max_range);
  ~LaserSim();
  bool isNearMapBorder(const double location[2], double range);
  bool getMapBorderInstersection(const double P0[2], const double P1[2], double intersect[2]);

  occ_map::FloatPixelMap * map;
  float occupancy_thresh;

  double * laserFramePoints;
  bot_core_planar_lidar_t laser_msg;
  float laser_max_range;

  bot_core_planar_lidar_t * simulate(BotTrans * curr_pose);

};

} /* namespace laser_util */
#endif /* LASERSIM_H_ */
