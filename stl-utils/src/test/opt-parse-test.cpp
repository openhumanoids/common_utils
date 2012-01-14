#include <stdio.h>
//#include <stl_utils/stl_opt_parse.hpp>
#include "../stl_opt_parse.hpp"

using namespace stl_utils;

int main(int argc, char ** argv)
{
  OptParse parser(argc, argv, "map-name", "HURRAY!");

  int i = -9;
  bool b = false;
  float f = -9;
  std::string s = "asdf";
  parser.add("i", "iiii", i, "ayyy maytey", true);
  parser.add("b", "bzzz", b, "bbbbasdfasdf");
  parser.add("f", "ffff", f);
  parser.add("s", "ssss", s, "do strings work ?");

  std::list<std::string> remaining = parser.parse();

  std::cerr << "i:" << i << " b:" << b << " f:" << f << " s:" << s << "\n";
  std::cerr << "Remaining: ";
  for (std::list<std::string>::iterator it = remaining.begin(); it != remaining.end(); it++) {
    std::cerr << *it << " ";
  }
  std::cerr << "\n";

  return 0;
}
