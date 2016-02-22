#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include "gstr.hpp"
#include <ctime>

#if defined(__GNUC__) && (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ <= 50200 )
  namespace std
  {
    static std::string put_time( const std::tm* tmb, const char* fmt )
    {
      std::string s( 128, '\0' );
      size_t written;
      while( !(written=strftime( &s[0], s.size(), fmt, tmb ) ) )
        s.resize( s.size() + 128 );
      s[written] = '\0';

      return s.c_str();
    }
  }
#endif

namespace GCommon
{
  bool findFile(std::string &configFile, const std::list<std::string>& paths, const std::map<std::string, std::string>& replacements={});
  bool findConfigFile(std::string &file, const std::string& ext, const std::list<std::string>& paths = {});
  std::string put_time(std::string format, time_t tim=0);

  class GlobalSettings
  {
  public:
    static void basics(std::string programName, std::string programShortName, std::string programFile, std::string programVersion);
    static void set(std::string key, std::string value);
    static std::string get(std::string key);
    static const std::map<std::string, std::string> &all();

  private:
    static std::map<std::string, std::string> storage;
  };
};
