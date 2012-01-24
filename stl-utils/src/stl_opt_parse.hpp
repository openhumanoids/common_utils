#ifndef __STL_OPT_PARSE__
#define __STL_OPT_PARSE__

#include <stdlib.h>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <map>
#include "stlstring_utils.hpp"
//TODO: COMMENT ME BETTER!!!!!

namespace stl_utils {

class OptParse {
public:
  //setup the argument parser
  //extra_args and description are only used for printing the usage.
  // Extra args should have the non option arguments that are expected
  // Description is a short blurb about the program
  // addHelpOption automatically add a -h/--help option that prints the usage
  OptParse(int _argc, char ** _argv, const std::string & _extra_args = "", const std::string & _description = "",
      bool addHelpOption = true);
  ~OptParse();

  //Add an argument handler
  template<class T>
  void add(T & var_ref, const std::string & shortName, const std::string & longName = "",
      const std::string & description = "", bool required = false);

  // do the parsing of flags, expecting handle 0-3 required arguments (IN ORDER) that don't have options flags
  // calls parseVarArg internally with the apropriate number of remaining arguments
  void parse();
  template<class T1>
  void parse(T1 & reqArg1);
  template<class T1, class T2>
  void parse(T1 & reqArg1, T2 & reqArg2);
  template<class T1, class T2, class T3>
  void parse(T1 & reqArg1, T2 & reqArg2, T3 & reqArg3);

  // do the actual parsing. The unparsed arguments will be returned
  // numRequired specifies the expected number of arguments that don't need a flag
  // if numRequired >=0, usage will be printed if the number of strings returned doesn't match
  std::list<std::string> parseVarArg(int numRequired = -1);

  // Check whether an option was parsed so that you could print something/do something special if it is/isn't
  // Must be a valid option.
  // Takes in either the long or short option
  bool wasParsed(const std::string & name);

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
