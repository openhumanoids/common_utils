
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arconf.h"
#include <common_c_utils/math_util.h>
//#include <carmen3d/lcm_utils.h>

#define CONF_LOCAL_FILENAME "local.cfg"
#define CONF_DEFAULT_FILENAME "quad.cfg"
#define CONF_LOCAL_FILEPATH CONFIG_DIR "/" CONF_LOCAL_FILENAME
#define CONF_DEFAULT_FILEPATH CONFIG_DIR "/" CONF_DEFAULT_FILENAME

//
//BotConf *
//arconf_parse_default (void)
//{
//    /* configuration file path:
//     *   - Use environmental variable AR_CONF_FILE as path if it is defined
//     *   - Otherwise, use CONF_LOCAL_FILEPATH as path if a file at
//     *     CONF_LOCAL_FILEPATH exists (i.e. openable for reading).
//     *   - Otherwise, use CONF_DEFAULT_FILEPATH as path
//     */
//
//    FILE * f = NULL;
//    char * path = getenv ("AR_CONF_FILE");
//    if (!path || !strlen(path)) {
//        path = CONF_LOCAL_FILEPATH;
//        f = fopen (path, "r");
//        if (!f)
//            path = CONF_DEFAULT_FILEPATH;
//
//    }
//
//    if (!f)
//        f = fopen (path, "r");
//    if (!f) {
//        fprintf (stderr, "Error: failed to open Agile config file:\n%s\n",
//                 path);
//        return NULL;
//    }
//
//    BotConf * c;
//    c = bot_conf_parse_file (f, path);
//    fclose (f);
//    return c;
//}

int
arconf_get_vehicle_footprint (BotConf *cfg, double fp[8])
{
    char *key_fl = "calibration.vehicle_bounds.front_left";
    char *key_fr = "calibration.vehicle_bounds.front_right";
    char *key_bl = "calibration.vehicle_bounds.rear_left";
    char *key_br = "calibration.vehicle_bounds.rear_right";

    if ((bot_conf_get_array_len (cfg, key_fl) != 2) ||
        (bot_conf_get_array_len (cfg, key_fr) != 2) ||
        (bot_conf_get_array_len (cfg, key_bl) != 2) ||
        (bot_conf_get_array_len (cfg, key_br) != 2)) {
        fprintf (stderr, "ERROR! invalid calibration.vehicle_bounds!\n");
        return -1;
    }

    bot_conf_get_double_array (cfg, key_fl, fp, 2);
    bot_conf_get_double_array (cfg, key_fr, fp+2, 2);
    bot_conf_get_double_array (cfg, key_br, fp+4, 2);
    bot_conf_get_double_array (cfg, key_bl, fp+6, 2);
    return 0;
}

//int
//arconf_get_rndf_absolute_path (BotConf * config, char *buf, int buf_size)
//{
//    char rndf_path[256];
//    char *rndf_file;
//    if (bot_conf_get_str (config, "rndf", &rndf_file) < 0)
//        return -1;
//    snprintf (rndf_path, sizeof (rndf_path), CONFIG_DIR"/%s", rndf_file);
//
//    if (strlen (rndf_path) > buf_size)
//        return -1;
//    strcpy (buf, rndf_path);
//    return 0;
//}


static int
get_str (BotConf *config, const char *key, char *result, int result_size)
{
    char *val = NULL;
    int status = bot_conf_get_str (config, key, &val);
    if (0 == status && result_size > strlen (val)) {
        strcpy (result, val);
        status = 0;
    } else {
        status = -1;
    }
    return status;
}

/* ================ cameras ============== */
int
arconf_get_camera_thumbnail_channel (BotConf *config,
                                     const char *camera_name, char *result, int result_size)
{
    char key[1024];
    snprintf (key, sizeof (key), "cameras.%s.thumbnail_channel", camera_name);
    return get_str (config, key, result, result_size);
}

char**
arconf_get_all_camera_names (BotConf *config)
{
    return bot_conf_get_subkeys (config, "cameras");
}

int
arconf_get_camera_calibration_config_prefix (BotConf *config,
                            const char *cam_name, char *result, int result_size)
{
    snprintf (result, result_size, "calibration.cameras.%s", cam_name);
    return bot_conf_has_key (config, result) ? 0 : -1;
}

