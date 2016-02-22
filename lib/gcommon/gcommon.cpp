#include "gcommon.hpp"
#include "gstr.hpp"
#include "cfileutils.h"
#include <iostream>

namespace GCommon
{
  std::map<std::string, std::string> GlobalSettings::storage;
  const std::list<std::string> stdConfigPaths =
  {
    ":sname.:cfgext",
    "/etc/:sname/:sname.:cfgext",
    "/etc/:sname.:cfgext",
    "/etc/:sname/config.:cfgext"
  };


  namespace
  {
    bool _findFile(std::string &file, const std::list<std::string>& paths, const std::map<std::string, std::string>& replacements)
    {
      for (auto p : paths)
	{
	  p = replace(p, replacements);
	  if (file_exists(p.c_str())==1)
	    {
	      file = p;
	      return true;
	    }
	}
    }
  };

  bool findFile(std::string &file, const std::list<std::string>& paths, const std::map<std::string, std::string>& replacements)
  {
    return _findFile(file, paths, replacements);
    /* std::vector<std::string> paths = {"/etc/sermon/config.json", "/etc/sermon.json", "./sermon.json", "./config.json", "./config/sermon.json"}; */
    return false;
  }

  bool findConfigFile(std::string &file, const std::string& ext, const std::list<std::string>& paths)
  {
    auto configReplaces(GlobalSettings::all());
    configReplaces.insert({"cfgext", ext});
    return (!paths.empty())?_findFile(file, paths, GlobalSettings::all()):_findFile(file, stdConfigPaths, configReplaces);
  }

  std::string put_time(std::string format, time_t tim)
  {
    struct tm tm;
    if (tim==0)
      tim = time (NULL);

    localtime_r(&tim, &tm);
    return std::put_time(&tm, format.c_str());
  }

  void GlobalSettings::basics(std::string programName, std::string programShortName, std::string programFile, std::string programVersion)
  {
    storage.insert({"name", programName});
    storage.insert({"sname", programShortName});
    storage.insert({"exec", programFile});
    storage.insert({"version", programVersion});
  }

  void GlobalSettings::set(std::string key, std::string value)
  {
    storage.insert({key, value});
  }

  std::string GlobalSettings::get(std::string key)
  {
    return storage[key];
  }

  const std::map<std::string, std::string> &GlobalSettings::all()
  {
    return storage;
  }

};
