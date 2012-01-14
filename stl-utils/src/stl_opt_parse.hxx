#ifndef __STL_OPT_PARSE_DIRECT_INCLUDE__
#error "\nThis .hxx should not be included directly -- Include the .hpp instead!\n"
#endif

class OptBase {
public:
  OptBase(const std::string & _shortName, const std::string & _longName, const std::string & _description,
      bool _required) :
      shortName(_shortName), longName(_longName), description(_description), required(_required)
  { //TODO: assert that shortname is a char?
  }
  std::string shortName;
  std::string longName;
  std::string description;
  bool required;
  virtual bool parse(const std::string & next, bool &swallowed)=0;
};

template<typename T>
class OptType: public OptBase {
public:
  OptType(const std::string & _shortName, const std::string & _longName, const std::string & _description, T & _var_ref,
      bool _required) :
      OptBase(_shortName, _longName, _description, _required), var_ref(_var_ref)
  {
  }
  bool parse(const std::string & next, bool & swallowed)
  {
    swallowed = false;
    T tmp_var;
    std::istringstream ss(next);
    ss >> tmp_var;
    if (next.size() > 0 && ss.eof()) {
      var_ref = tmp_var;
      swallowed = true;
      return true;
    }
    else if (!required)
      return true;
    else {
      std::cerr << "ERROR: Could not parse " << next << " as value for " << longName << std::endl;
      return false;
    }
  }

  T & var_ref;
};

template<>
class OptType<bool> : public OptBase {
public:
  OptType(const std::string & _shortName, const std::string & _longName, const std::string & _description,
      bool & _var_ref,
      bool _required) :
      OptBase(_shortName, _longName, _description, _required), var_ref(_var_ref)
  {
  }
  bool parse(const std::string & next, bool & swallowed)
  {
    swallowed = false;
    bool tmp_var;
    std::istringstream ss(next);
    ss >> tmp_var;
    if (next.size() > 0 && ss.eof()) {
      var_ref = tmp_var;
      swallowed = true;
      return true;
    }
    else if (!required) {
      var_ref = true;
      return true;
    }
    else {
      std::cerr << "ERROR: Could not parse " << next << " as bool value for " << longName << std::endl;
      return false;
    }
  }

  bool & var_ref;
};

OptParse::OptParse(int _argc, char ** _argv, const std::string & _msg) :
    msg(_msg)
{
  progName = _argv[0];
  for (int i = 1; i < _argc; i++)
    argv.push_back(std::string(_argv[i]));
}

template<class T>
void OptParse::add(const std::string & shortName, const std::string & longName, const std::string & description,
    T & var_ref, bool required)
{
  opts.push_back(new OptType<T>(shortName, longName, description, var_ref, required));
}
//specialization for bool with no args
void OptParse::add(const std::string & shortName, const std::string & longName, const std::string & description,
    bool & var_ref)
{
  opts.push_back(new OptType<bool>(shortName, longName, description, var_ref, false));
}

std::list<std::string> OptParse::parse()
{
  size_t found;
  bool swallowed = false;
  for (std::list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) {
    OptBase * opt = *oit;
    for (std::list<std::string>::iterator ait = argv.begin(); ait != argv.end(); ait++) {
      const std::string & str = *ait;

      //search for long opt
      found = str.find("--" + opt->longName);
      if (found == 0) {
        size_t
        eq_found = str.find("=");
        if (eq_found != std::string::npos && eq_found + 1 < str.size()) {
          if (!opt->parse(str.substr(eq_found + 1), swallowed))
            usage(true);
        }
        else {
          std::list<std::string>::iterator next_ait = ait;
          next_ait++;
          if (next_ait != argv.end()) {
            if (!opt->parse(*next_ait, swallowed))
              usage(true);
          }
          else {
            if (!opt->parse("", swallowed))
              usage(true);
          }
          if (swallowed)
            argv.erase(next_ait); //option was processed, so remove it
        }
        ait = argv.erase(ait); //option was processed, so remove it
        break; //option was found
      }

      //search for short opt
      found = str.find("-" + opt->shortName);
      if (found == 0) { //found at start
        int shortNameLength = opt->shortName.size() + 1;
        if (str.size() > shortNameLength) {
          if (!opt->parse(str.substr(shortNameLength), swallowed))
            usage(true);
        }
        else {
          std::list<std::string>::iterator next_ait = ait;
          next_ait++;
          if (next_ait != argv.end()) {
            if (!opt->parse(*next_ait, swallowed))
              usage(true);
          }
          else {
            if (!opt->parse("", swallowed))
              usage(true);
          }
          if (swallowed)
            argv.erase(next_ait); //option was processed, so remove it

        }
        ait = argv.erase(ait); //option was processed, so remove it

        break; //option was found, so stop searching
      }
    }
  }
  return argv;
}
void OptParse::usage(bool ext)
{
  size_t found;
  found = progName.find_last_of("/");
  if (found != std::string::npos)
    progName = progName.substr(found + 1);

  std::cerr << "Usage:\n";
  std::cerr << "  $ " << progName << " [opts]\n";
  for (std::list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) { //todo: better alignment?
    OptBase * opt = *oit;
    std::cerr << "    -" << opt->shortName << "   --" << opt->longName << "   : " << opt->description << "\n";
  }
  std::cerr << "\n";
  if (ext) {
    exit(1);
  }

}

