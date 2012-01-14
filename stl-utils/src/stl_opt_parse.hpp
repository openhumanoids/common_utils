#ifndef __STL_OPT_PARSE__
#define __STL_OPT_PARSE__

#include <stdlib.h>
#include <list>
#include <string>
#include <sstream>
#include <iostream>

class OptBase;
template<typename T> class OptType;

class OptParse {
public:
  OptParse(int _argc, char ** _argv, const std::string & _msg = "");

  template<class T>
  void add(const std::string & shortName, const std::string & longName, const std::string & description, T & var_ref,
      bool required = true);

  //specialization for bool with no args
  void add(const std::string & shortName, const std::string & longName, const std::string & description,
      bool & var_ref);
  std::list<std::string> parse();
  void usage(bool ext = false);

private:
  std::string msg;
  std::string progName;
  std::list<std::string> argv;
  std::list<OptBase *> opts;
};

#define __STL_OPT_PARSE_DIRECT_INCLUDE__
#include "stl_opt_parse.hxx"
#endif