BotCamTrans*
arconf_get_new_camtrans (BotConf *config, const char *cam_name)
{
    char prefix[1024];
    int status = arconf_get_camera_calibration_config_prefix (config, cam_name,
                                                       prefix, sizeof (prefix));
    if (0 != status) goto fail;

    char key[2048];
    double width;
    sprintf(key, "%s.width", prefix);
    if (0 != bot_conf_get_double(config, key, &width)) goto fail;

    double height;
    sprintf(key, "%s.height", prefix);
    if (0 != bot_conf_get_double(config, key, &height)) goto fail;

    double pinhole_params[5];
    snprintf(key, sizeof (key), "%s.pinhole", prefix);
    if (5 != bot_conf_get_double_array(config, key, pinhole_params, 5))
        goto fail;
    double fx = pinhole_params[0];
    double fy = pinhole_params[1];
    double cx = pinhole_params[3];
    double cy = pinhole_params[4];
    double skew = pinhole_params[2];

    double position[3];
    sprintf(key, "%s.position", prefix);
    if (3 != bot_conf_get_double_array(config, key, position, 3)) goto fail;

    sprintf (key, "cameras.%s", cam_name);
    double orientation[4];
    if (0 != arconf_get_quat (config, key, orientation)) goto fail;

    char *ref_frame;
    sprintf (key, "%s.relative_to", prefix);
    if (0 != bot_conf_get_str (config, key, &ref_frame)) goto fail;

    char * distortion_model;
    sprintf(key, "%s.distortion_model", prefix);
    if (0 != bot_conf_get_str (config, key, &distortion_model)) goto fail;

    if (strcmp(distortion_model,"spherical")==0){
      double distortion_param;
      sprintf(key, "%s.distortion_params", prefix);
      if (1 != bot_conf_get_double_array(config, key, &distortion_param, 1))
        goto fail;

      BotDistortionObj* sph_dist = bot_spherical_distortion_create (distortion_param);
      BotCamTrans* sph_camtrans = bot_camtrans_new (cam_name, width, height, fx, fy, cx, cy, skew,sph_dist);
      return sph_camtrans;
    }
    else if (strcmp(distortion_model,"plumb-bob")==0){
      double dist_k[3];
      sprintf(key, "%s.distortion_k", prefix);
      if (3 != bot_conf_get_double_array(config, key, dist_k, 3)) goto fail;

      double dist_p[2];
      sprintf(key, "%s.distortion_p", prefix);
      if (2 != bot_conf_get_double_array(config, key, dist_p, 2)) goto fail;

      BotDistortionObj* pb_dist = bot_plumb_bob_distortion_create (dist_k[0],dist_k[1],dist_k[2],dist_p[0],dist_p[1]);
      BotCamTrans* pb_camtrans = bot_camtrans_new (cam_name, width, height, fx, fy, cx, cy, skew,pb_dist);
      return pb_camtrans;
    }

 fail:
    return NULL;
}

int
arconf_cam_get_name_from_lcm_channel (BotConf *cfg, const char *channel,
                                      char *result, int result_size)
{
    int status = -1;

    memset (result, 0, result_size);
    char **cam_names = bot_conf_get_subkeys (cfg, "cameras");

    for (int i=0; cam_names && cam_names[i]; i++) {
        char *thumb_key =
            g_strdup_printf ("cameras.%s.thumbnail_channel", cam_names[i]);
        char *cam_thumb_str = NULL;
        int key_status = bot_conf_get_str (cfg, thumb_key, &cam_thumb_str);
        free (thumb_key);

        if ((0 == key_status) && (0 == strcmp (channel, cam_thumb_str))) {
            strcpy (result, cam_names[i]);
            status = 0;
            break;
        }

        char *full_key =
            g_strdup_printf ("cameras.%s.full_frame_channel", cam_names[i]);
        char *cam_full_str = NULL;
        key_status = bot_conf_get_str (cfg, full_key, &cam_full_str);
        free (full_key);

        if ((0 == key_status) && (0 == strcmp (channel, cam_full_str))) {
            strcpy (result, cam_names[i]);
            status = 0;
            break;
        }
    }
    g_strfreev (cam_names);

    return status;
}


/* ================ planar lidar ============== */
char**
arconf_get_all_planar_lidar_names (BotConf *config)
{
    return bot_conf_get_subkeys (config, "planar_lidars");
}

int
arconf_get_planar_lidar_config_path (BotConf *config, const char *plidar_name,
                                     char *result, int result_size)
{
    int n = snprintf (result, result_size, "planar_lidars.%s", plidar_name);
    if (n >= result_size)
        return -1;
    if (config)
        return bot_conf_has_key (config, result) ? 0 : -1;
    else
        return 0;
}



/* ================ general ============== */
int
_arconf_get_quat (BotConf *config, const char *name, double quat[4])
{
    char key[256];
    sprintf (key, "%s.orientation", name);
    if (bot_conf_has_key (config, key)) {
        int sz = bot_conf_get_double_array (config, key, quat, 4);
        assert (sz==4);
        return 0;
    }

    sprintf (key, "%s.rpy", name);
    if (bot_conf_has_key (config, key)) {
        double rpy[3];
        int sz = bot_conf_get_double_array (config, key, rpy, 3);
        assert (sz == 3);
        for (int i = 0; i < 3; i++)
            rpy[i] = to_radians (rpy[i]);
        bot_roll_pitch_yaw_to_quat (rpy, quat);
        return 0;
    }

    sprintf (key, "%s.rodrigues", name);
    if (bot_conf_has_key (config, key)) {
        double rod[3];
        int sz = bot_conf_get_double_array (config, key, rod, 3);
        assert (sz == 3);
        bot_rodrigues_to_quat(rod, quat);
        return 0;
    }

    sprintf (key, "%s.angleaxis", name);
    if (bot_conf_has_key (config, key)) {
        double aa[4];
        int sz = bot_conf_get_double_array (config, key, aa, 4);
        assert (sz==4);

        double theta = aa[0];
        double s = sin (theta/2);

        quat[0] = cos (theta/2);
        for (int i = 1; i < 4; i++)
            quat[i] = aa[i] * s;
        return 0;
    }
    return -1;
}

