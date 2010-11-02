#ifndef __ARCONF_H__
#define __ARCONF_H__

#include <bot_core/bot_core.h>
//#include <bot_core/core/lcmtypes/botlcm_pose_t.h>



#ifdef __cplusplus
extern "C" {
#endif

BotConf * arconf_parse_default (void);
int arconf_get_vehicle_footprint (BotConf *cfg, double fp[8]);
int arconf_get_rndf_absolute_path (BotConf * config, char *buf, int buf_size);


int arconf_get_camera_thumbnail_channel (BotConf *config,
                                         const char *camera_name, char *result, int result_size);
char** arconf_get_all_camera_names (BotConf *config);
int arconf_get_camera_calibration_config_prefix (BotConf *config,
                           const char *cam_name, char *result, int result_size);
BotCamTrans* arconf_get_new_camtrans (BotConf *config, const char *cam_name);
int arconf_cam_get_name_from_lcm_channel (BotConf *config,
                                          const char *channel, char *result, int result_size);


char** arconf_get_all_planar_lidar_names (BotConf *config);
int arconf_get_planar_lidar_config_path (BotConf *config,
                        const char *plidar_name, char *result, int result_size);



int arconf_get_quat (BotConf * config, const char *name, double quat[4]);
int arconf_get_pos (BotConf * config, const char *name, double pos[3]);
int arconf_get_matrix (BotConf * config, const char *name, double m[16]);

uint64_t arconf_get_unique_id ();

/** arconf_match_tag:
 * Searches for a tag that ends in the passed string.  This is very similar to
 * arconf_get_tag().  It differs only in that, if you pass it a tag that's not
 * long enough, it will try to match it as the end of the tag.
 *
 * @config: The config object.
 * @tag:  RFID tag as in an arlcm_rfid_tag_t.tag message.
 * @name: The short (readable) name of the tag (already allocated).
 * @desc: The (longer) description of the tag (already allocated).
 * Returns: TRUE on success, FALSE otherwise.
 */
// TODO test and make it work.
//int arconf_match_tag(BotConf * config, const char *tag, char *name, char *desc);

/** arconf_get_tag:
 * Searches for the passed tag.
 *
 * @config: The config object.
 * @tag:  RFID tag as in an arlcm_rfid_tag_t.tag message.
 * @name: The short (readable) name of the tag (already allocated).
 * @desc: The (longer) description of the tag (already allocated).
 * Returns: TRUE on success, FALSE otherwise.
 */
int arconf_get_tag(BotConf *config, const char *tag, char *name, char *desc);
int arconf_get_gis_ortho_imagery_dir (BotConf *cfg, char *buf, int buf_size);


#ifdef __cplusplus
}
#endif

#endif
