#include "octomap_util.hpp"
#include <iostream>
#include <sstream>
#include <lcmtypes/octomap_utils.h>
#include <stl_utils/stl_opt_parse.hpp>

using namespace std;
using namespace octomap;
using namespace occ_map;

int main(int argc, char ** argv)
{

  stl_utils::OptParse opt_parse(argc, argv, "map_file_name", "loads an octomap and publishes it to lcm");

  double repeat_period = -1;
  bool blurred_map = false;

  opt_parse.add(repeat_period, "r", "repeat", "repeat period for lcm publishing, negative only publish once", false);
  opt_parse.add(blurred_map, "b", "blurred",
      "map file contains a blurred map, load with loadOctomap instead of default", false);

  std::list<string> remaining = opt_parse.parse();

  if (remaining.size() != 1)
    opt_parse.usage(true);

  string octomap_fname = remaining.front();

  lcm_t * lcm = lcm_create(NULL);

  printf("loading octomap from: %s\n", octomap_fname.c_str());

  octomap::OcTree * ocTree;
  if (blurred_map) {
    double min_neg_log_like;
    ocTree = octomap_utils::loadOctomap(octomap_fname.c_str(), &min_neg_log_like);
    printf("loaded using loadOctomap, min_neg_log_like = %f\n", min_neg_log_like);
  }
  else {
    ocTree = new OcTree(octomap_fname);
  }

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
      fprintf(stderr, ".");
      octomap_raw_t_publish(lcm, "OCTOMAP", &msg);
    }
  }
  fprintf(stderr, "done! \n");

  return 0;
}

