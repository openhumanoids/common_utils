#include <stdlib.h>
#include <list>
#include "octomap_util.hpp"
#include <laser_utils/laser_util.h>

using namespace std;
using namespace octomap;
using namespace occ_map;

occ_map::FloatVoxelMap * octomapToVoxelMap(octomap::OcTree * ocTree, int occupied_depth, int free_depth)
{
  ocTree->toMaxLikelihood(); //lets not care about likelihoods
  double xyz0[3];
  double xyz1[3];
  ocTree->getMetricMin(xyz0[0], xyz0[1], xyz0[2]);
  ocTree->getMetricMax(xyz1[0], xyz1[1], xyz1[2]);

  double resolution = ocTree->getResolution();
  double mpp[3] = { resolution, resolution, resolution };
  FloatVoxelMap *voxMap = new FloatVoxelMap(xyz0, xyz1, mpp, .5);

  std::list<octomap::OcTreeVolume> occupiedVoxels;
  ocTree->getOccupied(occupiedVoxels, occupied_depth);
  list<octomap::OcTreeVolume>::iterator it;
  //mark the free ones
  double xyz[3];
  for (it = occupiedVoxels.begin(); it != occupiedVoxels.end(); it++) {
    double cxyz[3] = { it->first.x(), it->first.y(), it->first.z() };
    double side_length = it->second;
    int side_cells = side_length / resolution;
    if (side_cells > 1)
      int foo = side_length + 2;
    for (int i = 0; i < side_cells; i++) {
      xyz[0] = cxyz[0] - side_length / 2 + i * resolution;
      for (int j = 0; j < side_cells; j++) {
        xyz[1] = cxyz[1] - side_length / 2 + j * resolution;
        for (int k = 0; k < side_cells; k++) {
          xyz[2] = cxyz[2] - side_length / 2 + k * resolution;
          voxMap->writeValue(xyz, 1);
        }
      }
    }
  }
  occupiedVoxels.clear();

  std::list<octomap::OcTreeVolume> freeVoxels;
  ocTree->getFreespace(freeVoxels, free_depth);
  for (it = freeVoxels.begin(); it != freeVoxels.end(); it++) {
    double cxyz[3] = { it->first.x(), it->first.y(), it->first.z() };
    double side_length = it->second;
    int side_cells = side_length / resolution;
    if (side_cells > 1)
      int foo = side_length + 2;
    for (int i = 0; i < side_cells; i++) {
      xyz[0] = cxyz[0] - side_length / 2 + i * resolution;
      for (int j = 0; j < side_cells; j++) {
        xyz[1] = cxyz[1] - side_length / 2 + j * resolution;
        for (int k = 0; k < side_cells; k++) {
          xyz[2] = cxyz[2] - side_length / 2 + k * resolution;
          voxMap->writeValue(xyz, 0);
        }
      }
    }
  }

  memset(xyz, 0, 3 * sizeof(double));
  int ixyz[3];
  voxMap->worldToTable(xyz, ixyz);
  for (ixyz[2] = 0; ixyz[2] < voxMap->dimensions[2]; ixyz[2]++) {
    float v = voxMap->readValue(ixyz);
    voxMap->tableToWorld(ixyz, xyz);
    printf(" %f", v);
  }
  printf("\n");
  xyz[0] = -100;
  xyz[1] = 100;
  xyz[2] = 0;
  voxMap->worldToTable(xyz, ixyz);
  for (ixyz[2] = 0; ixyz[2] < voxMap->dimensions[2]; ixyz[2]++) {
    float v = voxMap->readValue(ixyz);
    voxMap->tableToWorld(ixyz, xyz);
    printf(" %f", v);
  }
  return voxMap;
}

double evaluateLaserLikelihood(octomap::OcTree *oc, const laser_projected_scan * lscan, const BotTrans * trans)
{

  double likelihood = 0;
  for (int i = 0; i < lscan->npoints; i++) {
    if (lscan->invalidPoints[i] != 0)
      continue;
    double proj_xyz[3];
    bot_trans_apply_vec(trans, point3d_as_array(&lscan->points[i]), proj_xyz);
    likelihood += getOctomapLogLikelihood(oc, proj_xyz);
  }

  double percent_hit = likelihood / (OCMAP_BIN_VAL * (double) lscan->numValidPoints);

  double abes_magic_exponent = 2.0;
  double abe_hack = pow(percent_hit , abes_magic_exponent);

  return log(abe_hack);
}
