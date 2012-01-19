#ifndef __STL_OPT_PARSE_DIRECT_INCLUDE__
#error "\nThis .hxx should not be included directly -- Include the .hpp instead!\n"
#endif

class OptParse::OptBase {
public:
  OptBase(const std::string & _shortName, const std::string & _longName, const std::string & _description,
      bool _required) :
      shortName(_shortName), longName(_longName), description(_description), required(_required), parsed(false)
  { //TODO: assert that shortname is a char?
  }
  std::string shortName;
  std::string longName;
  std::string description;
  bool required;
  bool parsed;
  virtual bool parse(const std::string & next, bool &swallowed)=0;
  virtual void print(int longOptWidth)=0;
  virtual std::string makeLongNameStr(int min_width = 0)=0;
};

//Generic type
template<typename T>
inline const std::string typenameToStr()
{
  return std::string("unkown");
}
// macro to implement specializations for given types
#define OPT_PARSE_MAKE_TYPENAME_TO_STRING( type ) \
    template<>\
    inline const std::string typenameToStr<type>() {\
       return std::string(#type);\
    }
OPT_PARSE_MAKE_TYPENAME_TO_STRING(double);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(float);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(int64_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(int32_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(int16_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(int8_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(uint64_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(uint32_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(uint16_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(uint8_t);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(bool);
OPT_PARSE_MAKE_TYPENAME_TO_STRING(std::string);

template<typename T>
class OptType: public OptParse::OptBase {
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
      parsed = true;
      var_ref = tmp_var;
      swallowed = true;
      return true;
    }
    else {
      std::cerr << "ERROR: Could not parse '" << next << "' as value for '" << longName << "'\n";
      return false;
    }
  }
  std::string makeLongNameStr(int min_width = 0)
  {
    using namespace std;
    string var_msg;
    if (required)
      var_msg = "--" + longName + " = <" + typenameToStr<T>() + ">";
    else
      var_msg = "--" + longName + " = [" + to_string(var_ref) + "]";

    stringstream s;
    s << left << setw(min_width) << var_msg << " ";
    string msg = s.str();
    return msg;
  }
  void print(int longOptWidth)
  {
    using namespace std;
    string req_msg = required ? "(REQ)" : "";
    string var_str = to_string(var_ref);
    cerr << "  -" << shortName << ", ";
    cerr << makeLongNameStr(longOptWidth);
    cerr << " : " << description << "\n"; // (def: [" << var_ref << "])\n";
  }

  T & var_ref;
};

//specialization for a boolean flag
template<>
bool OptType<bool>::parse(const std::string & next, bool & swallowed)
{
  parsed = true;
  swallowed = false;
  var_ref = !var_ref;
  return true;
}

OptParse::OptParse(int _argc, char ** _argv, const std::string & _extra_args, const std::string & _description,
    bool addHelpOption) :
    extra_args(_extra_args), description(_description), showHelp(false)
{
  progName = _argv[0];
  for (int i = 1; i < _argc; i++)
    argv.push_back(std::string(_argv[i]));
  if (addHelpOption)
    add(showHelp, "h", "help", "Display this help message");
}

OptParse::~OptParse()
{
  for (std::list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++)
    delete *oit;
}

template<class T>
void OptParse::add(T & var_ref, const std::string & shortName, const std::string & longName,
    const std::string & description, bool required)
{
  using namespace std;
  for (list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) {
    OptBase * opt = *oit;
    if (opt->shortName == shortName) {
      cerr << "ERROR: adding option (" << shortName << ", " << longName
          << "): conflicts with previous shortname in option:\n";
      opt->print(opt->longName.size());
      exit(1);
    }
    if (opt->longName == longName) {
      cerr << "ERROR: adding option (" << shortName << ", " << longName
          << "): conflicts with previous longName in option:\n";
      opt->print(opt->longName.size());
      exit(1);
    }
  }

  opts.push_back(new OptType<T>(shortName, longName, description, var_ref, required));
}

std::list<std::string> OptParse::parse()
{
  using namespace std;
  size_t found;
  bool swallowed = false;
  for (list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) {
    OptBase * opt = *oit;
    if (opt->shortName.size() == 0 && opt->longName.size() == 0)
      continue;
    for (list<string>::iterator ait = argv.begin(); ait != argv.end(); ait++) {
      const string & str = *ait;

      //search for long opt
      found = str.find("--" + opt->longName);
      if (found == 0) {
        size_t
        eq_found = str.find("=");
        if (eq_found != string::npos && eq_found + 1 < str.size()) {
          if (!opt->parse(str.substr(eq_found + 1), swallowed))
            usage(true);
        }
        else {
          list<string>::iterator next_ait = ait;
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
          list<string>::iterator next_ait = ait;
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
    if (opt->required && !opt->parsed) {
      cerr << "ERROR: option '" << opt->longName << "' is required!\n";
      usage(true);
    }
  }
  if (showHelp)
    usage(true);
  return argv;
}
void OptParse::usage(bool ext)
{
  using namespace std;
  size_t found;
  found = progName.find_last_of("/");
  if (found != string::npos)
    progName = progName.substr(found + 1);

  int maxLongOptLen = 0;
  for (list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) {
    OptBase * opt = *oit;
    int optPrintLen = opt->makeLongNameStr(0).size();
    if (optPrintLen > maxLongOptLen)
      maxLongOptLen = optPrintLen;
  }

  cerr << "Usage:\n";
  cerr << "  " << progName << " [opts] " << extra_args << "\n";
  if (description.size() > 0)
    cerr << " " << description << "\n";
  cerr << "Options:\n";
  for (list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) {
    OptBase * opt = *oit;
    if (opt->shortName.size() == 0 && opt->longName.size() == 0)
      cerr << opt->description << "\n";
    else
      opt->print(maxLongOptLen);
  }
  cerr << "\n";
  if (ext) {
    exit(1);
  }

}

bool OptParse::wasParsed(const std::string & name)
{
  using namespace std;
  for (list<OptBase *>::iterator oit = opts.begin(); oit != opts.end(); oit++) {
    OptBase * opt = *oit;
    if (opt->shortName == name || opt->longName == name)
      return opt->parsed;
  }
  cerr << "ERROR checking whether '" << name << "' was parsed. Not a valid option\n";
  usage(true);

}
