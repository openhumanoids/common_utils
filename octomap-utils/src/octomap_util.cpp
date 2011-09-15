#include <stdlib.h>
#include <list>
#include "octomap_util.hpp"
#include <laser_utils/laser_util.h>

using namespace std;
using namespace occ_map;

namespace octomap {

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

double evaluateLaserLogLikelihood(octomap::OcTree *oc, const laser_projected_scan * lscan, const BotTrans * trans)
{

  double logLike = 0;
  for (int i = 0; i < lscan->npoints; i++) {
    if (lscan->invalidPoints[i] != 0)
      continue;
    double proj_xyz[3];
    bot_trans_apply_vec(trans, point3d_as_array(&lscan->points[i]), proj_xyz);
    logLike += getOctomapLogLikelihood(oc, proj_xyz);
  }

  return logLike;
}

octomap::OcTree * octomapBlur(octomap::OcTree * ocTree, double blurSigma)
{

  float res = ocTree->getResolution();

  //normalize sigma from meteres to cell resolution
  blurSigma = blurSigma;
  double blurVar = blurSigma * blurSigma;
  //compute the size of the gaussian kernel assuming max likelihood of 255, and min of 32
  double det_var = pow(blurVar, 3); //covariance is diagonal
  double normalizer = pow(2 * M_PI,-3 / 2) * pow(det_var, -1 / 2);
  int sz = 0;
  float val = 1;
  while (val > .1) {
    sz++;
    double d = sz * res;
    val = normalizer * exp(-.5 * (d * d + d * d + d * d) / blurVar);
  }
  int kernel_size = 2 * sz;

  printf("kernel size is %d\n", kernel_size);

  //create the gaussian kernel
  double kxzy0[3] = { -res * kernel_size, -res * kernel_size, -res * kernel_size };
  double kxzy1[3] = { res * kernel_size, res * kernel_size, res * kernel_size };
  double krez[3] = { res, res, res };
  FloatVoxelMap * blurKernel = new FloatVoxelMap(kxzy0, kxzy1, krez);
  VoxelMap<point3d> * indexTable = new VoxelMap<point3d>(kxzy0, kxzy1, krez);
  double xyz[3];
  int ixyz[3];
  double kernel_sum = 0;
  for (ixyz[2] = 0; ixyz[2] < blurKernel->dimensions[2]; ixyz[2]++) {
    for (ixyz[1] = 0; ixyz[1] < blurKernel->dimensions[1]; ixyz[1]++) {
      for (ixyz[0] = 0; ixyz[0] < blurKernel->dimensions[0]; ixyz[0]++) {
        blurKernel->tableToWorld(ixyz, xyz);
        double val = normalizer * exp(-.5 * (bot_sq(xyz[0]) + bot_sq(xyz[1]) + bot_sq(xyz[2])) / blurVar); //diagonal cov
        kernel_sum += val;
        blurKernel->writeValue(ixyz, val);
        indexTable->writeValue(ixyz, point3d(xyz[0], xyz[1], xyz[2]));
      }
    }
  }

  printf("kernel_sum = %f\n", kernel_sum);

  for (int i = 0; i < blurKernel->num_cells; i++) {
    blurKernel->data[i] /= kernel_sum;
  }

  xyz[0] = xyz[1] = xyz[2] = 0;
  blurKernel->worldToTable(xyz, ixyz);
  fprintf(stderr, "kernel = [\n");
  for (ixyz[1] = 0; ixyz[1] < blurKernel->dimensions[1]; ixyz[1]++) {
    for (ixyz[0] = 0; ixyz[0] < blurKernel->dimensions[0]; ixyz[0]++) {
      fprintf(stderr, "%f ", blurKernel->readValue(ixyz));
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "];\n");


  printf("Creating Blurred Map\n");
  octomap::OcTree *ocTree_blurred = new OcTree(res);
  //set blurred map to occupancy probablity
  int numLeaves = ocTree->getNumLeafNodes();
  int count = 0;
  for (octomap::OcTree::leaf_iterator it = ocTree->begin_leafs(),
      end = ocTree->end_leafs(); it != end; ++it)
  {
    if (count % (numLeaves / 20) == 0) {
      printf("%d of %d\n", count, numLeaves);
    }
    count++;

    point3d center = it.getCoordinate();
    for (int i = 0; i < blurKernel->num_cells; i++) {
      OcTreeKey key;
      if (!ocTree_blurred->genKey(center + indexTable->data[i], key)) {
        fprintf(stderr, "Error: couldn't generate key in blurred map!\n");
      }
      octomap::OcTreeNode* leaf = ocTree_blurred->updateNode(key, blurKernel->data[i], true);
    }
  }

  printf("Updating inner occupancy!\n");
  ocTree_blurred->updateInnerOccupancy();

  //convert to log odds
  int numBlurLeaves = ocTree_blurred->getNumLeafNodes();
  printf("Converting to Log Odds\n");
  count = 0;
  for (octomap::OcTree::leaf_iterator it = ocTree_blurred->begin_leafs(),
      end = ocTree_blurred->end_leafs(); it != end; ++it)
  {
    octomap::OcTreeNode &node = *it;
    node.setValue(-log(node.getValue()));
  }

  return ocTree_blurred;
}

}
