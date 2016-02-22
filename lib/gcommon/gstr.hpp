#pragma once

#include <string>
#include <map>

namespace GCommon
{
  std::string replace(std::string source, std::string from, std::string to, int offset=0, int times=0);
  std::string replace(std::string source, std::map<std::string,std::string>strMap, int offset=0, int times=0, bool delimiters=true);
  std::string replace(std::string source, const std::map<std::string,std::string>strMap, int offset, int times, bool delimiters, std::string before, std::string after="");
  extern std::string whiteSpaces;

  void trimRight( std::string& str,
		  const std::string& trimChars = whiteSpaces );
  void trimLeft( std::string& str,
		 const std::string& trimChars = whiteSpaces );
  void trim( std::string& str, const std::string& trimChars = whiteSpaces );

  void setDefaultDelimiters(std::string start, std::string end);
  std::string defaultStartDelimiter();
  std::string defaultEndDelimiter();
  std::string defaultStartDelimiter(std::string start);
  std::string defaultEndDelimiter(std::string end);
};
