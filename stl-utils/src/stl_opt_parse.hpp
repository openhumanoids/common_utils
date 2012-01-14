#ifndef __STL_OPT_PARSE__
#define __STL_OPT_PARSE__

#include <stdlib.h>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>
//TODO: COMMENT ME BETTER!!!!!

namespace stl_utils {

class OptParse {
public:
  //setup the argument parser
  OptParse(int _argc, char ** _argv, const std::string & _extra_args = "", const std::string & _description = "");
  ~OptParse();
  //Add an argument handler
  template<class T>
  void add(const std::string & shortName, const std::string & longName, T & var_ref, const std::string & description =
      "",
      bool required = false);

  //do the actual parsing. The unparsed arguments will be returned
  std::list<std::string> parse();

  //print the usage
  void usage(bool ext = false);

private:
  std::string extra_args;
  std::string description;
  std::string progName;
  std::list<std::string> argv;
  class OptBase;
  std::list<OptBase *> opts;
  bool showHelp;
};

#define __STL_OPT_PARSE_DIRECT_INCLUDE__
#include "stl_opt_parse.hxx"
}

#endif
