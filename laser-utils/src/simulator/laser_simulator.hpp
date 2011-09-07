#ifndef LASER_SIMULATOR_H_
#define LASER_SIMULATOR_H_
#include <string>
#include <occ_map/PixelMap.hpp>

#include <bot_param/param_client.h>
#include <bot_frames/bot_frames.h>


class LaserSimulator {
public:
  LaserSimulator(lcm_t * _lcm, BotParam * _param, BotFrames * _frames, std::string _laser_name, std::string mapFname);
  ~LaserSimulator();


  BotParam *param;
  BotFrames *frames;
  lcm_t *lcm;

  float publish_freq;
  int64_t last_print_time;

  occ_map::FloatPixelMap * map;
  float   occupancy_thresh;

  int64_t last_publish_time;

  char * laser_name;
  char * laser_channel;

  double * laserFramePoints;
  bot_core_planar_lidar_t laser_msg;
  float laser_max_range;

  void PublishLaser();
  bool isNearMapBorder(const double location[2],double range);
  bool getMapBorderInstersection(const double P0[2],const double P1[2], double intersect[2]);

};

#endif /* LASER_SIMULATOR_H_ */
