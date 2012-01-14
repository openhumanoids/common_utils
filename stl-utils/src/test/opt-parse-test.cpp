#include <stdio.h>
//#include <stl_utils/stl_opt_parse.hpp>
#include "../stl_opt_parse.hpp"

int main(int argc, char ** argv)
{
  OptParse parser(argc, argv, "HURRAY!");

  int i = -9;
  bool b = false;
  float f = -9;
  parser.add("i", "iiii", "ayyy maytey", i);
  parser.add("b", "bbbb", "bzzz", b);
  parser.add("f", "ffff", "fpfppff", f);

  std::list<std::string> remaining = parser.parse();

  std::cerr << "i:" << i << " b:" << b << " f:" << f << "\n";
  std::cerr << "Remaining: ";
  for (std::list<std::string>::iterator it = remaining.begin(); it != remaining.end(); it++) {
    std::cerr << *it << " ";
  }
  std::cerr << "\n";

  return 0;
}