int
arconf_get_quat (BotConf *config, const char *name, double quat[4])
{
    // check if it's in calibration section
    char key[256];
    sprintf (key, "calibration.%s", name);
    if (!_arconf_get_quat (config, key, quat))
        return 0;

    // if not assume it was absolute path
    if (!_arconf_get_quat (config, name, quat))
        return 0;

    // not found
    return -1;
}

int
arconf_get_pos (BotConf *config, const char *name, double pos[3])
{
    // look in calibration section
    char key[256];
    sprintf (key, "calibration.%s.position", name);
    if (bot_conf_has_key (config, key)) {
        int sz = bot_conf_get_double_array (config, key, pos, 3);
        assert (sz==3);
        return 0;
    }
    // if not there look as absolute path
    sprintf (key, "%s.position", name);
    if (bot_conf_has_key (config, key)) {
        int sz = bot_conf_get_double_array (config, key, pos, 3);
        assert (sz==3);
        return 0;
    }
    // not found
    return -1;
}

int
arconf_get_matrix (BotConf *config, const char *name, double m[16])
{
    double quat[4];
    double pos[3];

    if (arconf_get_quat (config, name, quat))
        return -1;

    if (arconf_get_pos (config, name, pos))
        return -1;

    bot_quat_pos_to_matrix (quat, pos, m);

    return 0;
}


uint64_t
arconf_get_unique_id (void)
{
    return (0xefffffffffffffff&(bot_timestamp_now()<<8)) + 256*rand()/RAND_MAX;
}

int arconf_match_tag(BotConf *config, const char *tag, char *name, char *desc)
{
    fprintf(stderr, "["__FILE__":%d] ", __LINE__);
    fprintf(stderr, "arconf error: arconf_match_tag not yet implemented.\n");
    return FALSE;
}

int arconf_get_tag(BotConf *config, const char *tag, char *name, char *desc)
{
    char **tags = bot_conf_get_subkeys(config, "rfids");
    if (!tags)
    {
        fprintf(stderr, "arconf Error: Could not get rfids.\n");
        return FALSE;
    }

    char key[256];
    for (int i = 0; tags[i]; i++)
    {
        // The assumption here is that the tag passed is of the form:
        //
        //     *e[0-9]{7}b[0-9]{15}
        //
        // And since keys can't start with a '*' in the config file, they don't.
        // Take it out, THEN compare.
        if (!strcmp(&tag[1], tags[i]))
        {
            char *val = NULL;

            memset(key, 0, 256);
            sprintf(key, "rfids.%s.name", tags[i]);
            if (bot_conf_get_str(config, key, &val) == -1)
            {
                fprintf(stderr, "Error: Could not get key: %s\n", key);
                g_strfreev(tags);
                return FALSE;
            }
            strcpy(name, val);

            memset(key, 0, 256);
            sprintf(key, "rfids.%s.desc", tags[i]);
            if (bot_conf_get_str(config, key, &val) == -1)
            {
                fprintf(stderr, "Error: Could not get key: %s\n", key);
                g_strfreev(tags);
                return FALSE;
            }
            strcpy(desc, val);

            g_strfreev(tags);
            return TRUE;
        }
    }

    g_strfreev(tags);
    return FALSE;
}


static int
get_filename (BotConf *cfg, char *buf, int buf_size, const char *key,
        const char *prefix)
{
    char *fname;
    char full_fname[4096];
    if (bot_conf_get_str (cfg, key, &fname) < 0)
        return -1;
    snprintf (full_fname, sizeof (full_fname)-1, "%s%s", prefix, fname);
    if (strlen (full_fname) >= buf_size) return -1;
    strcpy (buf, full_fname);
    return 0;
}


//int
//arconf_get_gis_ortho_imagery_dir (BotConf *cfg, char *buf, int buf_size)
//{
//    char key[1024];
//    memset(key, 0, sizeof(key));
//    char *default_dataset = NULL;
//    if(bot_conf_get_str(cfg, "lanetruth.default_dataset", &default_dataset) < 0)
//        return -1;
//    snprintf(key, sizeof(key), "lanetruth.datasets.%s.ortho_imagery", default_dataset);
//    return get_filename(cfg, buf, buf_size, key, CONFIG_DIR"/../");
//}
