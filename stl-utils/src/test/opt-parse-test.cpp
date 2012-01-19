#include <stdio.h>
//#include <stl_utils/stl_opt_parse.hpp>
#include "../stl_opt_parse.hpp"

using namespace stl_utils;

int main(int argc, char ** argv)
{
  OptParse parser(argc, argv, "map-name");

  int i = -8;
  bool b = false;
  float f = -9;
  char c = 'o';
  std::string s = "asdf";
  parser.add(i, "i", "ints", "ayyy maytey", true);
  parser.add(i, "j", "ints2", "ayyy maytey");
  parser.add(b, "b", "bools", "bbbbasdfasdf");
  parser.add(b, "");
  parser.add(f, "f", "floats");
  parser.add(s, "s", "strings", "do strings work ?");
  parser.add(c, "c", "chars", "do chars work ?");

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
