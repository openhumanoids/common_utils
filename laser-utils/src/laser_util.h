/*
 * laser_utils.h
 *
 *  Created on: Jun 27, 2009
 *      Author: abachrac
 */

#ifndef LASER_UTILS_H_
#define LASER_UTILS_H_

#include <bot_core/bot_core.h>
#include <bot_param/param_client.h>
#include <bot_frames/bot_frames.h>
#include <geom_utils/geometry.h>


#ifdef __cplusplus
extern "C" {
#endif

  /*
   * structure that holds parameters needed for projecting a laser scan
   */
  typedef struct {
    BotFrames * bot_frames;
    BotParam *param;

    char * laser_name;
    char * coord_frame;

    double max_range; /* max sensor range (from param file) */
    double min_range; /* min sensor range (from param file) */

    int heightDownRegion[2]; /*begining and end of beams deflected down by mirror (from param)*/
    int heightUpRegion[2]; /*begining and end of beams deflected down by mirror (from param)*/
    int surroundRegion[2]; /*begining and end of beams not deflected by mirror (from param)*/
    int project_height;
    double laser_frequency; /*frequency with which we should be expecting the laser msgs*/
    char * laser_type;
  } Laser_projector;

  /*
   * structure that a projected laser scan
   */
  typedef struct _laser_projected_scan {
    Laser_projector * projector;
    bot_core_planar_lidar_t * rawScan; /*copy of original scan message*/

    BotTrans body;    /* local frame body pose */
    BotTrans origin;  /* scan origin relative to requested dest frame*/
    point3d_t *points; /* points relative to requested dest frame */
    uint8_t *invalidPoints; /* 0 if the point is good; 1 if max_range; 2 if min_range; 3 otherwise */
    int npoints; /* total number of points in arrays above */
    int numValidPoints; /*number of points that are valid*/

    int projection_status; /*1 if scan was projected using a valid transform, 0 otherwise (timestamp off end of tranform buffer)*/

    double aveSurroundRange; /*average range of Surround points */
    double stddevSurroundRange; /*std-dev of the range of Surround points */

    int64_t utime; /* time when data associated w/ this scan was received */
  } laser_projected_scan;

  /*
   * create a new structure that holds all the necessary info to project the
   * laser ranges from this laser
   * it takes care of max-range, min-range, surround region etc...
   */
  Laser_projector * laser_projector_new(BotParam *param, BotFrames * frames, const char * laser_name, int project_height);
  void laser_projector_destroy(Laser_projector * to_destroy);

  /*
   * take a planar_lidar_t message, and project it into frame %dest_frame for rendering/whatever...
   */
  laser_projected_scan *laser_create_projected_scan_from_planar_lidar(Laser_projector * projector,
      const bot_core_planar_lidar_t *msg, const char * dest_frame);

  /*
   * update the scan with the current transform...
   */
  int laser_update_projected_scan(Laser_projector * projector, laser_projected_scan * proj_scan, const char * dest_frame);


  void laser_destroy_projected_scan(laser_projected_scan * proj_scan);

#ifdef __cplusplus
}
#endif

#endif /* LASER_UTILS_H_ */
