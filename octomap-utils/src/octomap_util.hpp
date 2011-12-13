#ifndef __octomap_util_h__
#define __octomap_util_h__

#include <occ_map/VoxelMap.hpp>
#include <octomap/octomap.h>
#include <laser_utils/laser_util.h>
#include <bot_core/bot_core.h>




namespace octomap {
static const float LOGLIKE_HITS_EMPTY = -12;

//our own save/load
octomap::OcTree * loadOctomap(const char * fname, double *minNegLogLike);
void saveOctomap(octomap::OcTree *ocTree, const char * fname, double minNegLogLike);




occ_map::FloatVoxelMap * octomapToVoxelMap(octomap::OcTree * ocTree, int occupied_depth, int free_depth);

octomap::OcTree * octomapBlur(octomap::OcTree * ocTree, double blurSigma, double * minNegLogLike);

double evaluateLaserLogLikelihood(octomap::OcTree *oc, const laser_projected_scan * lscan, const BotTrans * trans, double minNegLogLike);

static inline double getOctomapLogLikelihood(octomap::OcTree *oc, double xyz[3])
{
  //check map bounds :-/
  double minxyz[3];
  oc->getMetricMin(minxyz[0], minxyz[1], minxyz[2]);
  if (xyz[0] < minxyz[0] || xyz[1] < minxyz[1] || xyz[2] < minxyz[2])
    return LOGLIKE_HITS_EMPTY;

  double maxxyz[3];
  oc->getMetricMax(maxxyz[0], maxxyz[1], maxxyz[2]);
  if (xyz[0] > maxxyz[0] || xyz[1] > maxxyz[1] || xyz[2] > maxxyz[2])
    return LOGLIKE_HITS_EMPTY;

  octomap::OcTreeNode* node = oc->search(xyz[0], xyz[1], xyz[2]);
  if (node != NULL) {
    return -node->getLogOdds();
  }
  else {
    return LOGLIKE_HITS_EMPTY;
  }
}

}

#endif
