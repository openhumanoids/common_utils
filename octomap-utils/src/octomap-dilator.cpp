#include <ConciseArgs>
#include <octomap/octomap.h>
#include <sstream>

using namespace std;
using namespace octomap;

void save_map(OcTree * ocTree, const string & outFname)
{
  fprintf(stderr, "Saving map to %s...", outFname.c_str());
  ocTree->writeBinaryConst(outFname.c_str());
  fprintf(stderr, "done! \n");
}

int main(int argc, char **argv)
{
  double radius = .5;
  string octomap_fname = "octomap.bt";

  ConciseArgs opt(argc, argv);
  opt.add(radius, "r", "radius");
  opt.add(octomap_fname, "m", "map_name", "full path to map");
  opt.parse();

  cout << "Dilation Radius: " << radius << endl;

  stringstream s;
  s << octomap_fname << "_dilated";
  string dilated_fname = s.str();

  printf("loading octomap from: %s\n", octomap_fname.c_str());
  OcTree * ocTree = new OcTree(octomap_fname);

  double minX, minY, minZ, maxX, maxY, maxZ;
  ocTree->getMetricMin(minX, minY, minZ);
  ocTree->getMetricMax(maxX, maxY, maxZ);
  printf("\nmap bounds: [%.2f, %.2f, %.2f] - [%.2f, %.2f, %.2f]  res: %f\n", minX, minY, minZ, maxX, maxY, maxZ,
      ocTree->getResolution());

  printf("Creating Dilated Map\n");
  float res = ocTree->getResolution();
  OcTree *ocTree_dilated = new OcTree(res);

  int numLeaves = ocTree->getNumLeafNodes();
  int count = 0;
  for (OcTree::leaf_iterator it = ocTree->begin_leafs(), end = ocTree->end_leafs(); it != end; ++it) {
    if (count % (numLeaves / 20) == 0) {
      printf("%d of %d\n", count, numLeaves);
    }
    count++;

    point3d center = it.getCoordinate();
    // Loop through the cube surrounding the current leaf
    // If the given cell is within a sphere of radius specified above, then fill it
    // NOTE: CELLS ARE BEING RE-FILLED OVER AND OVER AGAIN BECAUSE THEY FALL WITHIN THE RADIUS OF MULTIPLE CELLS OF THE ORIGINAL MAP
    // PROBABLY A MUCH FASTER WAY TO DO THIS
    for (double x = -radius; x <= radius; x += res) {
      for (double y = -radius; y <= radius; y += res) {
        for (double z = -radius; z <= radius; z += res) {
          if (sqrt(x * x + y * y + z * z) <= radius) {
            point3d newpoint(center(0) + x, center(1) + y, center(2) + z);
            ocTree_dilated->updateNode(newpoint, true);
          }
        }
      }
    }
  }

  save_map(ocTree_dilated, dilated_fname);
  return 0;
}
