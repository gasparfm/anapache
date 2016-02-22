#include "gstr.hpp"
#include <iostream>

namespace GCommon
{
  std::string _defaultStartDelimiter = ":";
  std::string _defaultEndDelimiter = "";
  std::string whiteSpaces( " \f\n\r\t\v" );

  std::string replace(std::string source, std::string from, std::string to, int offset, int times)
  {
    int total = 0;
    std::string::size_type pos=offset;

    do
      {
	pos = source.find(from, pos);

	if (pos == std::string::npos)
	  break;

	source.replace(pos, from.length(), to);
	pos+=to.size();

      } while ( (times==0) || (++total<times) );

    return source;
  }

  std::string replace(std::string source, std::map<std::string,std::string>strMap, int offset, int times, bool delimiters, std::string before, std::string after)
  {
    int total = 0;
    std::string::size_type pos=offset;
    std::string::size_type newPos;
    std::string::size_type lowerPos;
    std::string::size_type delsize;

    if (strMap.size() == 0)
      return source;

    if (delimiters)
      delsize = before.length() + after.length();

    do
      {
	std::string rep;
	for (auto i=strMap.begin(); i!=strMap.end(); ++i)
	  {
	    auto fromStr = i->first;
	    newPos = (delimiters)?
	      source.find(before + fromStr + after, pos):
	      source.find(fromStr, pos);

	    if ( (i==strMap.begin()) || (newPos<lowerPos) )
	      {
		rep = fromStr;
		lowerPos = newPos;
	      }
	  }

	pos = lowerPos;
	if (pos == std::string::npos)
	  break;

	std::string toStr = strMap[rep];
	source.replace(pos, rep.length()+((delimiters)?delsize:0), toStr);
	pos+=toStr.size();

      } while ( (times==0) || (++total<times) );

    return source;
  }

  std::string replace(std::string source, std::map<std::string,std::string>strMap, int offset, int times, bool delimiters)
  {
    return (delimiters)?replace(source, strMap, offset, times, delimiters, _defaultStartDelimiter, _defaultEndDelimiter):
      replace(source, strMap, offset, times, delimiters, "");
  }

  void setDefaultDelimiters(std::string start, std::string end)
  {
    _defaultStartDelimiter = start;
    _defaultEndDelimiter = end;
  }

  std::string DefaultStartDelimiter()
  {
    return _defaultStartDelimiter;
  }

  std::string defaultEndDelimiter()
  {
    return _defaultEndDelimiter;
  }

  std::string DefaultStartDelimiter(std::string start)
  {
    _defaultStartDelimiter = start;
    return _defaultStartDelimiter;
  }

  std::string defaultEndDelimiter(std::string end)
  {
    _defaultEndDelimiter = end;
    return _defaultEndDelimiter;
  }

  void trimRight( std::string& str,
		  const std::string& trimChars)
  {
    std::string::size_type pos = str.find_last_not_of( trimChars );
    str.erase( pos + 1 );
  }

  void trimLeft( std::string& str,
		 const std::string& trimChars)
  {
    std::string::size_type pos = str.find_first_not_of( trimChars );
    str.erase( 0, pos );
  }

  void trim( std::string& str, const std::string& trimChars)
  {
    trimRight( str, trimChars );
    trimLeft( str, trimChars );
  }

}
