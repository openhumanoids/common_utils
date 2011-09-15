#include "octomap_util.hpp"

using namespace std;
using namespace octomap;
using namespace occ_map;

int main(int argc, char ** argv)
{

  if (argc != 3) {
    printf("Usage:\n");
    printf("%s <octomap_fname> <blur_radius>\n", argv[0]);
    exit(1);
  }

  string octomap_fname = argv[1];
  double blur_sigma = atof(argv[2]);
  string blurred_fname = octomap_fname + string("_blurred");

  printf("loading octomap from: %s\n", octomap_fname.c_str());
  octomap::OcTree * ocTree = new OcTree(octomap_fname);

  ocTree->toMaxLikelihood();
  ocTree->expand();

  double minX, minY, minZ, maxX, maxY, maxZ;
  ocTree->getMetricMin(minX, minY, minZ);
  ocTree->getMetricMax(maxX, maxY, maxZ);
  printf("\nmap bounds: [%.2f, %.2f, %.2f] - [%.2f, %.2f, %.2f]  res: %f\n", minX, minY, minZ, maxX, maxY, maxZ,ocTree->getResolution());

  printf("blurring octomap\n");
  octomap::OcTree * ocTree_blurred = octomapBlur(ocTree, blur_sigma);
  ocTree_blurred->prune();
  printf("Saving blurred map to: %s\n", blurred_fname.c_str());
  ocTree_blurred->writeBinaryConst(blurred_fname);
  return 0;
}
