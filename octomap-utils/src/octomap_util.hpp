#ifndef __octomap_util_h__
#define __octomap_util_h__

#include <occ_map/VoxelMap.hpp>
#include <octomap/octomap.h>
#include <laser_utils/laser_util.h>

#define OCMAP_BIN_VAL 3.511031

namespace octomap {

occ_map::FloatVoxelMap * octomapToVoxelMap(octomap::OcTree * ocTree, int occupied_depth, int free_depth);

octomap::OcTree * octomapBlur(octomap::OcTree * ocTree, double blurSigma);

double evaluateLaserLikelihood(octomap::OcTree *oc, const laser_projected_scan * lscan, const BotTrans * trans);

static inline double getOctomapLogLikelihood(octomap::OcTree *oc, double xyz[3])
{
  octomap::OcTreeNode* node = oc->search(xyz[0], xyz[1], xyz[2]);
  if (node != NULL) {
    return node->getLogOdds();
  }
  else {
    return 0;
  }
}

}

#endif
