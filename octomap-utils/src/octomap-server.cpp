#include "octomap_util.hpp"
#include <iostream>
#include <sstream>
#include <lcmtypes/octomap_utils.h>

using namespace std;
using namespace octomap;
using namespace occ_map;

int main(int argc, char ** argv)
{

  if (argc <2) {
    printf("Usage:\n");
    printf("%s <octomap_fname> [repeat_period]\n", argv[0]);
    exit(1);
  }

  string octomap_fname = argv[1];

  double repeat_period = -1;
  if (argc > 2)
    repeat_period = atof(argv[2]);

  lcm_t * lcm = lcm_create(NULL);

  printf("loading octomap from: %s\n", octomap_fname.c_str());
  octomap::OcTree * ocTree = new OcTree(octomap_fname);

  double minX, minY, minZ, maxX, maxY, maxZ;
  ocTree->getMetricMin(minX, minY, minZ);
  ocTree->getMetricMax(maxX, maxY, maxZ);
  printf("\nmap bounds: [%.2f, %.2f, %.2f] - [%.2f, %.2f, %.2f]  res: %f\n", minX, minY, minZ, maxX, maxY, maxZ,
      ocTree->getResolution());

  octomap_raw_t msg;
  msg.utime = bot_timestamp_now();

  std::stringstream datastream;
  ocTree->writeBinaryConst(datastream);
  std::string datastring = datastream.str();
  msg.data = (uint8_t *) datastring.c_str();
  msg.length = datastring.size();

  octomap_raw_t_publish(lcm, "OCTOMAP", &msg);
  if (repeat_period > 0) {
    while (1) {
      usleep(1e6 * repeat_period);
      fprintf(stderr,".");
      octomap_raw_t_publish(lcm, "OCTOMAP", &msg);
    }
  }
  fprintf(stderr, "done! \n");

  return 0;
}

