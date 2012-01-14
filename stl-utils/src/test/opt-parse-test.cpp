#include <stdio.h>
//#include <stl_utils/stl_opt_parse.hpp>
#include "../stl_opt_parse.hpp"

using namespace stl_utils;

int main(int argc, char ** argv)
{
  OptParse parser(argc, argv, "map-name");

  int i = -9;
  bool b = false;
  float f = -9;
  std::string s = "asdf";
  parser.add(i, "i", "iiii", "ayyy maytey", true);
  parser.add(b, "b", "bzzz", "bbbbasdfasdf");
  parser.add(b, "");
  parser.add(f, "f", "ffff");
  parser.add(s, "s", "ssss", "do strings work ?");



  std::list<std::string> remaining = parser.parse();

  std::cerr << "i:" << i << " b:" << b << " f:" << f << " s:" << s << "\n";
  std::cerr << "Remaining: ";
  for (std::list<std::string>::iterator it = remaining.begin(); it != remaining.end(); it++) {
    std::cerr << *it << " ";
  }
  std::cerr << "\n";

  if (parser.wasParsed("b")) {
    std::cerr << "b was parsed!\n";
  }
  else {
    std::cerr << "b was NOT parsed!\n";
  }

  if (parser.wasParsed("ssss")) {
    std::cerr << "ssss was parsed! \n";
  }

  return 0;
}
